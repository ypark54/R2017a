/**
* @file:       hostlib_vntcan.h
*              
* Purpose:     Runtime functions used for VNT CAN blocks code gen.
*              
* $Authors:    S. S. $
*              
* $Copyright: 2010-2011 The MathWorks, Inc.$
*
*
*/

#ifndef hostlib_vntcan_header
#define hostlib_vntcan_header

/* Wrap everything in extern C */
#ifdef __cplusplus
extern "C" {
#endif 

extern const char *libName_canReceive;
extern const char *libName_canTransmit;
extern const char *libName_canReplay;
extern const char *libName_canLog;

/*******************************
EXPORTED FUNCTION TYPES/NAMES
*******************************/
typedef void* (*pFnLibCreate_CANReceive)   (char* err, 
                                            const char* const vendorName, 
                                            const char* const vendorPath,
                                            const char* const deviceName,
                                            int serialNumber,
                                            int deviceCode, 
                                            int deviceIndex, 
                                            int deviceChannelIndex, 
                                            const char* converterPath, 
                                            const char* pluginPath, 
                                            double busSpeed, int* bitParams, 
                                            int silentMode,
                                            int enableStdIDsFilter, unsigned int* stdIDs, int stdIDLength, 
                                            int enableExtIDsFilter, unsigned int* extIDs, int extIDLength,
                                            double* stdIDsStart, double* stdIDsEnd, int lengthStdIDsRange,
                                            int* idMaskParams);

typedef void (*pFnLibOutputs_CANReceive)   (void *canReceiveObj, 
                                            char *err, void* receivedFrame, 
                                            int msgsPerTimestep, int* isMsgReceived, int* isMsgAvailable);

typedef void* (*pFnLibCreate_CANTransmit)   (char* err, 
                                            const char* const vendorName, 
                                            const char* const vendorPath,
                                            const char* const deviceName,
                                            int serialNumber,
                                            int deviceCode, 
                                            int deviceIndex, 
                                            int deviceChannelIndex, 
                                            const char* converterPath, 
                                            const char* pluginPath, 
                                            double busSpeed, int* bitParams, 
                                            int silentMode, int isPeriodic, double period);

typedef void (*pFnLibOutputs_CANTransmit)   (void *canTransmitObj, 
                                            char *err, 
                                            void* msgsToSend, int nMessages);

typedef void* (*pFnLibCreate_CANReplay)    (char* err, 
                                            const char* const fileName, 
                                            const char* const fullPathFileName, 
                                            const char* const variableName, 
                                            double numTimes, 
                                            int replayMode, 
                                            const char* const vendorName, 
                                            const char* const vendorPath,
                                            const char* const deviceName,
                                            int serialNumber,
                                            int deviceCode, 
                                            int deviceIndex, 
                                            int deviceChannelIndex, 
                                            const char* converterPath, 
                                            const char* pluginPath, 
                                            double busSpeed, int* bitParams, 
                                            int silentMode);

typedef void (*pFnLibOutputs_CANReplay)    (void* canReplayObj, char* err, 
                                            void* replayFrame, double simTime, int* isMsgReceived);

typedef void* (*pFnLibCreate_CANLog)        (char* err, 
                                            const char* const fileName, 
                                            const char* const fullPathFileName, 
                                            const char* const variableName, 
                                            double maxMsgsToLog, 
                                            int logMode, 
                                            const char* const vendorName, 
                                            const char* const vendorPath,
                                            const char* const deviceName,
                                            int serialNumber,
                                            int deviceCode, 
                                            int deviceIndex, 
                                            int deviceChannelIndex, 
                                            const char* converterPath, 
                                            const char* pluginPath, 
                                            double busSpeed, int* bitParams, 
                                            int silentMode);

typedef void (*pFnLibOutputs_CANLog)    (void* canLogObj, char* err, 
                                            void* msgsToLog, int numMessages);

/**************************
LIBRARY FUNCTION WRAPPERS
**************************/

void LibCreate_CANReceive(void* hl, 
                        const char* const vendorName, 
                        const char* const vendorPath,
                        const char* const deviceName,
                        int serialNumber,
                        int deviceCode, 
                        int deviceIndex, 
                        int deviceChannelIndex, 
                        const char* converterPath, 
                        const char* pluginPath, 
                        double busSpeed, int* bitParams, 
                        int silentMode,
                        int enableStdIDsFilter, unsigned int* stdIDs, int stdIDLength, 
                        int enableExtIDsFilter, unsigned int* extIDs, int extIDLength,
                        double* stdIDsStart, double* stdIDsEnd, int lengthStdIDsRange,
                        int* idMaskParams);

void LibOutputs_CANReceive(void* hl, void* receivedFrame, int msgsPerTimestep, int* isMsgReceived, int* isMsgAvailable);


void LibCreate_CANTransmit(void* hl, 
                        const char* const vendorName, 
                        const char* const vendorPath,
                        const char* const deviceName,
                        int serialNumber,
                        int deviceCode, 
                        int deviceIndex, 
                        int deviceChannelIndex, 
                        const char* converterPath, 
                        const char* pluginPath, 
                        double busSpeed, int* bitParams, 
                        int silentMode, int isPeriodic, double period);

void LibOutputs_CANTransmit(void* hl, void* msgsToSend, int nMessages);

void LibCreate_CANReplay (void* hl, 
                        const char* const fileName, 
                        const char* const fullPathFileName, 
                        const char* const variableName, 
                        double numTimes, 
                        int replayMode, 
                        const char* const vendorName, 
                        const char* const vendorPath,
                        const char* const deviceName,
                        int serialNumber,
                        int deviceCode, 
                        int deviceIndex, 
                        int deviceChannelIndex, 
                        const char* converterPath, 
                        const char* pluginPath, 
                        double busSpeed, int* bitParams, 
                        int silentMode);

void LibOutputs_CANReplay (void* hl, void* replayFrame, double simTime, int* isMsgReceived);

void LibCreate_CANLog(void* hl, 
                        const char* const fileName, 
                        const char* const fullPathFileName, 
                        const char* const variableName, 
                        double maxMsgsToLog, 
                        int logMode, 
                        const char* const vendorName, 
                        const char* const vendorPath,
                        const char* const deviceName,
                        int serialNumber,
                        int deviceCode, 
                        int deviceIndex, 
                        int deviceChannelIndex, 
                        const char* converterPath, 
                        const char* pluginPath, 
                        double busSpeed, int* bitParams, 
                        int silentMode);

void LibOutputs_CANLog (void* hl, void* msgsToLog, int numMessages);

/* Include HostLib for declarations of LibStart, LibTerminate, CreateHostLibrary, and DestroyHostLibrary. */
#include "HostLib_rtw.h"

#ifdef __cplusplus
} /* extern "C"*/
#endif 

#endif
