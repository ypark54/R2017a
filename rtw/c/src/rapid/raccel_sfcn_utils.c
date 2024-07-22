/******************************************************************
 *  File: raccel_sfcn_utils.c
 *
 *  Abstract:
 *      - functions for dynamically loading s-function mex files
 *
 * Copyright 2007-2016 The MathWorks, Inc.
 ******************************************************************/

/* INCLUDES */
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <math.h>
#include  <float.h>
#include  <ctype.h>
#include  <setjmp.h>

#ifdef _WIN32
#include <windows.h>
#else
#include  <dlfcn.h>
#endif

/*
 * We want access to the real mx* routines in this file and not their RTW
 * variants in rt_matrx.h, the defines below prior to including simstruc.h
 * accomplish this.
 */

#include  "mat.h"
#define TYPEDEF_MX_ARRAY
#define rt_matrx_h
#include "simstruc.h"
#undef rt_matrx_h
#undef TYPEDEF_MX_ARRAY

#include "dt_info.h"
#include "sl_datatype_access.h"

#include "raccel_sfcn_utils.h"
#include "slsv_mexhelper.h"

typedef struct SFcnInfo_T
{
    const char* name;
    const char* mexPath;
    mxArray** dialogPrms;
    void* mexHandle;
    boolean_T* stringParameters;
    const char* blockSID;
} SFcnInfo;

size_t gblNumSFcns = 0;
size_t gblCurrentSFcnIdx = 0;
size_t gblNumMexFileSFcns = 0;
size_t* gblChildIdxToInfoIdx = NULL;
static SFcnInfo* gblSFcnInfo = NULL;

extern jmp_buf gblRapidAccelSFcnJmpBuf;
extern const char* gblSFcnInfoFileName;
extern const char* gblErrorStatus;

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* get s-function info (names, paths, parameters etc.) */
void
raccelInitializeForMexSFcns()
{
    const char* errStr = NULL;
    MATFile *pmat = NULL;
    mxArray *mxFeatureValue = NULL;
    mxArray *sFcnInfoStruct = NULL;
    
    int_T featureValue;

    if (gblSFcnInfoFileName == NULL)
        return;

    if ((pmat = matOpen(gblSFcnInfoFileName, "r")) == NULL)
    {
        errStr = "could not find MAT-file containing s-function info";
        goto EXIT_POINT;
    }

    if ((mxFeatureValue = matGetNextVariable(pmat, NULL)) == NULL)
    {
        errStr = "error reading feature value from s-function info MAT-file";
        goto EXIT_POINT;
    }

    if (!mxIsDouble(mxFeatureValue) ||
        !mxIsScalar(mxFeatureValue))
    {
        errStr = "feature value must be a double scalar";
        goto EXIT_POINT;
    }        

    featureValue = (int)((mxGetPr(mxFeatureValue))[0]);

    if (featureValue != 1 &&
        featureValue != 2)
    {
        errStr = "invalid feature value";
        goto EXIT_POINT;
    }

    if ((sFcnInfoStruct = matGetNextVariable(pmat, NULL)) == NULL)
    {
        errStr = "error reading s-function info structure from s-function info MAT-file";
        goto EXIT_POINT;
    }

    if (!mxIsStruct(sFcnInfoStruct) ||
        mxGetM(sFcnInfoStruct) != 1) 
    {
        errStr = "RTP must be a structure whose first dimension is 1";
        goto EXIT_POINT;
    }

    gblNumMexFileSFcns = 0;
    gblNumSFcns = mxGetN(sFcnInfoStruct);
    gblSFcnInfo = (SFcnInfo*) calloc(gblNumSFcns, sizeof(SFcnInfo));
    gblChildIdxToInfoIdx = (size_t*) calloc(gblNumSFcns, sizeof(size_t));

    {
        mxArray *temp =
            NULL;
        boolean_T sourceCodeExists;
        size_t sfIdx;

        if (featureValue == 1)
        {
            /* use mex-file only when source code is not available */
            for (sfIdx = 0;
                 sfIdx < gblNumSFcns;
                 ++sfIdx)
            {
                temp = mxGetField(sFcnInfoStruct, sfIdx, "sourceCodeExists");
                sourceCodeExists = mxIsLogicalScalarTrue(temp);

                if (!sourceCodeExists)
                {
                    gblNumMexFileSFcns++;
                    temp = mxGetField(sFcnInfoStruct, sfIdx, "mexPath");
                    gblSFcnInfo[sfIdx].mexPath = mxArrayToString(temp);
                    temp = mxGetField(sFcnInfoStruct, sfIdx, "name");                    
                    gblSFcnInfo[sfIdx].name = mxArrayToString(temp);
                    temp = mxGetField(sFcnInfoStruct, sfIdx, "blockSID");
                    gblSFcnInfo[sfIdx].blockSID = mxArrayToString(temp);
                    
                    temp = mxGetField(sFcnInfoStruct, sfIdx, "stringParameters");
                    if (temp != NULL)
                    {
                        size_t numDialogPrms =
                            mxGetN(temp);
                        
                        mxLogical* pl =
                            mxGetLogicals(temp);
                        
                        gblSFcnInfo[sfIdx].stringParameters =
                            calloc(numDialogPrms, sizeof(boolean_T));

                        {
                            size_t prmIdx = 0;
                            for (prmIdx = 0;
                                 prmIdx < numDialogPrms;
                                 prmIdx++)
                            {
                                gblSFcnInfo[sfIdx].stringParameters[prmIdx] =
                                    (boolean_T) pl[prmIdx];
                            }
                        }
                    }
                    
                }
            }
        } else if (featureValue == 2) {
            /* always use mex-file */
            gblNumMexFileSFcns = gblNumSFcns;
            
            for (sfIdx = 0;
                 sfIdx < gblNumSFcns;
                 ++sfIdx)
            {
                temp = mxGetField(sFcnInfoStruct, sfIdx, "mexPath");
                gblSFcnInfo[sfIdx].mexPath = mxArrayToString(temp);
                temp = mxGetField(sFcnInfoStruct, sfIdx, "name");
                gblSFcnInfo[sfIdx].name = mxArrayToString(temp);
                temp = mxGetField(sFcnInfoStruct, sfIdx, "blockSID");
                gblSFcnInfo[sfIdx].blockSID = mxArrayToString(temp);
                
                temp = mxGetField(sFcnInfoStruct, sfIdx, "stringParameters");

                if (temp != NULL)
                {
                    size_t numDialogPrms =
                        mxGetN(temp);
                        
                    mxLogical* pl =
                        mxGetLogicals(temp);
                        
                    gblSFcnInfo[sfIdx].stringParameters =
                        calloc(numDialogPrms, sizeof(boolean_T));

                    {
                        size_t prmIdx = 0;
                        for (prmIdx = 0;
                             prmIdx < numDialogPrms;
                             prmIdx++)
                        {
                            gblSFcnInfo[sfIdx].stringParameters[prmIdx] =
                                (boolean_T) pl[prmIdx];
                        }
                    }
                }                
            }
        }
    }


    if (gblNumMexFileSFcns > 0)
    {
        /* SLINTERNAL_RAPID_ACCEL_TARGET_IS_ACTIVE  is set in rapid_accel_target_utils before launching */
        /* the rapid accelerator executable; it is used to prevent rapid accelerator from automatically */
        /* loading libmwsimulink. It is disabled here so that dynamically loaded s-functions that link */
        /* against libmwsimulink will not cause rapid accelerator to crash. */

#if defined(_WIN32)
        _putenv_s("SLINTERNAL_RAPID_ACCEL_TARGET_IS_ACTIVE", "0");
#else
        setenv("SLINTERNAL_RAPID_ACCEL_TARGET_IS_ACTIVE", "0", 1);
#endif

        /* Error handling for calls to the mex API (e.g. mexCallMATLAB) */        
        setMexCallbacks();
    }
    
  EXIT_POINT:
    mxDestroyArray(mxFeatureValue);
    mxDestroyArray(sFcnInfoStruct);

    if (pmat!= NULL)
    {
        matClose(pmat);
        pmat = NULL;
    }

    return;
}


