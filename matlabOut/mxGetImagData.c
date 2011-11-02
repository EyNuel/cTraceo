#pragma once
#include "matlabOut.h"


void* mxGetImagData(mxArray* inArray);

void* mxGetImagData(mxArray* inArray){
	/*
	 * Returns pointer to the first element of the mxArray's imaginary data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	
	if (inArray->numericType != mxCOMPLEX){
		fatal("mxGetImagData(): mxArray does not contain imaginary data.");
	}
	
	if (inArray->isStruct){
		fatal("mxGetImagData(): not possible to get data pointer from a structure.\n");
	}
	
	switch(inArray->mxCLASS){
		case mxDOUBLE_CLASS:
			return(inArray->pi_double);
			break;
		case mxCHAR_CLASS:
			fatal("mxGetImagData(): mxChar type doe snot support imaginary data.\n");
			break;
		default:
			fatal("mxGetImagData(): mxArray has unknown mxClass.\n");
	}
	
	return NULL;
}
