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
#include "matOut.h"

uintptr_t	mxCalcSingleSubscript(mxArray *inArray, uintptr_t nSubs, uintptr_t *subs);

uintptr_t	mxCalcSingleSubscript(mxArray *inArray, uintptr_t nSubs, uintptr_t *subs){
	/*
	 * Calculates linear index from subbdimenions
	 * Arguments:
	 * 		pm		Pointer to an mxArray;
	 *		nsubs	Number of elements in the subs array. Typically, you set nsubs
	 * 				equal to the number of dimensions in the mxArray that pm points to. 
	 * 		subs    An array of integers. Each value in the array specifies that
	 * 				dimension's subscript. In C syntax, the value in subs[0]
	 * 				specifies the row subscript, and the value in subs[1] specifies
	 * 				the column subscript. Use zero-based indexing for subscripts.
	 * 				For example, to express the starting element of a two-dimensional
	 * 				mxArray in subs, set subs[0] to 0 and subs[1] to 0.
	 * 	
	 * 	Return Value:
	 *		The number of elements, or index, between the start of the mxArray and the
	 *		specified subscript. This is the linear index equivalent of the subscripts.
	 * 		Many MX Matrix Library routines (for example, mxGetField) require an index
	 * 		as an argument.
	 *		If subs describes the starting element of an mxArray, mxCalcSingleSubscript
	 * 		returns 0. If subs describes the final element of an mxArray,
	 * 		mxCalcSingleSubscript returns N-1 (where N is the total number of elements).
	 * 	
	 *	Source:
	 * 		http://www.mathworks.com/help/techdoc/apiref/mxcalcsinglesubscript.html
	 *	
	 *	NOTE:
	 * 		This implementation only supports up to 2 dimensions, as there is no need
	 * 		for more in cTraceo.
	 */
	
	uintptr_t linearIndex = 0;
	uintptr_t col, row, nRows;
	
	row		    = subs[0];
	col		    = subs[1];
	nRows	    = inArray->dims[0];
	linearIndex	= row + col * nRows;
	
	//printf("mxCalcSingleSubscript(): (row, col, nRows) = (%lu, %lu, %lu) => (%lu)\n", subs[0], subs[1], nRows, linearIndex);
	
	return linearIndex;
}