/* ////////////////////////////////////////////////////////////////////////////////////////// */
int
raccelBlockSIDToInfoIdx(
    const char* blockSIDToMatch,
    SimStruct* root,
    const char* sFcnName,
    const char* mdlName)
{
    size_t idx = 0;
    for (idx = 0; idx < gblNumSFcns; idx++)
    {
        const char* blockSID =
            gblSFcnInfo[idx].blockSID;

        if (blockSID != NULL)
        {
            int result =
                strcmp(blockSID, blockSIDToMatch);

            if (result == 0)
            {
                return idx;
            }
        }
    }

    /* couldn't find blockSID; error out (in raccel_register_model) */
    {
        const char* errTemplate =
            "<diag_root><diag id=\"Simulink:tools:rapidAccelCouldNotFindBlockSIDForSFcn\"><arguments><arg type = \"string\">%s</arg><arg type = \"string\">%s</arg></arguments></diag></diag_root>";

        char* message =
            (char *) calloc(1024, sizeof(char));
    
        sprintf(message,
                errTemplate,
                sFcnName,
                mdlName);

        ssSetErrorStatus(root, (const char*) message);

        return -1;
    }    
}


/* ////////////////////////////////////////////////////////////////////////////////////////// */
void
raccelSFcnLongJumper(const char* errMsg)
{
    gblErrorStatus = errMsg;
    
    longjmp(gblRapidAccelSFcnJmpBuf, 1);
}


/* ////////////////////////////////////////////////////////////////////////////////////////// */
#ifndef MAXSTRLEN
#define MAXSTRLEN 509
#endif

void
convertRealArrayToCharArray(
    real_T* r,
    size_t nchars,
    char_T *c)
{
    if (nchars >= MAXSTRLEN)
    {
        nchars = MAXSTRLEN - 1;
    }

    while (nchars--)
    {
        *c++ = (char) (*r++ + .5);
    }

    *c = '\0';
}


