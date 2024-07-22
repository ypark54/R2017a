/* Copyright 2015 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SLSIMTGTCLIENTSERVERAPIBRIDGE_H
#define SLSIMTGTCLIENTSERVERAPIBRIDGE_H

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

#include "sl_simtarget_core_spec.h"
#include "slClientServerAPIBridge_types.h"

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsInitFcnCallInfo(_ssFcnCallInfo   *info,
                                                  int_T             numInArgs,
                                                  int_T             numOutArgs,
                                                  _ssFcnCallArgInfo *inArgs,
                                                  _ssFcnCallArgInfo *outArgs);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C _ssFcnCallExecArgs *slcsCreateFcnCallExecArgs(int_T numInArgs,
                                                                                  int_T numOutArgs);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsDestroyFcnCallExecArgs(_ssFcnCallExecArgs *execArgs);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetCanBeInvokedConcurrently(_ssFcnCallInfo *info, boolean_T val);
SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetCallerBlockPaths(_ssFcnCallInfo *info,
                                                                 int_T           nCallers,
                                                                 const char    **callerBlockPaths);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetCallerTIDs(_ssFcnCallInfo *info,
    int_T nCallerTIDs,
    int_T *callerTIDs);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetReusedInOutArgs(_ssFcnCallInfo *info,
                                                                int_T  nInOutArgs,
                                                                int_T *inOutArgs);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetSymbolicDims(_ssFcnCallInfo *info,
                                                             int_T *arginSymbDims,
                                                             int_T *argoutSymbDims);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetArgumentIndices(_ssFcnCallInfo *info,
                                                                const char **argNames,
                                                                const char **argIndices);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetReturnArgIndex(_ssFcnCallInfo *info,
                                                               int_T returnArgIndex);
SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetReturnArgName(_ssFcnCallInfo *info,
                                                              const char * returnArgName);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C bool slcsHasSimulinkFunctionsDefined(SimStruct* S);

#ifdef __cplusplus
SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsGetDefinedSimulinkFunctions(SimStruct* S, std::vector<std::string> * slFcnList);
#endif

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsRegisterSimulinkFunction(
    SimStruct* S, const char * fcnName, SimulinkFunctionPtr fcnPtr,
    _ssFcnCallInfo callInfo, const char * fullPathToFunction, bool isGlobal);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsRequestService(
    SimStruct* S, const char * fcnName, _ssFcnCallExecArgs args);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsInvokeSimulinkFunction(
    SimStruct* S, const char * fcnName, _ssFcnCallExecArgInfo *args);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C  void slcsRegisterCallerBlock(
    SimStruct* S, const char * fcnName, _ssFcnCallInfo callInfo, const char * addlFullPath);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsRegisterCallgraph(SimStruct *S,
                                                             const char *callerFcnName,
                                                             const char *relativePathToFunction,
                                                             const char *calledFcnName,
                                                             const char *calledFcnPath,
                                                             const char *callerInfo);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C _ssFcnCallInfo slcsGetFcnCallInfo(
    SimStruct* S, const char * fcnName);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsFreeFcnCallInfo(
    _ssFcnCallInfo callInfo);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsUpdateServerSFcnCatalog(
    SimStruct* S, const char * fcnName, void * fPtr);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C bool slcsIsFunctionRegistered(
    SimStruct* S, const char * fcnName);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C bool slcsIsFunctionRegisteredWithModel(
    double modelHandle, double blockHandle, const char * fcnName);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C const char * slcsGetCodeGenScopedFcnName(
    SimStruct* S, const char * fcnName);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsSetCodeGenScopedFcnName(
    SimStruct* S, const char * fcnName, const char * codeGenName);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsCatalogAddCallerAndStateOwnerInSameTask(
    SimStruct* S, const char *callerPath, const char *stateOwnerPath);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsCatalogAddCallerAndStateAccessorInSameTask(
    SimStruct* S, const char *callerPath, const char *stateAccessorPath);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsAddStateOwnerInsideFunction(
    SimStruct* S, const char *fcnName, const char *stateOwnerPath);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void slcsAddStateAccessorInsideFunction(
    SimStruct* S, const char *fcnName, const char *stateAccessorPath);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void* slcsGetInputArgFromExecInfo(_ssFcnCallExecArgs* args, int i);

SL_SIMTARGET_CORE_EXPORT_EXTERN_C void* slcsGetOutputArgFromExecInfo(_ssFcnCallExecArgs* args, int i);

#endif
