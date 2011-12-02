#pragma once
#include "matlabOut.h"

void* mxGetPr(mxArray* inArray);

void*	mxGetPr(mxArray* inArray){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	if (inArray->isStruct){
		fatal("mxGetPr(): not possible to get data pointer from a structure.\n");
	}
	
	switch(inArray->mxCLASS){
		case mxDOUBLE_CLASS:
			return(inArray->pr_double);
			break;
		case mxCHAR_CLASS:
			return(inArray->pr_char);
			break;
		default:
			fatal("mxGetPr(): data has unknown mxClass.\n");
	}
	
	return NULL;
}