/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* mex-file s-functions expect dialog parameters in the form of mxarrays; this function creates 
 * an mxarray for each dialog parameter and copies the value of the parameter from the rtP */
void
raccelInitializeGblDialogPrms(
    SimStruct* child,
    size_t infoIdx)
{
    size_t nPrms =
        ssGetSFcnParamsCount(child);
    real_T** rtpPtr =
        (real_T**) ssGetSFcnParamsPtr(child);

    if (nPrms == 0) return;

    gblSFcnInfo[infoIdx].dialogPrms =
        (mxArray**) mxMalloc(sizeof(mxArray*) * nPrms);
    
    {
        size_t prmIdx;
        for (prmIdx = 0; prmIdx < nPrms; ++prmIdx)
        {
            real_T* rtpParam = rtpPtr[prmIdx];
            size_t m = (size_t) rtpParam[0];
            size_t n = (size_t) rtpParam[1];
            real_T* prmData = &rtpParam[2];

            if (!gblSFcnInfo[infoIdx].stringParameters[prmIdx])
            {
                gblSFcnInfo[infoIdx].dialogPrms[prmIdx] =
                    mxCreateDoubleMatrix(m, n, mxREAL);

                memcpy(
                    (real_T*) mxGetData(gblSFcnInfo[infoIdx].dialogPrms[prmIdx]),
                    prmData,
                    sizeof(real_T) * m * n);
            } else {
                char_T* prmString =
                    (char_T*) calloc(MAXSTRLEN, sizeof(char));

                convertRealArrayToCharArray(
                    prmData,
                    n,
                    prmString);

                gblSFcnInfo[infoIdx].dialogPrms[prmIdx] =
                    mxCreateString(prmString);
            }
        }
    }
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* make the s-function simstruct-parameter-pointer point to gblSFcnInfo, and copy parameter   */
/* values from the rtp to gblSFcnInfo (this function is used only if slfeature(               */
/* RapidAcceleratorSFcnMexFileLoadingCopyParameters) == 1) */
void
raccelSetSFcnParamPtrToGblAndCopyParamData(
    SimStruct* child,
    size_t infoIdx,
    const real_T** rtpPtr)
{
    size_t nPrms = ssGetSFcnParamsCount(child);
    {
        size_t prmIdx;
        for (prmIdx = 0;
             prmIdx < nPrms;
             ++prmIdx)
{
            size_t m =
                (size_t) rtpPtr[prmIdx][0];
            size_t n =
                (size_t) rtpPtr[prmIdx][1];
            const real_T *data =
                &rtpPtr[prmIdx][2];
            real_T *gblPrmData =
                (real_T*) mxGetData(gblSFcnInfo[infoIdx].dialogPrms[prmIdx]);
            
            memcpy(gblPrmData, data, sizeof(real_T) * m * n);
        }
    }
    
    ssSetSFcnParamsPtr(child, gblSFcnInfo[infoIdx].dialogPrms);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* make the s-function simstruct-parameter-pointer point to gblSFcnInfo, and copy parameter   */
/* values from the rtp to gblSFcnInfo (this function is used only if slfeature(               */
/* RapidAcceleratorSFcnMexFileLoadingCopyParameters) == 1) */
void
raccelSetSFcnParamPtrToGbl(
    SimStruct* child,
    size_t infoIdx)
{
    ssSetSFcnParamsPtr(child, gblSFcnInfo[infoIdx].dialogPrms);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* unload mex-files */
void
raccelUnloadSFcnLibrariesAndFreeAssociatedGbls(SimStruct* parent) 
{
    size_t sfIdx;
    for (sfIdx = 0;
         sfIdx < gblNumMexFileSFcns;
         ++sfIdx)
    {
        if (gblSFcnInfo[sfIdx].mexHandle != NULL)
        {
#ifdef _WIN32
            FreeLibrary(gblSFcnInfo[sfIdx].mexHandle);
#else
            dlclose(gblSFcnInfo[sfIdx].mexHandle);
#endif      
        }

        {
            if (gblSFcnInfo[sfIdx].dialogPrms != NULL)
            {
                SimStruct* child =
                    ssGetSFunction(parent, sfIdx);
                size_t nPrms =
                    ssGetSFcnParamsCount(child);
                
                if (nPrms != 0)
                {
                    mxFree(gblSFcnInfo[sfIdx].dialogPrms);
                }
            }
        }
    }

    free(gblSFcnInfo);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* data type access handling */

void
raccelDataTypeAccessHandler(const char* method)
{
    const char* name =
        gblSFcnInfo[gblCurrentSFcnIdx].name;

    const char* errTemplate =
        "<diag_root><diag id=\"Simulink:tools:rapidAccelSFcnDataTypeAccess\"><arguments><arg type = \"string\">%s</arg><arg type = \"string\">%s</arg></arguments></diag></diag_root>";
    

    char* message =
        (char *) calloc(1024, sizeof(char));
    
    sprintf(message,
            errTemplate,
            name,
            method);

    raccelSFcnLongJumper((const char*) message);
}

DTypeId
raccelDTARegisterDataType(
    void * v,
    const char_T * c1,
    const char_T * c2)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(c2);
    
    raccelDataTypeAccessHandler("ssRegisterDataType");
    return -1;
}

int_T
raccelDTAGetNumDataTypes(void * v)
{
    UNUSED_PARAMETER(v);
    
    raccelDataTypeAccessHandler("ssGetNumDataTypes");    
    return -1;
}

DTypeId
raccelDTAGetDataTypeId(
    void * v,
    const char_T * c1)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    
    raccelDataTypeAccessHandler("ssGetDataTypeId");
    return -1;
}

int_T
raccelDTAGetGenericDTAIntProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    GenDTAIntPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTAIntProp");
    return -1;
}

int_T
raccelDTASetGenericDTAIntProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    int_T i,
    GenDTAIntPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(i);    
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTAIntProp");
    return -1;
}

