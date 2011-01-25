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
 * 																					*
 ***********************************************************************************/

void	linearSpaced(double*, double*, double*, double*);

void	linearSpaced(double* n, double* xMin, double* xMax, double* x){
	uintptr_t	i;
	double_t	dx;
	
	dx = (xMax - xMin)/(*n - 1);
	for(i=0; i<(*n), i++){
		x[i] = xmin + dx*(i-1)
	}
	return x
}
