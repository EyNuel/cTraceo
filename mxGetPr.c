#pragma once
#include "matlabOut.h"

double* mxGetPr(mxArray* array);

double*	mxGetPr(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	return(array->pr);
}
