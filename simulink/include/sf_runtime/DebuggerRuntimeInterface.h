/* Copyright 1997-2016 The MathWorks, Inc. */

#ifndef _debugger_codegen_interface_h
#define _debugger_codegen_interface_h

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#if defined(S_FUNCTION_NAME)
#include "mwmathutil.h"
#endif

#include "sf_runtime/sf_runtime_spec.h"

typedef struct SimStruct_tag SimStruct;

LIBMWSF_RUNTIME_API void* sfListenerInitializeUsingSimStruct(SimStruct* S);
LIBMWSF_RUNTIME_API void* sfListenerInitializeUsingBlockPath(char* blkPath);

LIBMWSF_RUNTIME_API void sfListenerTerminate(SimStruct* S);

LIBMWSF_RUNTIME_API void sfListenerReportStartingSection(SimStruct* S, unsigned int aSSIDNumber, int aSectionId);
LIBMWSF_RUNTIME_API void sfListenerReportEndingSection(SimStruct* S, unsigned int aSSIDNumber, int aSectionId);

LIBMWSF_RUNTIME_API void sfListenerPushScope(SimStruct* S, unsigned int aSSIDNumber, unsigned int N, char* aVarNames[], void* aDataPtr[], char* aMarchallingOutFunctions[], char* aMarchallingInFunctions[]);
LIBMWSF_RUNTIME_API void sfListenerPopScope(SimStruct* S, unsigned int aSSIDNumber);

LIBMWSF_RUNTIME_API void sfListenerReportStartingEventBroadcast(SimStruct* S, unsigned int aSSIDNumber, int aSectionId);
LIBMWSF_RUNTIME_API void sfListenerReportEndingEventBroadcast(SimStruct* S, unsigned int aSSIDNumber, int aSectionId);
#endif
