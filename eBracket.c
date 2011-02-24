/********************************************************************************
 *	eBracket.c		 															*
 * 	(formerly "ebrckt.for")														*
 *	Extended bracket -bracketing with non-uniformly ordered data.				*
 * 	Given a sequence x0 ... < xn, and a number xi, such that:					*
 * 		 x0 < xi < xn,															*
 * 	with xi not belonging to the sequence, determine j such that:				*
 * 		 xj < xi < x[j+1]														*
 *																				*
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
 * 				n:		number of elements in vector x							*
 * 				x:		vector to be searched									*
 * 				xi:		scalar who's bracketing elements are to be found		*
 * 	Outputs:																	*
 * 				nb:		number of bracketing pairs found.						*
 *				ib:		the indexes of the lower bracketing elements.			*
 * 						Note: shall be previously allocated.					*
 * 																				*
 *******************************************************************************/

#pragma		once
#include	"tools.c"

void	eBracket(uintptr_t, double*, double, uintptr_t*, uintptr_t*);

void	eBracket(uintptr_t n, double* x, double xi, uintptr_t* nb, uintptr_t* ib){
	uintptr_t	i;
	double_t	a, b;
	
	ib[0]	= 0;
	*nb		= 0;
	
	for(i=0; i< n-2; i++){
		a = min( x[i], x[i+1]);
		b = max( x[i], x[i+1]);
		if ( (xi >= a) && (xi < b) && (*nb < 50)){
			*nb = *nb +1;
			ib[*nb] = i;
		}
	}
}
