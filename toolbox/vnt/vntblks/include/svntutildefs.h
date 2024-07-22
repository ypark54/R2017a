/**
* @file:svntutildefs.h
* Common header defining strings for VNT S-functions.
* Copyright: 2013-2016 The MathWorks, Inc.
*/

#ifndef __SVNTUTILDEFS__HEADER__
#define __SVNTUTILDEFS__HEADER__

#define RECEIVE_QUEUE_SIZE      8192
#define MAX_ERROR_MSG_LEN       1024
#define ID_UINT32_MAX           4294967295UL
#define LENGTH_MAT_FILE_EXTN    4

enum {
    BLOCK_TYPE_RECEIVE = 0,
    BLOCK_TYPE_TRANSMIT,
    BLOCK_TYPE_LOG,
    BLOCK_TYPE_REPLAY
};

#define VECTOR_RECEIVE_QUEUE_SIZE  "ReceiveQueueSize"
#define VECTOR_PORT_HANDLE         "PortHandle"
#define VECTOR_CHANNEL_MASK        "ChannelMask"
#define RECEIVE_CAPABLE            "ReceiveCapable"
#define TRANSMIT_CAPABLE           "TransmitCapable"
#define REPLAY_CAPABLE             "ReplayCapable"
#define VECTOR_BUS_TYPE            "BusType"
#define REPLAY_COUNT_OPT	       "ReplayCount"
#define SET_REPLAY_COUNT_CMD	   "SetReplayCount"
#define BREAK_TRANSMIT_CMD   	   "BreakTransmit"

#define KVASER_PORT_HANDLE      "Handle"
#define NI_PORT_HANDLE          "Handle"
#define NI_XNET_INPUT_SESSION   "SessionReferenceInput"
#define NI_XNET_OUTPUT_SESSION  "SessionReferenceOutput"
#define PEAK_SYSTEM_HANDLE      "Handle"
#define MATHWORKS_HANDLE        "Handle"
#define FILTER_PASS_STANDARD    "FilterPassListStandard"
#define FILTER_PASS_EXTENDED    "FilterPassListExtended"

#define VNT_ML_LIBRARY_PATH     "MLLibraryPath"
#define VNT_SL_LIBRARY_PATH     "SLLibraryPath"
#define VNT_PLUGIN_PATH         "PluginPath"
#define VNT_WRAPPER_PATH        "WrapperPath"
#define VNT_CONVERTER_PATH      "ConverterPath"
#define IS_RAPID_ACCEL          "IsRapidAccel"

// Parameter structure
#define VNT_VENDOR              "Vendor"
#define DEVICE_NAME             "DeviceName"
#define DEVICE_INDEX            "DeviceIndex"
#define DEVICE_CHANNEL_INDEX    "DeviceChannelIndex"
#define SERIAL_NUMBER           "SerialNumber"
#define DEVICE_CODE             "DeviceCode"

// Bus parameters
#define BUS_SPEED               "BusSpeed"
#define SJW                     "SJW"
#define TSEG1                   "TSEG1"
#define TSEG2                   "TSEG2"
#define NUM_SAMPLES             "NSamples"
#define SILENT_MODE             "SilentMode"

// CAN Message Fields.
#define EXTENDED_CAN_FIELD      "Extended"
#define ID_CAN_FIELD            "ID"
#define LENGTH_CAN_FIELD        "Length"
#define DATA_CAN_FIELD          "Data"
#define REMOTE_CAN_FIELD        "Remote"
#define ERROR_CAN_FIELD         "Error"
#define TIMESTAMP_CAN_FIELD     "Timestamp"
#define MESSAGE_OBJECT_CLASS    "can.Message"

// ID Params.
#define ENABLE_STD_ID_FILTER    "EnabledStdIDFiltering"
#define ENABLE_EXT_ID_FILTER    "EnabledExtIDFiltering"
#define STANDARD_IDS            "StdIDs"
#define EXTENDED_IDS            "ExtIDs"
#define STANDARD_IDS_SIZE       "StdIDsSize"
#define EXTENDED_IDS_SIZE       "ExtIDsSize"
#define START_STD_IDS           "StartIDs"
#define END_STD_IDS             "EndIDs"
#define STD_RANGE_LENGTH        "NumRangeStdIDs"

