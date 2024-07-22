/* Copyright 2013-2016 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SL_SFCN_COV_BRIDGE_H
#define SL_SFCN_COV_BRIDGE_H

#include "tmwtypes.h"

#ifdef BUILDING_LIBMWSL_SFCN_COV_BRIDGE
/* being included from simulink source code */
# define LINKAGE_SPEC extern "C" DLL_EXPORT_SYM
# include "package.h"
#else
# ifdef __cplusplus
# define LIBMWSL_SFCN_COV_BRIDGE_EXTERN_C extern "C"
# else
# define LIBMWSL_SFCN_COV_BRIDGE_EXTERN_C extern
# endif
# ifdef S_FUNCTION_NAME
/* Used with any s-function (user written) - normal sim, rtw (grt) */
# define LINKAGE_SPEC LIBMWSL_SFCN_COV_BRIDGE_EXTERN_C
# else
# ifdef MDL_REF_SIM_TGT
/* Used with any Model Ref SIM s-function */
# define LINKAGE_SPEC LIBMWSL_SFCN_COV_BRIDGE_EXTERN_C
# else
/* Defaulting now to allow Stateflow to compile */
# define LINKAGE_SPEC LIBMWSL_SFCN_COV_BRIDGE_EXTERN_C
# endif
# endif
#endif

#ifndef _SIMSTRUCT
# define _SIMSTRUCT
typedef struct SimStruct_tag SimStruct;
#endif

typedef uint32_T covid_T;

LINKAGE_SPEC boolean_T slcovStartRecording(SimStruct* S);
LINKAGE_SPEC void slcovStopRecording(SimStruct* S);
LINKAGE_SPEC size_t slcovGetNumberOfSFunctionInstances(SimStruct* S);
LINKAGE_SPEC boolean_T slcovIsSFunctionInstanceSupported(SimStruct* S);
LINKAGE_SPEC boolean_T slcovEnterSFunctionMethod(SimStruct* S);
LINKAGE_SPEC boolean_T slcovExitSFunctionMethod(SimStruct* S);
LINKAGE_SPEC void slcovUploadSFunctionCoverageData(covid_T covId);
LINKAGE_SPEC void slcovUploadSFunctionCoverageSynthesis(SimStruct* S,
                                                        const uint32_T numH,
                                                        uint32_T* hTable,
                                                        const uint32_T numT,
                                                        uint32_T* tTable);
LINKAGE_SPEC void slcovClearSFunctionCoverageData(SimStruct* S,
                                                  const uint32_T numH,
                                                  uint32_T* hTable,
                                                  const uint32_T numT,
                                                  uint32_T* tTable);

LINKAGE_SPEC double slcovGetCovBoundaryAbsTol(void);
LINKAGE_SPEC double slcovGetCovBoundaryRelTol(void);

LINKAGE_SPEC boolean_T slcovIsInitialized(void);
LINKAGE_SPEC void slcovUploadStateflowCoverageSynthesisByName(const char* mainMachineName, 
                                                              const char* machineName,
                                                              const uint32_T numH,
                                                              uint32_T* hTable,
                                                              const uint32_T numT,
                                                              uint32_T* tTable);
LINKAGE_SPEC void slcovUploadStateflowCoverageSynthesisBySimstruct(SimStruct* S,
                                                                   const uint32_T numH,
                                                                   uint32_T* hTable,
                                                                   const uint32_T numT,
                                                                   uint32_T* tTable);

#include "matrix.h"

typedef struct slcovMxFunctionRef_tag slcovMxFunctionRef;

LINKAGE_SPEC boolean_T slcovLoadInstrumentedMexFunction(SimStruct* S, slcovMxFunctionRef **p);
LINKAGE_SPEC void slcovRunInstrumentedMexFunction(slcovMxFunctionRef *p,
                                                  int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
LINKAGE_SPEC void slcovUnloadInstrumentedMexFunction(slcovMxFunctionRef **p);

#endif
