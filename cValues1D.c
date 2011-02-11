/********************************************************************************
 *	cValues1D.c		 															*
 * 	(formerly "cvals1.for")														*
 * 	Perform 1-Dimensional Interpolation of sound speed and its derivatives.		*
 * 																				*
 *	originally written in FORTRAN by:											*
 *  					Orlando Camargo Rodriguez:								*
 *						Copyright (C) 2010										*
 * 						Orlando Camargo Rodriguez								*
 *						orodrig@ualg.pt											*
 *						Universidade do Algarve									*
 *						Physics Department										*
 *						Signal Processing Laboratory							*
 *																				*
 *	Ported to C by:		Emanuel Ey												*
 *						emanuel.ey@gmail.com									*
 *						Signal Processing Laboratory							*
 *						Universidade do Algarve									*
 *																				*
 *	Inputs:																		*
 * 				n:		number of elements in vectors x anc c.					*
 * 				xTable:	vector containing independent variable.					*
 * 				cTable:	vector containing c(x) 									*
 * 				xi:		interpolation point.									*
 * 	Outputs:																	*
 * 				ci:		interpolated value c(xi).								*
 * 				cxi:	1st derivative of c at xi.								*
 * 				cxxi:	2nd derivative of c at xi.								*
 * 																				*
 *******************************************************************************/
#pragma once
#include "interpolation.h"
#include "bracket.c"

void	cValues1D(uintptr_t, double*, double*, double*, double*, double*, double*);

void	cValues1D(uintptr_t n, double* xTable, double* cTable, double* xi, double* ci, double* cxi, double* cxxi){
	DEBUG(10,"Entering cValues1D().\n");
	uintptr_t	i = 0;

	//TODO invert order if clauses:
	
	if( *xi < xTable[1]){
		//if xi is in first interval of xTable, do linear interpolation
		intLinear1D(	&xTable[0],
						&cTable[0],
						xi, ci, cxi);
		*cxxi = 0.0;
		
	}else if( *xi >= xTable[n-2]){
		//if xi is in last interval of xTable, do linear interpolation
		intLinear1D(	&xTable[n-2],
						&cTable[n-2],
						xi, ci, cxi);
		*cxxi = 0.0;
		
	}else if(( *xi >= xTable[1] ) && ( *xi < xTable[2])){
		//if xi is in second interval of xTable, do barycentric parabolic interpolation
		intBarycParab1D(	&xTable[0],
							&cTable[0],
							xi, ci, cxi, cxxi);
		
	}else if(( *xi >= xTable[n-3] ) && ( *xi < xTable[n-2] )){
		//if xi is in second to last interval of xTable, do barycentric parabolic interpolation
		intBarycParab1D(	&xTable[n-3],
							&cTable[n-3],
							xi, ci, cxi, cxxi);
		
	}else{
		//for all other cases do barycentric cubic interpolation
		bracket(n, xTable, xi, &i);
		intBarycCubic1D(	&xTable[i-1],
							&cTable[i-1],
							xi, ci, cxi, cxxi);

	}
	DEBUG(10,"Leaving cValues1D.\n");
}