const void*
raccelDTAGetGenericDTAVoidProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    GenDTAVoidPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTAVoidProp");
    return NULL;
}

int_T
raccelDTASetGenericDTAVoidProp(
    void * v1,
    const char_T * c1,
    DTypeId d,
    const void * v2,
    GenDTAVoidPropType g)
{
    UNUSED_PARAMETER(v1);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(v2);    
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTAVoidProp");
    return -1;
}

GenericDTAUnaryFcn
raccelDTAGetGenericDTAUnaryFcnGW(
    void * v,
    const char_T * c1,
    DTypeId d,
    GenDTAUnaryFcnType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTAUnaryFcnGW");
    return NULL;
}

int_T
raccelDTASetGenericDTAUnaryFcnGW(
    void * v,
    const char_T * c1,
    DTypeId d,
    GenericDTAUnaryFcn f,
    GenDTAUnaryFcnType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(f);        
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTAUnaryFcnGW");
    return -1;
}

GenericDTABinaryFcn
raccelDTAGetGenericDTABinaryFcnGW(
    void * v,
    const char_T * c1,
    DTypeId d,
    GenDTABinaryFcnType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTABinaryFcnGW");
    return NULL;
}

int_T
raccelDTASetGenericDTABinaryFcnGW(
    void * v,
    const char_T * c1,
    DTypeId d,
    GenericDTABinaryFcn f,
    GenDTABinaryFcnType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(f);        
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTABinaryFcnGW");
    return -1;
}

ConvertBetweenFcn
raccelDTAGetConvertBetweenFcn(
    void * v,
    const char_T * c1,
    DTypeId d)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    raccelDataTypeAccessHandler("dtaGetConvertBetweenFcn");
    return NULL;
}

int_T
raccelDTASetConvertBetweenFcn(
    void * v,
    const char_T * c1,
    DTypeId d,
    ConvertBetweenFcn f)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(f);
    
    raccelDataTypeAccessHandler("dtaSetConvertBetweenFcn");
    return -1;
}

int_T
raccelDTAGetGenericDTADiagnostic(
    void * v,
    const char_T * c1,
    GenDTADiagnosticType g,
    BDErrorValue * b)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(g);
    UNUSED_PARAMETER(b);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTADiagnostic");
    return -1;
}

void *
raccelDTARegisterDataTypeWithCheck(
    void * v,
    const char_T * c1,
    const char_T * c2,
    DTypeId * d)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(c2);
    UNUSED_PARAMETER(d);
    
    raccelDataTypeAccessHandler("dtaRegisterDataTypeWithCheck");
    return NULL;
}

int_T
raccelDTAGetGenericDTAIntElemProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    int_T i,
    GenDTAIntElemPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(i);        
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTAIntElemProp");
    return -1;
}

int_T
raccelDTASetGenericDTAIntElemProp(
    void * v,
    const char_T * c1 ,
    DTypeId d,
    int_T i,
    int_T j,
    GenDTAIntElemPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(i);
    UNUSED_PARAMETER(j);    
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTAIntElemProp");
    return -1;
}

const void*
raccelDTAGetGenericDTAVoidElemProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    int_T i,
    GenDTAVoidElemPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(i);        
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTAVoidElemProp");
    return NULL;
}

int_T
raccelDTASetGenericDTAVoidElemProp(
    void * v1,
    const char_T * c1,
    DTypeId d,
    int_T i,
    const void * v2,
    GenDTAVoidElemPropType g)
{
    UNUSED_PARAMETER(v1);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(i);    
    UNUSED_PARAMETER(v2);        
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTAVoidElemProp");
    return -1;
}

real_T
raccelDTAGetGenericDTARealElemProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    int_T i,
    GenDTARealElemPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);            
    UNUSED_PARAMETER(i);
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaGetGenericDTARealElemProp");
    return -1;
}

int_T
raccelDTASetGenericDTARealElemProp(
    void * v,
    const char_T * c1,
    DTypeId d,
    int_T i,
    int_T j,
    GenDTARealElemPropType g)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(d);            
    UNUSED_PARAMETER(i);
    UNUSED_PARAMETER(j);
    UNUSED_PARAMETER(g);
    
    raccelDataTypeAccessHandler("dtaSetGenericDTARealElemProp");
    return -1;
}

