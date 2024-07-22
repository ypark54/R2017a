/* Copyright 2013-2016 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef _sl_messages_sfcn_bridge_h_
#define _sl_messages_sfcn_bridge_h_

#include "simulink_spec.h"

typedef enum {
    SS_MESSAGE_QUEUE_FIFO,
    SS_MESSAGE_QUEUE_LIFO,
    SS_MESSAGE_QUEUE_PRIORITY
} ssMessageQueueType;

typedef enum {
    SS_MESSAGE_QUEUE_OVERFLOW_ERROR,
    SS_MESSAGE_QUEUE_OVERFLOW_WARNING,
    SS_MESSAGE_QUEUE_OVERFLOW_IGNORE
} ssMessageQueueOverflowDiagnostic;

typedef enum {
    SS_MESSAGE_QUEUE_ASCENDING,
    SS_MESSAGE_QUEUE_DESCENDING
} ssMessagePriorityOrder;

typedef enum {
    SS_INPUT_PORT_MESSAGE_QUEUE,
    SS_LOCAL_MESSAGE_QUEUE
} ssMessageQueueLocationType;

typedef void* ssMessage;

SIMULINK_EXPORT_EXTERN_C
void ssSetMsgQueueDWorkId(SimStruct *S, int dIndex, int dId);

SIMULINK_EXPORT_EXTERN_C void ssSetOutputPortIsMessage(SimStruct *S, int pIdx, int val);

SIMULINK_EXPORT_EXTERN_C void ssSetInputPortIsMessage(SimStruct *S, int pIdx, int val);
SIMULINK_EXPORT_EXTERN_C void ssSetInputPortHasImplicitMessageQueue(SimStruct *S, int pIdx, int val);

SIMULINK_EXPORT_EXTERN_C void ssSetNumLocalMessageQueues(SimStruct *S, int num);
SIMULINK_EXPORT_EXTERN_C void ssSetMessageQueueLength(SimStruct *S, ssMessageQueueLocationType queueLocType, int queueID, int queueLen);
SIMULINK_EXPORT_EXTERN_C void ssMessageQueueSetDataProperties(SimStruct *S, ssMessageQueueLocationType queueType, int queueID, int numDims, int_T* dims, DTypeId typeId, int complexity);
SIMULINK_EXPORT_EXTERN_C void ssSetMessageQueueType(SimStruct *S, ssMessageQueueLocationType queueLocType, int queueID, ssMessageQueueType queueType);
SIMULINK_EXPORT_EXTERN_C void ssSetMessageQueuePriorityOrder(SimStruct *S, ssMessageQueueLocationType queueLocType, int queueID, ssMessagePriorityOrder pOrder);
SIMULINK_EXPORT_EXTERN_C void ssSetMessageQueueOverflowDiagnostic(SimStruct *S, ssMessageQueueLocationType queueLocType, int queueID, ssMessageQueueOverflowDiagnostic qOverflow);
SIMULINK_EXPORT_EXTERN_C void ssSetInputPortMightQueryNumMsg(SimStruct *S, size_t portIdx);

SIMULINK_EXPORT_EXTERN_C void ssMessageSend(SimStruct *S, int outputPortIdx, const void *payload, unsigned int priority);
SIMULINK_EXPORT_EXTERN_C void ssMessageSendToLocalQueue(SimStruct *S, int pIdx, const void *payload, unsigned int priority);
SIMULINK_EXPORT_EXTERN_C int_T ssGetNumMessagesInOutputQueue(SimStruct *S, int pIdx);

SIMULINK_EXPORT_EXTERN_C void ssMessageRelay(SimStruct *S, int outputPortIdx, ssMessage msgObj);

SIMULINK_EXPORT_EXTERN_C int_T ssGetNumMessagesInQueue(SimStruct *S, ssMessageQueueLocationType qt, int queueID);

SIMULINK_EXPORT_EXTERN_C int_T ssGetMessagePayloadSizeInLocalQueue(SimStruct *S, int queueID);

SIMULINK_EXPORT_EXTERN_C void ssMessageQueuePush(SimStruct *S, ssMessageQueueLocationType qt, int queueID, ssMessage msg);

SIMULINK_EXPORT_EXTERN_C ssMessage ssMessageQueuePop(SimStruct *S, ssMessageQueueLocationType qt, int queueID);

SIMULINK_EXPORT_EXTERN_C ssMessage ssMessageQueuePopForDesCharts(SimStruct *S, ssMessageQueueLocationType qt, int queueID, bool isMsgTrigger);

SIMULINK_EXPORT_EXTERN_C int ssEntityStartTimer(SimStruct *S, int timerId, double delay);
SIMULINK_EXPORT_EXTERN_C void ssEntityCancelTimer(SimStruct *S, int timerId);
SIMULINK_EXPORT_EXTERN_C int ssEntityGetActiveTimer(SimStruct *S);

SIMULINK_EXPORT_EXTERN_C void ssEntityAddTrigger(SimStruct *S, ssMessageQueueLocationType qt, int queueID, int triggerID);
SIMULINK_EXPORT_EXTERN_C void ssEntityRemoveTrigger(SimStruct *S, ssMessageQueueLocationType qt, int queueID, int triggerID);

SIMULINK_EXPORT_EXTERN_C void *ssMessageQueuePeekPayload(SimStruct *S, ssMessageQueueLocationType qt, int queueID);

SIMULINK_EXPORT_EXTERN_C void *ssMessageQueuePeekPayloadAtIndex(SimStruct *S, ssMessageQueueLocationType qt, int queueID, int msgIndex);
SIMULINK_EXPORT_EXTERN_C void *ssMessageOutputQueuePeekPayloadAtIndex(SimStruct *S, int portIdx, int msgIndex);

SIMULINK_EXPORT_EXTERN_C void ssMessagePayloadDestroy(SimStruct *S, void *msgData);

SIMULINK_EXPORT_EXTERN_C void *ssGetMessagePayload(SimStruct *S, ssMessage msg);
SIMULINK_EXPORT_EXTERN_C void ssSetMessagePayload(SimStruct *S, ssMessage msg, const void *payload);
SIMULINK_EXPORT_EXTERN_C void ssMessageDestroy(SimStruct *S, ssMessage msgObj);

SIMULINK_EXPORT_EXTERN_C boolean_T ssIsMessageQueueEmpty(SimStruct *S, ssMessageQueueLocationType qt, int portId);
SIMULINK_EXPORT_EXTERN_C boolean_T ssIsMessageQueueFull(SimStruct *S, ssMessageQueueLocationType qt, int portIdx, bool isOutputPort, bool throwErrQueryNumMsgOnMultirateQueue);
SIMULINK_EXPORT_EXTERN_C size_t ssGetNumDroppedMessagesFromLocalQueue(SimStruct *S, int queueId);
SIMULINK_EXPORT_EXTERN_C size_t ssGetRecommendedCapacityForLocalQueue(SimStruct *S, int queueId);

SIMULINK_EXPORT_EXTERN_C ssMessageQueueOverflowDiagnostic ssGetMessageQueueOverflowDiagnostic(SimStruct *S, ssMessageQueueLocationType queueLocType, int queueID);

#endif
