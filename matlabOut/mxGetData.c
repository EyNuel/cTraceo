#pragma once
#include "matlabOut.h"

void* 	mxGetData(mxArray* inArray);

void*	mxGetData(mxArray* inArray){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 * NOTE: This is actually identical to mxGetPr()
	 */
	if (inArray->isStruct){
		fatal("mxGetData(): not possible to get data pointer from a structure.\n");
	}
	
	switch(inArray->mxCLASS){
		case mxDOUBLE_CLASS:
			return(inArray->pr_double);
			break;
		case mxCHAR_CLASS:
			return(inArray->pr_char);
			break;
		default:
			fatal("mxGetData(): data has unknown mxClass.\n");
	}
	return NULL;
}

