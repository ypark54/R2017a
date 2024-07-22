/* $Revision: 1.1.6.4 $ */
/**
* @file:       can_datatype.h
*              
* Purpose:     CAN datatype S-Function.
*              
* $Authors:    S.S.$
*              
* $Copyright: 2008-2011 The MathWorks, Inc.$
*
* $Date: 2011/05/13 18:05:24 $
*/
#ifndef __SCANDATATYPE__HEADER__
#define __SCANDATATYPE__HEADER__

/*---------------------------------------------------------------
*   Header File For Sfunction CAN utilities
*
*   This file should be included with any S-Function
*   blocks that require CAN specific data types. 
-----------------------------------------------------------------*/

#include "can_message.h"
#include "scansimstruc.h"
#include "scanexportdef.h"

#define SL_CAN_MESSAGE_DTYPE_NAME "CAN_MESSAGE"

/*--------------------------------------------------------------
 * Function mdlInitialize_CAN_datatype
 *
 * Description
 *
 *      Registers all the signal datatypes that are required to
 *      use the CAN signal blocks. Call this function in
 *      any MDLInitializeSizes callback of an S-function before
 *      you use the 
 *
 *      DTypeID ssGetDataTypeId(SimStruct *S, char *name)
 *
 *      methods to assign data types to your ports. The 
 *
 *--------------------------------------------------------------*/
void SCAN_EXPORT_FCN mdlInitialize_CAN_datatype( SimStruct *S );


/*---------------------------------------------------------------
 * Function mdlInitializeCANMessage
 *
 *
 * Description
 * Initializes the fields in the CANDATATYPE (CAN Message) to default value (0).
 *
 *
 *--------------------------------------------------------------*/

void SCAN_EXPORT_FCN mdlInitializeCANMessage( CAN_DATATYPE* canmsg, uint8_T Extended );

#endif
