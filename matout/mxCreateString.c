#pragma once
#include <stdio.h>
#include <string.h>
#include "matlabOut.h"

mxArray* mxCreateString(const char *);

mxArray* mxCreateString(const char *inString){
	// allocate memory for struct
	mxArray*	outArray = malloc(sizeof(mxArray));
	if (outArray == NULL){
		fatal("mxCreateString(): memory allocation error.");
	}
	
	// initialize variables
	outArray->mxCLASS			= mxCHAR_CLASS;
	outArray->dataElementSize	= sizeof(char);
	outArray->pr 				= NULL;
	outArray->pi 				= NULL;
	outArray->dims[0]			= 1;
	outArray->dims[1]			= strlen(inString);
	outArray->numericType		= mxREAL;
	outArray->isStruct			= false;
	outArray->nFields			= 0;
	outArray->fieldNames		= NULL;
	
	// allocate memory for string
	// NOTE: mallocChar already checks for succesfull allocation
	outArray->pr 	=	(void*)mallocChar(strlen(inString));
	
	//Copy string to data pointer
	//strncpy(char* dst, const char* src, size_t size);
	strncpy(outArray->pr, inString, strlen(inString));
	printf("strlen(inString): %u", (uint32_t)strlen(inString));
	
	return outArray;
}
