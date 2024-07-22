/**
* @file:       hostlib_vntcan.c
*              
* Purpose:     Runtime functions used for VNT CAN Rx/Tx code gen.
*              
* $Authors:    S. S. $
*              
* $Copyright: 2010-2011 The MathWorks, Inc.$
*
*
*/

#include "hostlib_vntcan.h"

const char *libName_canReceive  = "slhostlibcanreceive.dll" ;
const char *libName_canTransmit = "slhostlibcantransmit.dll";
const char *libName_canReplay  = "slhostlibcanreplay.dll" ;
const char *libName_canLog  = "slhostlibcanlog.dll" ;

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
                        int* idMaskParams){
    HostLibrary *hostLib = (HostLibrary*)hl;
    *hostLib->errorMessage  = '\0';
    hostLib->instance = (MAKE_FCN_PTR(pFnLibCreate_CANReceive,hostLib->libCreate))(hostLib->errorMessage, vendorName, vendorPath, deviceName,
                                                serialNumber, deviceCode, deviceIndex, deviceChannelIndex, converterPath, pluginPath, busSpeed,
                                                bitParams, silentMode, enableStdIDsFilter, stdIDs, stdIDLength, enableExtIDsFilter, extIDs, extIDLength, stdIDsStart, stdIDsEnd, lengthStdIDsRange, idMaskParams);

}

void LibOutputs_CANReceive(void* hl, void* receivedFrame, int msgsPerTimestep, int* isMsgReceived, int* isMsgAvailable){
    HostLibrary *hostLib = (HostLibrary*)hl;
    if(hostLib->instance)
        (MAKE_FCN_PTR(pFnLibOutputs_CANReceive,hostLib->libOutputs))(hostLib->instance, hostLib->errorMessage, receivedFrame, msgsPerTimestep, isMsgReceived, isMsgAvailable);
}


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
                        double busSpeed, 
                        int* bitParams, 
                        int silentMode, int isPeriodic, double period){
    HostLibrary *hostLib = (HostLibrary*)hl;
    *hostLib->errorMessage  = '\0';
    hostLib->instance = (MAKE_FCN_PTR(pFnLibCreate_CANTransmit,hostLib->libCreate))(hostLib->errorMessage, vendorName, vendorPath, deviceName,
                                                serialNumber, deviceCode, deviceIndex, deviceChannelIndex, converterPath, pluginPath, busSpeed, bitParams, 
                                                silentMode, isPeriodic, period);
}

void LibOutputs_CANTransmit(void* hl, void* msgsToSend, int nMessages){
    HostLibrary *hostLib = (HostLibrary*)hl;
    if(hostLib->instance)
        (MAKE_FCN_PTR(pFnLibOutputs_CANTransmit,hostLib->libOutputs))(hostLib->instance, hostLib->errorMessage, msgsToSend, nMessages);
}


void LibCreate_CANReplay(void* hl, 
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
                        int silentMode){
    HostLibrary *hostLib = (HostLibrary*)hl;
    *hostLib->errorMessage  = '\0';
    hostLib->instance = (MAKE_FCN_PTR(pFnLibCreate_CANReplay,hostLib->libCreate))(hostLib->errorMessage, fileName, fullPathFileName, variableName, numTimes, replayMode, 
                                                vendorName, vendorPath, deviceName,
                                                serialNumber, deviceCode, deviceIndex, deviceChannelIndex, converterPath, pluginPath, busSpeed,
                                                bitParams, silentMode);

}

void LibOutputs_CANReplay(void* hl, void* replayFrame, double simTime, int* isMsgReceived){
    HostLibrary *hostLib = (HostLibrary*)hl;
    if(hostLib->instance)
        (MAKE_FCN_PTR(pFnLibOutputs_CANReplay,hostLib->libOutputs))(hostLib->instance, hostLib->errorMessage, replayFrame, simTime, isMsgReceived);
}

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
                        int silentMode){
    HostLibrary *hostLib = (HostLibrary*)hl;
    *hostLib->errorMessage  = '\0';
    hostLib->instance = (MAKE_FCN_PTR(pFnLibCreate_CANLog,hostLib->libCreate))(hostLib->errorMessage, fileName, fullPathFileName, variableName, maxMsgsToLog, logMode, 
                                                vendorName, vendorPath, deviceName,
                                                serialNumber, deviceCode, deviceIndex, deviceChannelIndex, converterPath, pluginPath, busSpeed,
                                                bitParams, silentMode);

}

void LibOutputs_CANLog(void* hl, void* msgsToLog, int numMessages){
    HostLibrary *hostLib = (HostLibrary*)hl;
    if(hostLib->instance)
        (MAKE_FCN_PTR(pFnLibOutputs_CANLog,hostLib->libOutputs))(hostLib->instance, hostLib->errorMessage, msgsToLog, numMessages);
}
