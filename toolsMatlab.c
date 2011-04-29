/********************************************************************************
 *  toolsMatlab.c																		*
 * 	Collection of functions to write c arrays (pointers) to mxArrays.			*
 *																				*
 *	Written by:		Emanuel Ey													*
 *					emanuel.ey@gmail.com										*
 *					Signal Processing Laboratory								*
 *					Universidade do Algarve										*
 *																				*
 *******************************************************************************/

#pragma once
#include	<matrix.h>			//for matlab functions (used in copyComplexToPtr and copyComplexToPtr2D)


///Prototypes:
 
void			copyDoubleToPtr(double*, double*, uintptr_t);
void			copyDoubleToPtr2D(double**, double*, uintptr_t, uintptr_t);
void			copyDoubleToPtr2D_transposed(double**, mxArray*, uintptr_t, uintptr_t);
void			copyComplexToPtr(complex double*, mxArray*, uintptr_t);
void			copyComplexToPtr2D(complex double**, mxArray*, uintptr_t, uintptr_t);
void			copyComplexToPtr2D_transposed(complex double**, mxArray*, uintptr_t, uintptr_t);



///Functions:

void		copyDoubleToPtr(double* origin, double* dest, uintptr_t nItems){
	uintptr_t	i;

	for( i=0; i<nItems; i++ ){
		dest[i] = origin[i];
	}
}

void		copyDoubleToPtr2D(double** origin, double* dest, uintptr_t rowSize, uintptr_t colSize){
	uintptr_t	i,j;

	for( j=0; j<colSize; j++ ){
		for(i=0; i<rowSize; i++){
			dest[i*colSize +j] = origin[j][i];
		}
	}
}

void		copyDoubleToPtr2D_transposed(double** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
	uintptr_t	i,j;
	double*	destReal = NULL;
	
	destReal = mxGetData(dest);
	
	for( j=0; j<dimR; j++ ){
		for(i=0; i<dimZ; i++){
			destReal[j*dimZ + i] = origin[j][i];
		}
	}
}

void		copyComplexToPtr(complex double* origin, mxArray* dest, uintptr_t nItems){
	uintptr_t	i;
	double*	destImag = NULL;
	double*	destReal = NULL;
	
	//get a pointer to the real and imaginary parts of the destination:
	destReal = mxGetData(dest);
	destImag = mxGetImagData(dest);
	
	for( i=0; i<nItems; i++ ){
		destReal[i] = creal(origin[i]);
		destImag[i] = cimag(origin[i]);
	}
}

void		copyComplexToPtr2D(complex double** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
	uintptr_t	i,j;
	double*	destImag = NULL;
	double*	destReal = NULL;
	
	//get a pointer to the real and imaginary parts of the destination:
	destReal = mxGetData(dest);
	destImag = mxGetImagData(dest);
	
	for( j=0; j<dimR; j++ ){
		for(i=0; i<dimZ; i++){
			destReal[j + i*dimR] = creal(origin[j][i]);
			destImag[j + i*dimR] = cimag(origin[j][i]);
		}
	}
}

void		copyComplexToPtr2D_transposed(complex double** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
	uintptr_t	i,j;
	double*		destImag = NULL;
	double*		destReal = NULL;
	
	//get a pointer to the real and imaginary parts of the destination:
	destReal = mxGetData(dest);
	destImag = mxGetImagData(dest);
	
	for( j=0; j<dimR; j++ ){
		for(i=0; i<dimZ; i++){
			destReal[j*dimZ + i] = creal(origin[j][i]);
			destImag[j*dimZ + i] = cimag(origin[j][i]);
		}
	}
}
