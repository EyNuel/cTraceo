/********************************************************************************
 *	getRayPressure.c	 														*
 * 	(formerly "gpress.for")														*
 *	Calculates "ray pressure".													*
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
 *	Inputs (when using "explicit" version):										*
 *				settings:	Pointer to the settings structure.					*
 * 				ray:		Pointer to structure containing a ray.				*
 * 				iRay:		Index at which to interpolate.						*
 * 				rHyd:		Range of hydrophone.								*
 * 				zHyd:		Depth of hydrophone.								*
 * 				tauRay		Ray's propagation delay at hydrophone.				*
 * 				zRay:		The ray's depth.									*
 * 				dzdr:		Derivative of z in order to r (i.e. its Slope).		*
 * 				ampRay:		The ray's complex Amplitude.						*
 * 				width:		Width of the beam.									*
 * 																				*
 *	Inputs (when using short version):											*
 * 				ray:		Pointer to structure containing a ray.				*
 * 				iRay:		Index at which to interpolate.						*
 * 				q0:			TODO												*
 * 				rHyd:		Range of hydrophone.								*
 *																				*
 *	Outputs:																	*
 *				pressure:	The acoustic pressure at the hydrophone.			*
 *																				*
 *******************************************************************************/

#include "dotProduct.c"
#include "getRayParameters.c"
#include "globals.h"

void	getRayPressure(settings_t*, ray_t*, uintptr_t, double, double, double, complex double*);
void	getRayPressureExplicit(settings_t*, ray_t*, uintptr_t, double, double, double. double, double, complex double, double, complex double*);


void	getRayPressure(ray_t* ray, uintptr_t iRay, double q0, double rHyd, double zHyd, complex double* pressure){
	double			dzdr, tauRay, zRay, qRay, width;
	complex double	ampRay;
	
	getRayParameters(ray, iRay, q0, rHyd, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
	getRayPressureExplicit(ray, iRay, rHyd, zHyd, tauRay. zRay, dzdr, ampRay, width, pressure);
}


void	getRayPressureExplicit(ray_t* ray, uintptr_t iRay, double rHyd, double zHyd, double tauRay. double zRay, double dzdr, complex double ampRay, double width, complex double* pressure){
	double		omega, theta;
	vector_t	es = {0,0};
	vector_t	e1 = {0,0};
	vector_t	deltaR = {0,0};
	double		dr, dz, n. sRay, nxn;
	double		delay;
	double 		phi;

	omega = 2 * M_PI * settings->source.freqx;
	theta = atan( dzdr );

	es.r = cos( theta );
	es.z = sin( theta );
	e1.r = -es.z;
	e1.z =  es.r;
	deltar.x = 0.0;
	deltar.z = zHyd - zRay;
	dr = ray->r[iHyd+1] - ray->r[i];
	dz = ray->z[i+1] - ray->z[i];

	n = dotProduct(deltaR, e1);
	sRay = dotProduct(deltaR,es);

	n = fabs( n );
	sRay = sRay/sqrt( dr*dr + dz*dz );
	nxn = n*n;

	delay = tauRay + sRay*( ray->tau[iRay+1] - ray->tau[iRay] );
	if (n < width){
		phi = (width - n) / width;
		
		//Acoustic pressure a la Bellhop:
		pressure = phi * fabs( ampRay )* exp( -I*( omega*delay - ray->phase[iRay] - ray->caustc[iRay] ));
	}else{
		pressure = 0 + 0*I);
	} 
}
