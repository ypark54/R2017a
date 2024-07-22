/* Copyright 1990-2015 The MathWorks, Inc. */

#ifndef __SLCLIENTSERVERAPIBRIDGE_TYPES_H__
#define __SLCLIENTSERVERAPIBRIDGE_TYPES_H__

#include "simulink_spec.h"

typedef struct _ssFcnCallArgInfo_tag {
    DimsInfo_T *dimsInfo;
    int_T      dataType;
    int_T      argumentType;
    int_T      complexSignal;
} _ssFcnCallArgInfo;

typedef struct _ssFcnCallStatusArgInfo_tag {
    boolean_T  returnStatus;
    int_T      dataType;
} _ssFcnCallStatusArgInfo;

typedef struct _ssFcnCallExecArgInfo_tag {
    void       *dataPtr;
    int_T      dataSize;
} _ssFcnCallExecArgInfo;

typedef struct _ssFcnCallExecData_tag {
    double callerBlk;
    const char *fcnName;
} _ssFcnCallExecData;

typedef struct _ssFcnCallExecArgs_tag {
    int_T                  numInArgs;
    int_T                  numOutArgs;
    _ssFcnCallExecArgInfo  *inArgs;
    _ssFcnCallExecArgInfo  *outArgs;
    _ssFcnCallExecArgInfo  *statusArg;
    _ssFcnCallExecData     *execData;
} _ssFcnCallExecArgs;

typedef struct _ssFcnCallInfo_tag {
    int_T                    numInArgs;
    int_T                    numOutArgs;
    int_T                    numInOutArgs;
    int_T                    numCallers;
    int_T                    numCallerTIDs;
    _ssFcnCallArgInfo        *inArgs;
    _ssFcnCallArgInfo        *outArgs;
    int_T                    *inOutArgs;
    int_T                    *inArgsSymbDims;
    int_T                    *outArgsSymbDims;
    int_T                    *callerTIDs;
    int_T                    returnArgIndex;
    const char               *returnArgName;
    const char               **callerBlockPaths;
    const char               **argNames;
    const char               **argIndices;
    struct {
        unsigned int canBeInvokedConcurrently : 1;
    } flags;
} _ssFcnCallInfo;

typedef void (*SimulinkFunctionPtr)(SimStruct *S, int tid, _ssFcnCallExecArgs *args);

#endif
