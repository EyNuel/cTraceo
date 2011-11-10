#pragma once
#include <stdint.h>
#include "matlabOut.h"

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
	 * 		This implementatio only supports up to 2 dimensions, as no nedd for more
	 * 		in cTraceo.
	 */
	
	uintptr_t index = 0;
	uintptr_t col, row, nRows;
	
	row		= subs[0];
	col		= subs[1];
	nRows	= inArray->dims[0];
	index	= col + row * nRows;
	
	return index;
}
