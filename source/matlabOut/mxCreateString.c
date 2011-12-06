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
#include <stdio.h>
#include <string.h>
#include "matOut.h"

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
