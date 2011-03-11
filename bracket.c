/********************************************************************************
 *	bracket.c		 															*
 * 	(formerly "brcket.for")														*
 * 	Given a sequence x0 < x1 < ... <xj < ... < xn, and a number xi, such that:	*
 * 		 x0 < xi < xn,															*
 * 	with xi not belonging to the sequence, determine j such that:				*
 * 		 xj < xi < x[j+1]														*
 *																				*
 *	Used to determine between which values to interpolate.						*
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
 * 				n:		number of elements in vector x							*
 * 				x:		vector to be searched									*
 * 				xi:		scalar who's bracketing elements are to be found		*
 * 	Outputs:																	*
 * 				i:		the index of the lower bracketing element.				*
 * 																				*
 *******************************************************************************/

#pragma once

void	bracket(uintptr_t, float*, float, uintptr_t*);

void	bracket(uintptr_t n, float* x, float xi, uintptr_t* i){
	DEBUG(5, "Entering bracket().\n");
	uintptr_t	ia,im,ib;

	ia = 0;
	ib = n-1;

DEBUG(5,"n: %u\n",		(uint32_t)n);
DEBUG(5,"xi: %e\n",		xi);
DEBUG(5,"x[0]: %e\n",	x[0]);
DEBUG(5,"[n-1]: %e\n",	x[n-1]);

	//if xi is outside the interval [ x[0], x[n-1] ], quit
	if( (xi < x[0]) || (xi > x[n-1])){
		printf("n: %u\n",		(uint32_t)n);
		printf("xi: %e\n",		xi);
		printf("x[0]: %e\n",	x[0]);
		printf("[n-1]: %e\n",	x[n-1]);
		fatal("Bounding error in bracket().\nAborting.");
	}else{
		while( ib-ia > 1){
			//If xi is inside the interval [x(1) x(n)] just divide it by half until ib - ia = 1:
			im = (ia+ib)/2;
			if( (xi >= x[ia] ) && ( xi < x[im])){
				ib = im;
			}else{
				ia = im;
			}
			//printf("ia:%u; im:%u; ib:%u\n",ia, im, ib);
		}
		*i = ia;
	}
	DEBUG(5, "Leaving bracket().\n");
}