// Code and Mask.
#define STD_IDS_CODE            "StdIDsCode"
#define STD_IDS_MASK            "StdIDsMask"
#define EXT_IDS_CODE            "ExtIDsCode"
#define EXT_IDS_MASK            "ExtIDsMask"

#define MAT_FILE_EXIST_CODE     2 // MATLAB 'exist' command returns 2 when a file exists.

#if defined(_WIN32)
#define FILE_SEP "\\"
#define SL_EXT     ".dll"
#else
// We do not support other platforms yet. Once vendors support it, update this.
// #define FILE_SEP "/"
#endif 

namespace svnt{
#define MAX_CAN_MESSAGE_LENGTH 8
#define NUMBER_OF_FIELDS 7

    namespace errors{

        /******************************************
        ERRORS FROM S-FUNCTIONS
        ******************************************/
        // Object creation error.
        const char* const QUERY_FAILED = "Unable to query hardware information for the selected CAN channel object.";

        /******************************************
        EXCEPTION FROM SHARED LIBRARIES
        ******************************************/
        // Device not found.
        const char* const DEVICE_NOT_FOUND_ID = "vnt:vntblks:deviceNotFound";
        const char* const DEVICE_NOT_FOUND = "Could not find the requested CAN hardware. Device may not be connected or may be in use.";

        // Device Open failed.
        const char* const DRIVER_OPEN_FAILED_ID = "vnt:vntblks:driverOpenFailed";
        const char* const DRIVER_OPEN_FAILED = "Could not open CAN device to connect to the network.";

        // Vendor Loader Error.
        const char* const UNABLE_TO_LOAD_VENDOR_ID = "vnt:vntblks:unableToLoadVendor";
        const char* const UNABLE_TO_LOAD_VENDOR = "Unable to load Vendor driver.";
        const char* const UNABLE_TO_FIND_FCNENTRY_ID = "vnt:vntblks:unableToFindFcnEntry";
        const char* const UNABLE_TO_FIND_FCNENTRY = "Could not determine function entry points in the host library.";
        const char* const OPEN_METHOD_NOT_FOUND_ID = "vnt:vntblks:unableToFindOpenMethod";
        const char* const OPEN_METHOD_NOT_FOUND = "Unable to find exported Open Method in Vendor Wrapper.";
        const char* const CLOSE_METHOD_NOT_FOUND_ID = "vnt:vntblks:unableToFindCloseMethod";
        const char* const CLOSE_METHOD_NOT_FOUND = "Unable to find exported Close Method in Vendor Wrapper.";
        const char* const SETPARAMS_METHOD_NOT_FOUND_ID = "vnt:vntblks:unableToFindSetParamsMethod";
        const char* const SETPARAMS_METHOD_NOT_FOUND = "Unable to find exported Set parameters Method in Vendor Wrapper.";

        // Replay and Log Error messages.
        const char* const STOP_SIMULATION = "Simulation stopped at user's request.";
        const char* const UNABLE_TO_LOAD_REPLAY_FILE_ID = "vnt:vntblks:unableToLoadReplayFile";
        const char* const UNABLE_TO_LOAD_REPLAY_FILE = "Unable to load specified file to replay CAN messages. Please ensure the file exists in the specified path.";
        const char* const UNABLE_TO_OPEN_LOG_FILE_ID = "vnt:vntblks:unableToOpenLogFile";
        const char* const UNABLE_TO_OPEN_LOG_FILE = "Unable to open specified file to log CAN messages. Please ensure the folder exists and is writable.";
        const char* const UNABLE_TO_CREATE_VARIABLE_ID = "vnt:vntblks:unableToCreateVariableInFile";
        const char* const UNABLE_TO_CREATE_VARIABLE = "Unable to create specified variable in the log file.";
        const char* const UNABLE_TO_LOAD_VARIABLE_ID = "vnt:vntblks:unableToLoadVariable";
        const char* const UNABLE_TO_LOAD_VARIABLE = "Unable to load specified variable from file.";
        const char* const MESSAGE_OBJ_TO_STRUCT_ID = "vnt:vntblks:msgObjectToStruct";
        const char* const MESSAGE_OBJ_TO_STRUCT = "The messages have been specified as can.Message object. Please convert the message objects to a message structure compatible with CAN Replay block.";
        const char* const INVALID_MESSAGE_FORMAT_ID = "vnt:vntblks:invalidMessageFormat";
        const char* const INVALID_MESSAGE_FORMAT = "CAN Messages must be specified as a MATLAB structure with the following required fields: ID, Length, Extended, Data and Timestamp. Please refer to CAN Replay block documentation to know about the required message format.";
        const char* const INVALID_DATA_FORMAT_ID = "vnt:vntblks:invalidDataFormat";
        const char* const INVALID_DATA_FORMAT = "Data field array in message must be specified as row major matrix.";
        const char* const INVALID_MESSAGE_DIMENSIONS_ID = "vnt:vntblks:invalidMessageDimensions";
        const char* const INVALID_MESSAGE_DIMENSIONS = "The dimensions of individual fields in message do not match. Ensure all required fields in the message are entered.";
        const char* const INSUFFICIENT_MEMORY_TO_LOG = "Unable to allocate memory to log data to run model successfully. Reduce maximum number of messages to log parameter to run model successfully";
        const char* const INSUFFICIENT_MEMORY_TO_REPLAY = "Unable to allocate memory to load and replay data.";

