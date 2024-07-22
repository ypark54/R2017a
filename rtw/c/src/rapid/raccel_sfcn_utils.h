/*
 * Copyright 2007-2015 The MathWorks, Inc.
 *
 * File: raccel_sfcn_utils.h
 *
 *
 * Abstract:
 *      functions for dynamically loading s-function mex files
 *
 * Requires include files
 *	tmwtypes.h
 *	simstruc_type.h
 * Note including simstruc.h before rsim.h is sufficient because simstruc.h
 * includes both tmwtypes.h and simstruc_types.h.
 */

#ifndef __RACCEL_SFCN_UTILS_H__
#define __RACCEL_SFCN_UTILS_H__

void raccelUnloadSFcnLibrariesAndFreeAssociatedGbls();

void raccelSetSFcnParamPtrToGblAndCopyParamData(
    SimStruct* child, 
    size_t infoIdx, 
    const double** rtpPtr);

void raccelSetSFcnParamPtrToGbl(
    SimStruct* child, 
    size_t infoIdx);

void raccelLoadSFcnMexFile(
    SimStruct* parent, 
    SimStruct* child, 
    size_t infoIdx);

int rapidAccelMexCallback(
    int nlhs,
    mxArray* plhs[],
    int nrhs,
    mxArray* prhs[],
    const char* fcn_name,
    bool mex_trap_flag);

void raccelInitializeForMexSFcns();

int raccelBlockSIDToInfoIdx(
    const char* blockSIDToMatch,
    SimStruct* root,
    const char* sFcnName,
    const char* mdlName);

void setMexCallbacks();   
    
#endif /* __RACCEL_SFCN_UTILS_H__ */
