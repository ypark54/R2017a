/* Copyright 2008-2016 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
# pragma once
#endif

#ifndef __slMessagesSfcnCGBridge_hpp__
#define __slMessagesSfcnCGBridge_hpp__

#include "simulink_spec.h"
#include "simstruct/simstruc.h"

namespace RTWCG
{
    class CGIR_Block_IF;
}
namespace CG
{
    class CoreConstructionFacade;
    class Var;
    class Expr;
    class Region;
}

namespace slmsg
{
   
    SIMULINK_EXPORT_EXTERN_C
        CG::Region* cgCreateAndSendMessage(SimStruct *S,
                                           CG::CoreConstructionFacade *f,
                                           CG::Var *u,
                                           int queueType,
                                           int queueId,
                                           CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
        CG::Expr* cgCreateMessage(SimStruct* S,
                                  CG::CoreConstructionFacade* f,
                                  CG::Var* u,
                                  int queueType,
                                  int id,
                                  CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
        CG::Region* cgSendMessage(SimStruct* S,
                                  CG::CoreConstructionFacade* f,
                                  CG::Var* msgVar,
                                  int queueType,
                                  int prtId,
                                  CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
        CG::Expr* cgPopMessage(SimStruct* S,
                               CG::CoreConstructionFacade* f,
                               int queueType,
                               int portId,
                               CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
        CG::Expr* cgGetMessagePayload(CG::CoreConstructionFacade* f,
                                      CG::Var* msgVar);

    SIMULINK_EXPORT_EXTERN_C
        CG::Expr* cgGetNumMessages(SimStruct* S,
                                   CG::CoreConstructionFacade* f,
                                   int queueType,
                                   int portId,
                                   CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
        CG::Expr* cgDestroyMessage(SimStruct *S,
                                   CG::CoreConstructionFacade* f,
                                   CG::Var* msgVar);

    SIMULINK_EXPORT_EXTERN_C
        CG::Region* cgTargetQueueSend(CG::CoreConstructionFacade& ccf,
                                      SimStruct* s, CG::Var* msgDataVar,
                                      int queueType, int portId, CG::Var* localQVar);

    SIMULINK_EXPORT_EXTERN_C
        CG::Region* cgTargetQueueReceive(CG::CoreConstructionFacade& ccf,
                                         CG::Var* successVar,
                                         SimStruct* s, CG::Var* msgHandleVar, CG::Var* msgDataVar,
                                         int queueType, int portId, CG::Var* localQVar);

    SIMULINK_EXPORT_EXTERN_C
        CG::Region* cgTargetQueueDiscard(CG::CoreConstructionFacade& ccf,
                                         SimStruct* s, CG::Var* msgHandleVar,
                                         int queueType);

}

#endif
