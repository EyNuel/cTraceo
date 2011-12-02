#pragma once
#include "matlabOut.h"

void* mxGetPi(mxArray* inArray);

void* mxGetPi(mxArray* inArray){
	/*
	 * Returns pointer to the first element of the mxArray's imaginary data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	if (inArray->numericType != mxCOMPLEX){
		fatal("mxGetPi(): mxArray does not contain imaginary data.");
	}
	
	if (inArray->isStruct){
		fatal("mxGetPi(): not possible to get data pointer from a structure.\n");
	}
	
	switch(inArray->mxCLASS){
		case mxDOUBLE_CLASS:
			return(inArray->pi_double);
			break;
		case mxCHAR_CLASS:
			fatal("mxGetPi(): mxChar type doe snot support imaginary data.\n");
			break;
		default:
			fatal("mxGetPi(): mxArray has unknown mxClass.\n");
	}
	
	return NULL;
}