        // Fields not found in the message.
        const char* const FIELDS_NOT_FOUND_ID = "vnt:vntblks:FieldsNotFound";
        const char* const FIELDS_NOT_FOUND = "CAN Message does not contain a required field: ID, Extended, Timestamp, Data and Length.";
        const char* const ID_NOT_FOUND_ID = "vnt:vntblks:IDFieldNotFound";
        const char* const ID_NOT_FOUND = "CAN message does not contain ID array.";
        const char* const EXTENDED_NOT_FOUND_ID = "vnt:vntblks:extendedFieldNotFound";
        const char* const EXTENDED_NOT_FOUND = "CAN message does not contain Extended array.";
        const char* const LENGTH_NOT_FOUND_ID = "vnt:vntblks:lengthFieldNotFound";
        const char* const LENGTH_NOT_FOUND = "CAN message does not contain Length array.";
        const char* const DATA_NOT_FOUND_ID = "vnt:vntblks:dataFieldNotFound";
        const char* const DATA_NOT_FOUND = "CAN message does not contain Data array.";
        const char* const TIMESTAMP_NOT_FOUND_ID = "vnt:vntblks:timestampFieldNotFound";
        const char* const TIMESTAMP_NOT_FOUND = "CAN message does not contain Timestamp array.";

        // Data Type mismatch in the message.
        const char* const INVALID_EXTENDED_TYPE_ID = "vnt:vntblks:extendedTypeMismatch";
        const char* const INVALID_EXTENDED_TYPE = "Extended field in CAN message must be specified as a vector array of UINT8.";
        const char* const INVALID_ID_TYPE_ID = "vnt:vntblks:IDTypeMismatch";
        const char* const INVALID_ID_TYPE = "ID field in CAN message must be specified as a vector array of UINT32.";
        const char* const INVALID_LENGTH_TYPE_ID = "vnt:vntblks:lengthTypeMismatch";
        const char* const INVALID_LENGTH_TYPE = "Length field in CAN message must be specified as a vector array of UINT8.";
        const char* const INVALID_DATA_TYPE_ID = "vnt:vntblks:extendedTypeMismatch";
        const char* const INVALID_DATA_TYPE = "Data field in CAN message must be specified as a matrix array of UINT8.";
        const char* const INVALID_TIMESTAMP_TYPE_ID = "vnt:vntblks:timestampTypeMismatch";
        const char* const INVALID_TIMESTAMP_TYPE = "Timestamp field in CAN message structure must be specified as a vector array of doubles.";
        const char* const INVALID_ERROR_TYPE_ID = "vnt:vntblks:errorTypeMismatch";
        const char* const INVALID_ERROR_TYPE = "Error field in CAN message must be specified as a vector array of UINT8.";
        const char* const INVALID_REMOTE_TYPE_ID = "vnt:vntblks:remoteTypeMismatch";
        const char* const INVALID_REMOTE_TYPE = "Remote field in CAN message must be specified as a vector array of UINT8.";

