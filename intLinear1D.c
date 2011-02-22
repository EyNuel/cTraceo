/************************************************************************
 *  intLinear1D.c														*
 * 	(formerly "lini1d.for)												*
 * 	Perform linear interpolation at 1D.									*
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
void intLinear1D(double*, double*, double, double*, double*);

void intLinear1D(double* x, double* f, double xi, double* fi, double* fxi){
	DEBUG(8,"in\n");
	*fxi	= 	( f[1] -f[0]) / ( x[1] -x[0]);
	*fi		=	f[0] +(xi -x[0]) *(*fxi);
	DEBUG(8,"out\n");
}
