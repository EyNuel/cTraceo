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

mxArray* mxCreateDoubleMatrix( uintptr_t nRows, uintptr_t nCols, uintptr_t numericType);

mxArray* mxCreateDoubleMatrix(uintptr_t nRows, uintptr_t nCols, uintptr_t numericType){
	/*
	 * creates a 2D array of double precision floating point values
	 * can be real or complex.
	 */
	//printf("nRows: %lu, nCols: %lu\n", nRows, nCols);
	mxArray*	outArray = NULL;
	
	
	// do some input value validation
	if (nRows == 0 || nCols == 0){
		fatal("mxCreateDoubleMatrix(): array dimensions may not be null");
	}
	if (numericType != mxREAL && numericType != mxCOMPLEX){
		fatal("mxCreateDoubleMatrix(): only 'real' and 'complex' arrays are supported. 'Logical' and 'global' are not implemented.");
	}
	
	// allocate memory
	outArray	= malloc(sizeof(mxArray));
	if (outArray == NULL){
		fatal("mxCreateDoubleMatrix(): memory allocation error.");
	}
	
	// initialize variables
	outArray->mxCLASS			= mxDOUBLE_CLASS;
	outArray->miTYPE			= miDOUBLE;
	outArray->dataElementSize	= sizeof(double);
	outArray->pr_double			= NULL;
	outArray->pi_double			= NULL;
	outArray->pr_char			= NULL;
	outArray->dims[0]			= nRows;
	outArray->dims[1]			= nCols;
	outArray->numericType		= numericType;
	outArray->isStruct			= false;
	outArray->isChild 			= false;
	outArray->nFields			= 0;
	outArray->fieldNames		= NULL;
	
	// allocate memory for structure members
	// NOTE: mallocDouble already checks for succesfull allocation
	outArray->pr_double	=	mallocDouble(nRows*nCols);
	if (numericType	==	mxCOMPLEX){
		outArray->pi_double=	mallocDouble(nRows*nCols);
	}
	
	//printf("mxCreateDoubleMatrix(): address of new mxArray: %lu\n", outArray);
	
	return outArray;
}
