#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "matlabOut.h"

mxArray* mxCreateStructMatrix(uintptr_t nRows, uintptr_t nCols, uintptr_t nFields, const char **fieldNames);

mxArray* mxCreateStructMatrix(uintptr_t nRows, uintptr_t nCols, uintptr_t nFields, const char **fieldNames){
	/*
	 * creates a 2D array of structures
	 */
	mxArray*	outArray = NULL;
	
	
	// do some input value validation
	if (nRows == 0 || nCols == 0){
		fatal("mxCreateStructMatrix(): array dimensions may not be null");
	}
	
	// allocate memory
	outArray	= malloc(nRows*nCols*sizeof(mxArray));
	if (outArray == NULL){
		fatal("mxCreateStructMatrix(): memory allocation error.");
	}
	
	// initialize variables
	//NOTE: only the first element of outArray will contain the basic struct information
	outArray[0].mxCLASS			= mxSTRUCT_CLASS;
	outArray[0].dataElementSize	= sizeof(mxArray);
	outArray[0].dims[0]			= nRows;
	outArray[0].dims[1]			= nCols;
	outArray[0].numericType		= mxREAL;
	outArray[0].isStruct		= true;
	outArray[0].nFields			= nFields;
	outArray[0].fieldNames		= malloc(nFields*sizeof(uintptr_t));
	
	//copy fieldnames into struct info
	for (int iField=0; iField<nFields; iField++){
		//NOTE: strlen returns the length of a string, not including the terminating NULL character
		outArray[0].fieldNames[iField] = mallocChar(strlen(fieldNames[iField])+1);	
		strncpy(outArray[0].fieldNames[iField], fieldNames[iField], strlen(fieldNames[iField])+1);
	}
	
	// allocate memory for structure members (fields)
	for (int iStruct=0; iStruct<nCols*nRows; iStruct++){
		outArray[iStruct].pr	= NULL;
		outArray[iStruct].pi	= NULL;
		
		outArray[iStruct].field	= NULL;
		outArray[iStruct].field	= (mxArray*)malloc(nFields*sizeof(mxArray*));
		if (outArray[iStruct].field== NULL){
			fatal("mxCreateStructMatrix(): memory allocation error.\n");
		}
	}
	
	return outArray;
}
