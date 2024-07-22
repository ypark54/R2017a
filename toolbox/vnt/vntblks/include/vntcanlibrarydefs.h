/**
* @file:       vntcanlibrarydefs.h
*              
* Purpose:     C functions used to manage library handles.
*              
* $Authors:    Shankar Subramanian $
*              
* $Copyright: 2010-2013 The MathWorks, Inc.$
*
*/
#ifndef vntcanlibrarydefs_header
#define vntcanlibrarydefs_header

namespace vntcansl{
    // *******************************************************************
    // ENGINE-VENDOR COMMUNICATION.
    // *******************************************************************
    namespace vendor {
        /** Exported vendor function name, available to access Vendor API.
        *
        *  Opens connection to the hardware.
        */
        const char* const VENDOR_OPEN_FCN = "exSLVendorOpen";
        
        typedef void (*VENDOR_OPEN_FCN_PTR)(const char* const deviceName, 
                                       int& serialNumber,
                                       int deviceCode, 
                                       int& deviceIndex, 
                                       int deviceChannelIndex, 
                                       double& rAccessMask, 
                                       double& rPortHandle, 
                                       double& rPermissionMask, 
                                       double busSpeed, int sjw, int tseg1, int tseg2, int numSamples, 
                                       int silentMode,
                                       double blockType);

        /** Exported vendor function name, available to CAN blocks.
        *
        * Sets IDs filtering in the hardware.
        *
        */
        const char* const VENDOR_SETIDS_FCN = "exSLVendorSetIDsFilter";
        typedef void (*VENDOR_SETIDS_FCN_PTR)(double portHandle, double accessMask, int enableStdIDFilter, int enableExtIDFilter, 
                                              double* stdIDsStart, double* stdIDsEnd, int lengthStdIDsRange, int* idMaskParams);

        /** Exported vendor function name, available to CAN blocks.
        * Closes the connection to the hardware.
        */
        const char* const VENDOR_CLOSE_FCN = "exSLVendorClose";
        
        typedef void (*VENDOR_CLOSE_FCN_PTR)(double portHandle, double accessMask);
    }
}

#endif
