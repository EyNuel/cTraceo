/************************************************************************************
 *	boundaryReflectionCoeff.c														*
 *	(formerly "bdryr.for")															*
 *	Calculates the reflection coefficient of a boudary between to media.			*
 *																					*
 *	Reference: "Recovery of the properties of an elastic bottom using reflection	*
 *	coefficient measurements", P.J.Papadakis et al, Proc. of the 2nd. ECUA, Vol II,	*
 *	page 943, 1994.																	*
 * 																					*
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
 * 				TODO: correct this:
 * 				aIn:	Atenuation value to be converted.							*
 * 				lambda:	Wavelength.													*
 * 				freq:	Frequency.													*
 * 				units:	Determines the input units.									*
 * 						(see globals.h for possible values)							*
 *																					*
 * 	Outputs:																		*
 * 				aOut:	Converted atenuation value.									*
 * 																					*
 ***********************************************************************************/

#include <complex.h>
#include <math.h>

void	boundaryReflectionCoeff(double*, double*, double*, double*, double*, double*, double*, double*, complex*);

void	boundaryReflectionCoeff(double* rho1, double* rho2, double* cp1, double* cp2, double* cs2, double* ap,
								double* as, double* theta, complex* refl){

	double		tilap, tilas;
	double		a1, a2, a3, a4, a5, a6, d;
	complex		tilcp2, tilcs2;
	
	tilap = ap/( 40.0 * M_PI * M_LOG10E );
	tilas = as/( 40.0 * M_PI * M_LOG10E );
	
	tilcp2 = cp2 * (1 - I * tilap) / (1 + tilap * tilap);
	tilcs2 = cs2 * (1 - I * tilas) / (1 + tilas * tilas);
	
	a1	= rho2 / rho1;
	a2	= tilcp2 / cp1;
	a3	= tilcs2 / cp1;
	a4	= a3 * sin( *theta );
	a5	= 2*a4 * a4;
	a6	= a2 * sin( *theta );
	a7	= 2*a5 - a5*a5;
	
	d	= a1 * ( a2 * (1 - a7 ) / sqrt( 1 - a6 * a6 ) + a3 * a7 / sqrt(1 - 0.5*a5) );
	
	*refl = ( d * cos( *theta ) -1) / ( d * cos( *theta ) + 1);
}

