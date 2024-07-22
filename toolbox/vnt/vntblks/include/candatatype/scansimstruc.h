/* $Revision: 1.1.6.3 $ */
/**
* @file:       scansimstruc.h
*              
* Purpose:     General header for including the Simulink simstruc header.
*              
* $Authors:    S.S.$
*              
* $Copyright: 2008-2011 The MathWorks, Inc.$
*
* $Date: 2011/05/13 18:05:29 $
*/
#ifndef __SCANSIMSTRUC__HEADER__
#define __SCANSIMSTRUC__HEADER__

#ifndef S_FUNCTION_NAME
// Including simstruc header requires you to indicate what kind 
// of module you are. If we aren't being included as part of an
// S function, we need to define ourselves as a reguls MEX file.
#ifndef MATLAB_MEX_FILE
#define MATLAB_MEX_FILE
#endif

#endif

#include "simstruc.h"

#endif
