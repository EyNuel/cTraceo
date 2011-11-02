#pragma once
#include "matlabOut.h"

double* mxGetData(mxArray* array);

double*	mxGetData(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's real data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 * NOTE: This is actually identical to mxGetPr()
	 */
	return(array->pr);
}