slDataTypeAccess *
createDataTypeAccess()
{
    slDataTypeAccess * dta =
        (slDataTypeAccess *) calloc(1, sizeof(slDataTypeAccess));

    dta->dataTypeTable = NULL;
    dta->errorString = NULL;
    
    dta->registerFcn =
        raccelDTARegisterDataType;
    
    dta->getNumDataTypesFcn =
        raccelDTAGetNumDataTypes;
    
    dta->getIdFcn =
        raccelDTAGetDataTypeId;
    
    dta->getGenericDTAIntProp =
        raccelDTAGetGenericDTAIntProp;
    
    dta->setGenericDTAIntProp =
        raccelDTASetGenericDTAIntProp;
        
    dta->getGenericDTAVoidProp =
        raccelDTAGetGenericDTAVoidProp;
        
    dta->setGenericDTAVoidProp =
        raccelDTASetGenericDTAVoidProp;
    
    dta->getGenericDTAUnaryFcnGW =
        raccelDTAGetGenericDTAUnaryFcnGW;
        
    dta->setGenericDTAUnaryFcnGW =
        raccelDTASetGenericDTAUnaryFcnGW;
        
    dta->getGenericDTABinaryFcnGW =
        raccelDTAGetGenericDTABinaryFcnGW;
        
    dta->setGenericDTABinaryFcnGW =
        raccelDTASetGenericDTABinaryFcnGW;
        
    dta->getConvertBetweenFcn =
        raccelDTAGetConvertBetweenFcn;
        
    dta->setConvertBetweenFcn =
        raccelDTASetConvertBetweenFcn;
    
    dta->getGenericDTADiagnostic =
        raccelDTAGetGenericDTADiagnostic;
        
    dta->registerFcnWithCheck =
        raccelDTARegisterDataTypeWithCheck;
        
    dta->getGenericDTAIntElemProp =
        raccelDTAGetGenericDTAIntElemProp;
        
    dta->setGenericDTAIntElemProp =
        raccelDTASetGenericDTAIntElemProp;
        
    dta->getGenericDTAVoidElemProp =
        raccelDTAGetGenericDTAVoidElemProp;
        
    dta->setGenericDTAVoidElemProp =
        raccelDTASetGenericDTAVoidElemProp;
    
    dta->getGenericDTARealElemProp =
        raccelDTAGetGenericDTARealElemProp;
        
    dta->setGenericDTARealElemProp =
        raccelDTASetGenericDTARealElemProp;

    return dta;
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* setting up various parts of the child simstruct */
int_T
raccelSetRegNumInputPortsFcn(
    void* arg1,
                             int_T nInputPorts) 
{
    SimStruct* child =
        (SimStruct*) arg1;

    child->sizes.in.numInputPorts =
        nInputPorts;

    return 1;
}

int_T
raccelSetRegNumOutputPortsFcn(
    void* arg1,
                              int_T nOutputPorts) 
{
    SimStruct* child =
        (SimStruct*) arg1;
    
    child->sizes.out.numOutputPorts =
        nOutputPorts;
    
    return 1;
}

void
setupPortInfo(SimStruct* child)
{
    child->portInfo.regNumInputPortsFcn =
        raccelSetRegNumInputPortsFcn;
    
    child->portInfo.regNumInputPortsFcnArg =
        child;
    
    child->portInfo.regNumOutputPortsFcn =
        raccelSetRegNumOutputPortsFcn;
    
    child->portInfo.regNumOutputPortsFcnArg =
        child;
}

#define RACCEL_SFCNPARAM_TUNABLE         (1 << 0x1)
void
setupParamInfo(SimStruct* child)
{
    size_t nParams =
        ssGetNumSFcnParams(child);

    child->sfcnParams.dlgAttribs =
        calloc(nParams, sizeof(uint_T));
    
    {
        size_t loopIdx;
        
        for (loopIdx=0;
             loopIdx < nParams;
             ++loopIdx)
        {
            child->sfcnParams.dlgAttribs[loopIdx] =
                RACCEL_SFCNPARAM_TUNABLE;
        }
    }
}

void
setupDataTypeAccess(SimStruct * child)
{
    slDataTypeAccess * dta =
        createDataTypeAccess();

    ssSetDataTypeAccess(child, dta);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* generic function handling */
int_T
raccelGenericFcn(
    SimStruct * S,
    GenFcnType genFcnType,
    int_T arg1,
    void * arg2)
{
    switch(genFcnType)
    {

        /* ////////////////////////////////////////////////////////////////////////////////////////// */
        /* vardims */
        /* case GEN_FCN_SET_INPUT_DIMS_MODE: */
        /*   break; */

        /* case GEN_FCN_SET_OUTPUT_DIMS_MODE: */
        /*   break; */
        
      case GEN_FCN_SET_CURR_OUTPUT_DIMS:
        {
            /* _ssVarDimsIdxVal declared in simstruc.h */
            struct _ssVarDimsIdxVal_tag dimIdxVal =
                *((struct _ssVarDimsIdxVal_tag *) arg2);

            int dimIdx =
                dimIdxVal.dIdx;
            
            int dimVal =
                dimIdxVal.dVal;
            
            S->blkInfo.blkInfo2->portInfo2->outputs[arg1].portVarDims[dimIdx] 
                = dimVal;

            break;
        }

        /* case GEN_FCN_REG_SET_INPUT_DIMS_MODE_MTH: */
        /*   break; */

        /* case GEN_FCN_SET_INPUT_DIMS_SAMEAS_OUTPUT: */
        /*   break; */

        /* case GEN_FCN_SET_COMP_VARSIZE_COMPUTE_TYPE: */
        /*   break; */

        /* case GEN_FCN_ADD_DIMS_DEPEND_RULE: */
        /*   break; */
    }

    return 0;
}

void
setupGenericFcn(SimStruct * child)
{
    ssSetGenericFcn(child, raccelGenericFcn);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* set up the regDataType field of the s-function simstruct */
/* ssRegisterTypeFromExpr, ssRegisterTypeFromExprNoError, ssRegisterTypeFromNamedObject and ssRegisterTypeFromParameter */
/* are not here because they are pound-defined in simstruc.h to do nothing */
void
raccelRegDataTypeHandler(const char* method)
{
    const char* name =
        gblSFcnInfo[gblCurrentSFcnIdx].name;

    const char* errTemplate =
        "<diag_root><diag id=\"Simulink:tools:rapidAccelSFcnRegDataType\"><arguments><arg type = \"string\">%s</arg><arg type = \"string\">%s</arg></arguments></diag></diag_root>";        

    char* message =
        (char *) calloc(1024, sizeof(char));
    
    sprintf(message,
            errTemplate,
            name,
            method);

    raccelSFcnLongJumper((const char*) message);
}

DTypeId
raccelRDTRegisterFcn(
    void *v,
    const char *c)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c);
    raccelRegDataTypeHandler("ssRegisterDataType");
}

int_T
raccelRDTSetDataTypeSize(
    void *v,
    DTypeId d,
    int_T i)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(i);
    raccelRegDataTypeHandler("ssSetDataTypeSize");
}

