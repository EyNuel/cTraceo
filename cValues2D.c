/********************************************************************************
 *	cValues2D.c		 															*
 * 	(formerly "cvals2.for")														*
 * 	Perform 2-Dimensional piecewise Interpolation of sound speed and its		*
 *	derivatives.																*
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
 *	Ported to C for project SENSOCEAN by:										*
 * 						Emanuel Ey												*
 *						emanuel.ey@gmail.com									*
 *						Signal Processing Laboratory							*
 *						Universidade do Algarve									*
 *																				*
 *	Inputs:																		*
 * 				nx:		number of elements in vector x.							*
 * 				ny:		number of elements in vector y.							*
 * 				xTable:	vector containing independent variable x.				*
 * 				yTable:	vector containing independent variable y.				*
 * 				cTable:	array containing c(x, y)								*
 * 				xi:		x coordinate of interpolation point.					*
 * 				yi:		y coordinate of interpolation point.					*
 * 	Outputs:																	*
 * 				ci:		interpolated value c(xi, yi).							*
 * 				cxi:	1st partial derivative of c in order to x at (xi, yi).	*
 * 				cyi:	1st partial derivative of c in order to y at (xi, yi).	*
 * 				cxxi:	2nd order partial derivative of c in order to x.		*
 * 				cyyi:	2nd order partial derivative of c in order to y.		*
 * 				cxyi:	2nd order partial derivative of c in order to x and y.	*
 * 																				*
 *******************************************************************************/

#include "bracket.c"
#include "globals.h"

void	cValues2D(uintptr_t, uintptr_t, double*, double*, double**, double*, double*, double*, double*, double*, double*, double*, double*);

void	cValues2D(uintptr_t nx, uintptr_t ny, double* xTable, double* yTable, double** cTable, double* xi, double* yi, double* ci, double* cxi, double* cyi, double* cxxi, double* cyyi, double* cxyi){
	DEBUG(8, "in: nx: %u, ny: %u, xi: %lf (x[nx-2]: %lf), yi: %lf (y[ny-2]: %lf)\n", (uint32_t)nx, (uint32_t)ny, *xi, xTable[nx-2], *yi, yTable[ny-2]);
	uintptr_t	i=0, j=0, a, b;
	double**	tempDouble2D = mallocDouble2D(3,3);	//TODO this causes a memory leak -fix!
	
	if (*xi <= xTable [1]){
		i = 0;
	}else if (*xi >= xTable[nx-2]){
		i = nx - 3;
	}else{
		bracket(nx, xTable, xi, &i);
	}

	if (*yi <= yTable[1]){
		j = 0;
	}else if (*yi >= yTable[ny -2]){
		j = ny - 3;
	}else{
		bracket(ny, yTable, yi, &j);
	}

	for(a=0; a<3; a++){
		for(b=0; b<3; b++){
			DEBUG(8, "tempDouble2D[%u][%u] = cTable[%u][%u] = ", (uint32_t)a, (uint32_t)b, (uint32_t)(j+a), (uint32_t)(i+b));
			tempDouble2D[a][b] = cTable[j+a][i+b];
			DEBUG(8, "%lf.\n", cTable[j+a][i+b]);
		}
	}

	intBarycParab2D( &xTable[i], &yTable[j], tempDouble2D, *xi, *yi, ci, cxi, cyi, cxxi, cyyi, cxyi);
	freeDouble2D(tempDouble2D,3);
	DEBUG(8, "out, ci: %lf\n", *ci);
}