        // Device Open failed.
        const char* const OPEN_FAILED_ID = "vnt:vntblks:openFailed";
        const char* const OPEN_FAILED = "Could not open CAN device to connect to the network.";

        // Set failed.
        const char* const SET_PARAMS_FAILED_ID = "vnt:vntblks:setFailed";
        const char* const SET_PARAMS_FAILED = "Could not set the required parameters on the CAN device.";

        // Device Query.
        const char* const DEVICE_QUERY_ERROR_ID = "vnt:vntblks:queryFailed";
        const char* const DEVICE_QUERY_ERROR = "Could not query the device for driver information.";

        // Unhandled error.
        const char* const UNHANDLED_ERROR_ID = "vnt:vntblks:unhandledError";
        const char* const UNHANDLED_ERROR = "An unhandled error occurred with CAN device.";

        // Receive error.
        const char* const RECEIVE_FAILED_ID = "vnt:vntblks:receiveError";
        const char* const RECEIVE_FAILED = "Unable to receive CAN messages from the specified channel.";

        // Transmit error.
        const char* const TRANSMIT_FAILED_ID = "vnt:vntblks:transmitError";
        const char* const TRANSMIT_FAILED = "Unable to transmit CAN messages to the specified channel.";
    }

    // Vendor names.
    const char* const CAN_STRING = "CAN";
    const char* const VENDOR_VECTOR = "Vector";
    const char* const VENDOR_KVASER = "Kvaser";
    const char* const VENDOR_NI = "ni";
    const char* const VENDOR_NI_XNET = "ni-xnet";
    const char* const VENDOR_PEAK_SYSTEM = "PEAK-System";
    const char* const VENDOR_MATHWORKS = "MathWorks";
    const char* const VNT_ON = "on";
    const char* const CAN_BUS_STRING = "CAN Bus";

    namespace libnames{
        // SL Wrapper libraries.
        const char* const VECTOR_WRAPPER = "slvectorxlwrapper";
        const char* const KVASER_WRAPPER = "slkvaserwrapper";
        const char* const NI_WRAPPER = "slnicanwrapper";
        const char* const NI_XNET_WRAPPER = "slnixnetwrapper";
        const char* const PEAK_SYSTEM_PCANBASIC_WRAPPER = "slpeaksystempcanbasicwrapper";
        const char* const MATHWORKS_WRAPPER = "slmathworkswrapper";

        // VNT CAN device driver
        const char* const VNT_CAN_DRIVER = "vntdevicedriver";

        // Vendor Plugins.
        const char* const VECTOR_PLUGIN = "vectorxlplugin";
        const char* const KVASER_PLUGIN = "kvasercanlibplugin";
        const char* const NI_PLUGIN = "nicanplugin";
        const char* const NI_XNET_PLUGIN = "nixnetplugin";
        const char* const PEAK_SYSTEM_PCANBASIC_PLUGIN = "peakpcanbasicplugin";
        const char* const MATHWORKS_PLUGIN = "mathworksplugin";

        // SL and ML Converters.
        const char* const SL_CONVERTER = "canslconverter";
        const char* const ML_CONVERTER = "canmlconverter";
    }

    // General string definitions.
    const char* const START_FUNCTION = "vntcansfcngetparams";
    const char* const PRIVATE_CHECK_FILE_FUNCTION = "privatevntslvalidatefile";
    const char* const PRIVATE_GET_FULL_FILE_FUNCTION = "privatevntslgetfullfile";
    const char* const VNT_GATE_FUNCTION = "vntslgate";
    const char* const EXIST_FUNCTION = "exist";
    const char* const DIR_TYPE = "file";
    const char* const REPLAY_BLOCK = "replay";
    const char* const LOG_BLOCK = "log";
    const char* const FALSE_STR = "false";
    const char* const NEW_LINE_CHAR = "\n";

    enum {
        ONE_MESSAGE = 1,
        ALL_MESSAGES
    };

    enum {
        CAN_BUS = 1,
        SIMULINK
    };

    enum {
        ID_FILTERING_ALLOW_ALL = 0,
        ID_FILTERING_ALLOW_ONLY,
        ID_FILTERING_BLOCK_ALL
    };

};

#endif