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
	outArray->miTYPE			= miINT8;
	outArray->dataElementSize	= sizeof(char);
	outArray->pr_double			= NULL;
	outArray->pi_double			= NULL;
	outArray->pr_char			= NULL;
	outArray->dims[0]			= 1;
	outArray->dims[1]			= strlen(inString);
	outArray->numericType		= mxREAL;
	outArray->isStruct			= false;
	outArray->isChild			= false;
	outArray->nFields			= 0;
	outArray->fieldNames		= NULL;
	
	// allocate memory for string
	// NOTE: mallocChar already checks for succesfull allocation
	outArray->pr_char 	=	mallocChar(strlen(inString)+1);
	memset(outArray->pr_char, 0x00, strlen(inString)+1);
	
	//Copy string to data pointer
	//strncpy(char* dst, const char* src, size_t size);
	strncpy(outArray->pr_char, inString, strlen(inString));
	//printf("strlen(inString): %u", (uint32_t)strlen(inString));
	
	return outArray;
}