int_T
raccelRDTGetDataTypeSize(
    void *v,
    DTypeId d)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(d);
    raccelRegDataTypeHandler("ssGetDataTypeSize");
}

int_T
raccelRDTSetDataTypeZero(
    void *v1,
    DTypeId d,
    void *v2)
{
    UNUSED_PARAMETER(v1);
    UNUSED_PARAMETER(d);
    UNUSED_PARAMETER(v2);
    raccelRegDataTypeHandler("ssSetDataTypeZero");
}

const void *
raccelRDTGetDataTypeZero(
    void *v,
    DTypeId d)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(d);
    raccelRegDataTypeHandler("ssGetDataTypeZero");
}

const char_T *
raccelRDTGetDataTypeName(
    void *v,
    DTypeId d)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(d);
    raccelRegDataTypeHandler("ssGetDataTypeName");
}

DTypeId
raccelRDTGetDataTypeId(
    void *v,
    const char_T *c)
{
    UNUSED_PARAMETER(v);
    UNUSED_PARAMETER(c);
    raccelRegDataTypeHandler("ssGetDataTypeId");
}

int_T
raccelRDTSetNumDWorkFcn(
    SimStruct * s,
    int_T i)
{
    s->sizes.numDWork = i;
    /* UNUSED_PARAMETER(s); */
    /* UNUSED_PARAMETER(i); */
    /* raccelRegDataTypeHandler(); */
}

