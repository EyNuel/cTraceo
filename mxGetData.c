#pragma once
#include "matlabOut.h"

void* 	mxGetData(mxArray* inArray);

void*	mxGetData(mxArray* inArray){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 * NOTE: This is actually identical to mxGetPr()
	 */
	printf("mxGetData()\n");
	if (inArray->isStruct){
		fatal("mxGetData(): not possible to get data pointer from a structure.\n");
	}
	
	switch(inArray->mxCLASS){
		case mxDOUBLE_CLASS:
			printf("mxGetData(): mxDOUBLE\n");
			//printf("mxGetData(): pr_double[0]: %lf <----\n", inArray->pr_double[0]);
			return (inArray->pr_double);
			break;
		case mxCHAR_CLASS:
			printf("mxGetData(): mxCHAR\n");
			return (inArray->pr_char);
			break;
		default:
			fatal("mxGetData(): data has unknown mxClass.\n");
	}
	return NULL;
}

