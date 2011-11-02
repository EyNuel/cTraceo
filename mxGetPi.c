#pragma once
#include "matlabOut.h"

double* mxGetPi(mxArray* array);

double*	mxGetPi(mxArray* array){
	/*
	 * Returns pointer to the first element of the mxArray's imaginary data.
	 * NOTE: in the matlab API this functions returns a void* pointer.
	 */
	if (array->numericType == mxCOMPLEX){
		return(array->pi);
	}else{
		fatal("mxGetPi(): mxArray does not contain imaginary data.");
	}
	return NULL;
}
