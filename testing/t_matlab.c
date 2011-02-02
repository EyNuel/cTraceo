/*
 * This code tests interfacing with matlab to write a .mat file.
  * clang -I /usr/local/matlabr14/extern/include -L /usr/local/matlabr14/sys/os/glnxa64 -L /usr/local/matlabr14/bin/glnxa64 -leng -lmat -lmex -lut -Wl,-rpath,/usr/local/matlabr14/bin/glnxa64 -o t_matlab.bin t_matlab.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "../tools.c"
//#include "../globals.h"
#include <math.h>
#include <complex.h>
#include "mat.h"
#include "matrix.h"
#include <inttypes.h>

int main(void);

int main(void){
	
	MATFile*	matfile;
	double*		thetas = NULL;
	double* 	ptr;
	uint32_t	i,j;
	
	double*		x = mallocDouble(5);
	double**	y = mallocDouble2D(5,5);
	mxArray*	mtlbDataArray;
	mxArray*	mtlbDataArray2D;

	//fill in values of x an y
	for(i=0; i<5; i++){
		x[i] = i*i;
		for(j=0; j<5; j++){
			y[i][j] = i*j;
		}
	}
	//open matlab file
	matfile 		= matOpen("t_matlab.mat", "w");

	mtlbDataArray = mxCreateDoubleMatrix(1, 5, mxREAL);
	if( mtlbDataArray == NULL ) {
		fatal("Memory Alocation error.");
	}

	copyDoubleToPtr(x, mxGetPr(mtlbDataArray), 5);
	matPutVariable(matfile, "x", mtlbDataArray);
	mxDestroyArray(mtlbDataArray);
	
	mtlbDataArray2D = mxCreateDoubleMatrix(5, 5, mxREAL);
	if( mtlbDataArray2D == NULL ) {
		fatal("Memory Alocation error.");
	}
	copyDoubleToPtr2D(y, mxGetPr(mtlbDataArray2D), 5, 5);
	matPutVariable(matfile, "y", mtlbDataArray2D);
	mxDestroyArray(mtlbDataArray2D);
	

	exit(EXIT_SUCCESS);
}

