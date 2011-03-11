/************************************************************************************
 *	linearSpaced.c		 															*
 *	Generate a linearly spaced vector.												*
 *																					*
 *	originally written in FORTRAN by:												*
 *						Orlando Camargo Rodriguez:									*
 *						Copyright (C) 2010											*
 *						Orlando Camargo Rodriguez									*
 *						orodrig@ualg.pt												*
 *						Universidade do Algarve										*
 *						Physics Department											*
 *						Signal Processing Laboratory								*
 *																					*
 *	Ported to C for project SENSOCEAN by:											*
 * 						Emanuel Ey													*
 *						emanuel.ey@gmail.com										*
 *						Signal Processing Laboratory								*
 *						Universidade do Algarve										*
 *																					*
 *	Inputs:																			*
 * 				n:		Number of points to generate.								*
 * 				xMin:	First value of x.											*
 * 				xMax:	Last value of x.											*
  *																					*
 * 	Outputs:																		*
 * 				x:		a pointer to the generated vector							*
 * 						Note that this pointer has the previously allocated to the	*
 * 						correct size.												*
 * 																					*
 ***********************************************************************************/
#pragma once

void	linearSpaced(uint32_t, float, float, float*);

void	linearSpaced(uint32_t n, float xMin, float xMax, float* x){
	uintptr_t	i;
	float	dx;

	DEBUG(10, "n:%u, xMin: %lf, xMax: %lf\n", n, xMin, xMax);
	dx = (xMax - xMin)/((float)(n - 1));
	for(i=0; i<n; i++){
		x[i] = xMin + dx*(float)(i);
	}
}