void
setupRegDataType(SimStruct * child)
{
    struct _ssRegDataType * rdt =
        (struct _ssRegDataType *) calloc(1, sizeof(struct _ssRegDataType));

    rdt->arg1 = NULL;

    rdt->registerFcn =
        raccelRDTRegisterFcn;

    rdt->setSizeFcn =
        raccelRDTSetDataTypeSize;

    rdt->getSizeFcn =
        raccelRDTGetDataTypeSize;

    rdt->setZeroFcn =
        raccelRDTSetDataTypeZero;

    rdt->getZeroFcn =
        raccelRDTGetDataTypeZero;

    rdt->getNameFcn =
        raccelRDTGetDataTypeName;

    rdt->getIdFcn =
        raccelRDTGetDataTypeId;

    rdt->setNumDWorkFcn =
        raccelRDTSetNumDWorkFcn;

    child->regDataType = *rdt;
    
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* set up specific fields of the s-function simstruct */
void
setupSFcnSimstruct(SimStruct* child) 
{
    setupPortInfo(child);
    setupParamInfo(child);
    setupDataTypeAccess(child);
    setupGenericFcn(child);
    setupRegDataType(child);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* mexfunction flags */
#define RACCEL_SFCN_NLHS  5
#define RACCEL_RHS_T 0
#define RACCEL_RHS_X 1
#define RACCEL_RHS_U 2
#define RACCEL_RHS_FLAG 3
#define RACCEL_RHS_P1 4
#define RACCEL_SFCNPARAM_TUNABLE (1 << 0x1)

typedef void (*MexFunctionPtr)(
    int_T, 
    mxArray*[],
    int_T,
    const mxArray *[]
    );

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* setting up the call to s-function mexFunctions */
void callSFcnMexFunction(SimStruct* parent,
                         SimStruct** childPtr, 
                         MexFunctionPtr mexFunctionHandle) 
{
    int_T nPrms =
        ssGetSFcnParamsCount(*childPtr);
    int_T nrhs =
        4 + nPrms;
    mxArray* plhs[RACCEL_SFCN_NLHS] =
        {NULL};
    
    const mxArray** prhs;
    double *pr;
    size_t nEl;


    /* mimicking CreateSFcnRHSArgs */
    if ((prhs = (const mxArray **) calloc(nrhs, sizeof(mxArray*))) == NULL)
    { 
        ssSetErrorStatus(
            parent,
            "error during memory allocation for mexFunction return arguments");
        
        return;
    }

    if ((prhs[RACCEL_RHS_T] = mxCreateDoubleMatrix(0,0,0)) == NULL)
    { 
        ssSetErrorStatus(
            parent,
            "error during memory allocation for mexFunction return arguments");
        
        return;
    }

    nEl = (sizeof(SimStruct*)/sizeof(int)+1);
    
    if ((prhs[RACCEL_RHS_X] = mxCreateDoubleMatrix(nEl, 1, 0)) == NULL)
    {
        ssSetErrorStatus(
            parent,
            "error during memory allocation for mexFunction return arguments");
        
        return;
    }   
    
    pr = mxGetPr(prhs[RACCEL_RHS_X]);
     
    {
        void *voidChildPtr =
            (void *) (*childPtr);
        int *intPtr =
            (int*) &voidChildPtr;
        size_t loopIdx;

        for (loopIdx = 0;
             loopIdx < nEl-1;
             loopIdx++)
        {
            pr[loopIdx] = (double)(intPtr[loopIdx]);
        }
    }
    
    pr[nEl-1] = (double) SIMSTRUCT_VERSION_LEVEL2;

    if ((prhs[RACCEL_RHS_U] = mxCreateDoubleMatrix(0,0,0)) == NULL)
    { 
        ssSetErrorStatus(
            parent,
            "error during memory allocation for mexFunction return arguments");
        
        return;
    }

    if ((prhs[RACCEL_RHS_FLAG] = mxCreateDoubleMatrix(1,1,0)) == NULL)
    {
        ssSetErrorStatus(
            parent,
            "error during memory allocation for mexFunction return arguments");
        
        return;
    }

    {
        size_t prmIdx;
        
        for (prmIdx = 0;
             prmIdx < nPrms;
             ++prmIdx) 
        {
            prhs[RACCEL_RHS_P1 + prmIdx] =
                ssGetSFcnParam(*childPtr, prmIdx);
        }
    }

    mexFunctionHandle(1, plhs, nrhs, prhs);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* dynamic loading */
void raccelLoadSFcnMexFile(
    SimStruct* parent,
    SimStruct* child,
    size_t infoIdx)
{
    const char* mexFilePath =
        gblSFcnInfo[infoIdx].mexPath;

    MexFunctionPtr mexFunctionHandle;
#ifdef _WIN32
    HINSTANCE libraryHandle;
#else
    void *libraryHandle;
#endif

#ifdef _WIN32
    libraryHandle = LoadLibrary(mexFilePath);
#else
    libraryHandle = dlopen(mexFilePath, RTLD_NOW | RTLD_LOCAL);
#endif

    if (libraryHandle == NULL)
    {
        ssSetErrorStatus(
            parent,
            "error loading s-function mex file");
        
        return;
    }

    gblSFcnInfo[infoIdx].mexHandle =
        libraryHandle;

    /* the casts circumvent a warning: ISO C forbids conversion of object pointer to 
       function pointer type */
#ifdef _WIN32
    mexFunctionHandle =
        (MexFunctionPtr) GetProcAddress(libraryHandle, "mexFunction");
#else
    *(void **)(&mexFunctionHandle) =
        dlsym(libraryHandle, "mexFunction");
#endif

    if (mexFunctionHandle == NULL)
    {
        ssSetErrorStatus(
            parent,
            "error finding mexFunction symbol in s-function mex file");
        
        return;
    }

    setupSFcnSimstruct(child);

    callSFcnMexFunction(parent, &child, mexFunctionHandle);
}

/* ////////////////////////////////////////////////////////////////////////////////////////// */
/* mex api handling */
void
raccelSFcnMexHandler(const char* mexFcn)
{
    const char* name =
        gblSFcnInfo[gblCurrentSFcnIdx].name;

    const char* errTemplate =
        "<diag_root><diag id=\"Simulink:tools:rapidAccelSFcnMex\"><arguments><arg type = \"string\">%s</arg><arg type = \"string\">%s</arg></arguments></diag></diag_root>";                

    char* message =
        (char *) calloc(1024, sizeof(char));
    
    sprintf(message,
            errTemplate,
            name,
            mexFcn);

    raccelSFcnLongJumper((const char*) message);
}

bool
raccelMexIsLocked()
{
    raccelSFcnMexHandler("mexIsLocked");
    return false;
}

int
raccelMexPutVar (
    const char* c1,
    const char* c2,
    const mxArray* mx)
{
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(c2);
    UNUSED_PARAMETER(mx);
    raccelSFcnMexHandler("mexPutVar");
    return 0;
}

const mxArray *
raccelMexGetVarPtr(
    const char* c1,
    const char* c2)
{
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(c2);
    raccelSFcnMexHandler("mexGetVarPtr");
    return NULL;    
}

mxArray *
raccelMexGetVar(
    const char* c1,
    const char* c2)
{
    UNUSED_PARAMETER(c1);
    UNUSED_PARAMETER(c2);
    raccelSFcnMexHandler("mexGetVar");
    return NULL;    
}

void
raccelMexLock()
{
    raccelSFcnMexHandler("mexLock");
}

void
raccelMexUnlock()
{
    raccelSFcnMexHandler("mexUnlock");
}

const char *
raccelMexFunctionName()
{
    raccelSFcnMexHandler("mexFunctionName");
    return NULL;
}

int
raccelMexEvalString(
    const char * c1)
{
    UNUSED_PARAMETER(c1);    
    raccelSFcnMexHandler("mexEvalString");
    return 0;
}

mxArray *
raccelMexEvalStringWithTrap(
    const char * c1)
{
    UNUSED_PARAMETER(c1);    
    raccelSFcnMexHandler("mexEvalStringWithTrap");
    return NULL;
}

int
raccelMexSet(
    double d,
    const char * c,
    mxArray * m)
{
    UNUSED_PARAMETER(d);    
    UNUSED_PARAMETER(c);    
    UNUSED_PARAMETER(m);    
    raccelSFcnMexHandler("mexSet");
    return 0;
}

const mxArray *
raccelMexGet(
    double d,
    const char * c)
{
    UNUSED_PARAMETER(d);    
    UNUSED_PARAMETER(c);
    raccelSFcnMexHandler("mexGet");
    return NULL;
}

void
raccelMexSignalDebugger()
{
    raccelSFcnMexHandler("mexSignalDebugger");
}

int
raccelMexCallMatlab(
    int n1,
    mxArray* m1[],
    int n2,
    mxArray* m2[],
    const char* c,
    bool b)
{
    UNUSED_PARAMETER(n1);
    UNUSED_PARAMETER(m1);
    UNUSED_PARAMETER(n2);
    UNUSED_PARAMETER(m2);
    UNUSED_PARAMETER(c);
    UNUSED_PARAMETER(b);
    raccelSFcnMexHandler("mexCallMATLAB");
    return 0;
}

mxArray*
raccelMexCallMatlabWithTrap(
    int n1,
    mxArray* m1[],
    int n2,
    mxArray* m2[],
    const char* c)
{
    UNUSED_PARAMETER(n1);
    UNUSED_PARAMETER(m1);
    UNUSED_PARAMETER(n2);
    UNUSED_PARAMETER(m2);
    UNUSED_PARAMETER(c);
    raccelSFcnMexHandler("mexCallMATLABWithTrap");
    return NULL;
}

mxArray *
raccelMexCreateSimpleFunctionHandle(
    mxFunctionPtr f)
{
    UNUSED_PARAMETER(f);
    raccelSFcnMexHandler("mexCreateSimpleFunctionHandle");
    return NULL;    
}

int
raccelMexAtExit()
{
    raccelSFcnMexHandler("mexAtExit");
    return 0;
}

void
setMexCallbacks()
{
    
    RAccelMexCallbacks raccelMexCallbacks =
        {
            (RAccelMexIsLocked)&raccelMexIsLocked,
            (RAccelMexPutVar)&raccelMexPutVar,
            (RAccelMexGetVarPtr)&raccelMexGetVarPtr,
            (RAccelMexGetVar)&raccelMexGetVar,
            (RAccelMexLock)&raccelMexLock,
            (RAccelMexUnlock)&raccelMexUnlock,
            (RAccelMexFunctionName)&raccelMexFunctionName,
            (RAccelMexEvalString)&raccelMexEvalString,
            (RAccelMexEvalStringWithTrap)&raccelMexEvalStringWithTrap,
            (RAccelMexSet)&raccelMexSet,
            (RAccelMexGet)&raccelMexGet,
            (RAccelMexCallMatlab)&raccelMexCallMatlab,
            (RAccelMexCallMatlabWithTrap)&raccelMexCallMatlabWithTrap,
            (RAccelMexCreateSimpleFunctionHandle)&raccelMexCreateSimpleFunctionHandle,
            (RAccelMexAtExit)&raccelMexAtExit
        };

#ifndef __MINGW32__    
    rapidAccelSetMexCallbacks(raccelMexCallbacks);
#else
    rapidAccelSetMexCallbacks_wrapper(raccelMexCallbacks);
#endif
}


/* EOF raccel_sfcn_utils.c  */

/* LocalWords:  raccel_sfcn_utils.c */
