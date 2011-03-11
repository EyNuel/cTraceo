/************************************************************************
 *  intComplexLinear1D.c												*
 * 	(formerly "clin1d.for)												*
 * 	Perform linear interpolation at 1D using imaginary numbers.			*
 * 																		*
 *	originally written in FORTRAN by:									*
 *  					Orlando Camargo Rodriguez:						*
 *						Copyright (C) 2010								*
 * 						Orlando Camargo Rodriguez						*
 *						orodrig@ualg.pt									*
 *						Universidade do Algarve							*
 *						Physics Department								*
 *						Signal Processing Laboratory					*
 *																		*
 *	Ported to C by:		Emanuel Ey										*
 *						emanuel.ey@gmail.com							*
 *						Signal Processing Laboratory					*
 *						Universidade do Algarve							*
 *																		*
 *	Inputs:																*
 * 				x:		vector containing x0, x1						*
 * 				f:		vector containing f0, f1						*
 * 				xi:		scalar (the interpolation point)				*
 * 	Outputs:															*
 * 				fi:		interpolated value of f at xi					*
 * 				fxi:	derivative of f at xi							*
 * 																		*
 ************************************************************************/
#pragma once
#include <complex.h>
void intComplexLinear1D(float*, complex float*, complex float, complex float*, complex float*);

void intComplexLinear1D(float* x, complex float* f, complex float xi, complex float* fi, complex float* fxi){
	DEBUG(8,"in\n");
	*fxi	= 	( f[1] -f[0]) / ( x[1] -x[0]);
	*fi		=	f[0] +(xi -x[0]) *(*fxi);
	DEBUG(8,"out\n");
}

