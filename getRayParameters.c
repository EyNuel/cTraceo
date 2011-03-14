/********************************************************************************
 *	getRayParameters.c	 														*
 * 	(formerly "grayp.for")														*
 *	interpolates ray parameters													*
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
 * 				ray:	Pointer to structure containing a ray.					*
 * 				iRay:	Index at which to interpolate.							*
 * 				q0:		TODO
 * 				rHyd:	Range of hydrophone.									*
 * 	Outputs:																	*
 * 				dzdr:	Derivative of z in order to r (i.e. its Slope).			*
 *				tauRay:	The ray's propagation delay at hydrophone.				*
 * 				zRay:	The ray's depth.										*
 * 				ampRay:	The ray's complex Amplitude.							*
 * 				qRay:	TODO
 * 				width:	Width of the beam.										*
 * 																				*
 *******************************************************************************/

#include "globals.h"
#include "interpolation.h"
#include <complex.h>
#include "tools.c"

void	getRayParameters(ray_t*, uintptr_t, double, , double, double*, double*, double*, double complex*, double*, double*);

void	getRayParameters(ray_t* ray, uintptr_t iRay, double q0, double rHyd, double* dzdr, double* tauRay, double* zRay, double complex* ampRay, double* qRay, double* width){

	complex double	junkComplex;
	double			junkDouble;
	double			theta;

	if( ray->iRefl[iRay+1] == TRUE){
		iRay = iRay - 1;
	}

	intLinear1D(		&ray->r[iRay], &ray->z[iRay],	rHyd, zRay,		dzdr);
	intLinear1D(		&ray->r[iRay], &ray->tau[iRay],	rHyd, tauRay,	junkDouble);
	intComplexLinear1D(	&ray->r[iRay], &ray->amp[iRay],	rHyd, ampRay,	junkComplex);
	intLinear1D(		&ray->r[iRay], &ray->q[iRay],	rHyd, qRay,		junkDouble);

	theta = atan( *dzdr );
	*width = max( fabs( ray->q[iHyd] ), fabs( ray->q[iHyd+1]) );
	*width = *width / ((*q0) * cos(theta));
}
