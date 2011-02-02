/*
 * This code tests interfacing with matlab to write a .mat file.
  * clang -I /usr/local/matlabr14/extern/include -L /usr/local/matlabr14/sys/os/glnxa64 -L /usr/local/matlabr14/bin/glnxa64 -leng -lmat -lmex -lut -Wl,-rpath,/usr/local/matlabr14/bin/glnxa64 -o t_matlab.bin t_matlab.c
 */

#include <stdio.h>
#include <stdlib.h>
//#include "../tools.c"
//#include "../globals.h"
#include <math.h>
#include <complex.h>
#include "mat.h"
#include "matrix.h"
#include <inttypes.h>

int main(void);

int main(void){
	
	MATFile*	mp;
	mxArray*	pthtas;
	double*		thetas = NULL;
	double* 	ptr;
	uint32_t	i;
	
	double		x[5];
	double*		data;
	mxArray*	mtlbDataArray;

	// code to fill in values of x
	for(i=0; i<5; i++){
		x[i] = i*i;
	}
	// code to open engine
	mp 		= matOpen("t_matlab.mat", "w");

	
	mtlbDataArray = mxCreateDoubleMatrix(1, 5, mxREAL);
	if( mtlbDataArray == NULL ) {
		//fatal("Memory Alocation error.");
	}
	data = mxGetPr(mtlbDataArray);
	for( i=0; i<5; i++ ){
	   data[i] = x[i];
	}
	matPutVariable(mp, "myArray", mtlbDataArray);
	mxDestroyArray(mtlbDataArray);

	exit(EXIT_SUCCESS);
}

