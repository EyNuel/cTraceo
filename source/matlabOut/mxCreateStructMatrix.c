/*
 *  Copyright 2011 Emanuel Ey <emanuel.ey@gmail.com>
 * 
 *  This file is part of matOut.
 *
 *  MatOut is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MatOut is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with matOut.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "matOut.h"

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
	// allocate memory for structure members (fields)
	for (uintptr_t iStruct=0; iStruct<nCols*nRows; iStruct++){
		outArray[iStruct].mxCLASS			= mxSTRUCT_CLASS;
		//outArray[iStruct].miTYPE			= mxSTRUCT_CLASS; TODO
		outArray[iStruct].dataElementSize	= sizeof(mxArray);
		outArray[iStruct].dims[0]			= nRows;
		outArray[iStruct].dims[1]			= nCols;
		outArray[iStruct].numericType		= mxREAL;
		outArray[iStruct].isStruct			= true;
		outArray[iStruct].isChild 			= false;
		outArray[iStruct].nFields			= nFields;
		outArray[iStruct].fieldNames		= malloc(nFields*sizeof(char*));
		
		//copy fieldnames into struct info
		for (uintptr_t iField=0; iField<nFields; iField++){
			//NOTE: strlen returns the length of a string, not including the terminating NULL character
			outArray[iStruct].fieldNames[iField] = mallocChar(strlen(fieldNames[iField])+1);
			memset(outArray[iStruct].fieldNames[iField], 0x00, strlen(fieldNames[iField])+1);
			strncpy(outArray[iStruct].fieldNames[iField], fieldNames[iField], strlen(fieldNames[iField]));
		}
		
		//printf("iStruct: %lu\n", iStruct);
		outArray[iStruct].pr_double	= NULL;
		outArray[iStruct].pi_double	= NULL;
		outArray[iStruct].pr_char	= NULL;
		outArray[iStruct].field		= NULL;
		outArray[iStruct].field		= malloc(nFields*sizeof(mxArray*));
		if (outArray[iStruct].field	== NULL){
			fatal("mxCreateStructMatrix(): memory allocation error.\n");
		}
		//initialize field members:
		for (uintptr_t iField=0; iField<nFields; iField++){
			outArray[iStruct].field[iField] = NULL;
		}
	}
	
	return outArray;
}
