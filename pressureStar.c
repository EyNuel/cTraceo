/********************************************************************************
*	pressureStar.c	 															*
* 	(formerly "star.for")														*
*	Determines the star pressure contributions for particle velocity components	*
*	of a specific ray at a specify coordinate.									*
*																				*
*	originally written in FORTRAN by:											*
*  						Orlando Camargo Rodriguez:								*
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
*				settings:	Pointer to structure containing all input info.		*
*				ray:		Pointer the the ray, who's influence we are			*
*							determining.										*
*				rHyd:		Range coordinate of the hydrophone.					*
*				zHyd:		Depth coordinate of the hydrophone.					*
*				q0:			Value of q at beginning of ray.						*
*	Outputs:																	*
*				pressure_H:	A 3 element array containing the horizontal			*
* 							pressure components ( LEFT, CENTER, RIGHT).			*
* 				pressure_V:	A 3 element array containing the vertical			*
* 							pressure components ( TOP, CENTER, BOTTOM).			*
* 							NOTE:	there is redundacy in the above arrays: the	*
* 									center component is repeated for 			*
* 									performance reasons.						*
*	Return Value:																*
*				0:			At least one of the star coordinates (rLeft, rRight,*
*							zBottom, zTop) is outside the rays' range or rBox.	*
*							Do not use the outputs -doing so will result in a	*
*							segfault.											*
*				1:			Outputs are valid.									*
*																				*
*********************************************************************************/

#pragma once
#include "globals.h"
#include <complex.h>

uintptr_t	pressureStar(settings_t*, ray_t*, double, double, double, complex double*, complex double[]);

uintptr_t	pressureStar( settings_t* settings, ray_t* ray, double rHyd, double zHyd, double q0, complex double* pressure_H, complex double* pressure_V){

	double			rLeft, rRight, zTop, zBottom;
	uintptr_t		iHyd;
	double			dzdr, tauRay, zRay, qRay, width;
	complex double	ampRay;
	
	/*
	 * TODO check if this would ruin the result:
	pressure1D=  0 + 0*I;
	pressureStar1D[_LEFT]	= 0 + 0*I;
	pressureStar1D[_RIGHT]	= 0 + 0*I;
	pressureStar1D[_UP]		= 0 + 0*I;
	pressureStar1D[_DOWN]	= 0 + 0*I;
	*/
	
	/*
	 * start with determining the coordinates for which we will need to calculate
	 * acoustic pressure.
	 */
	rLeft	= rHyd - settings->output.dr;
	rRight	= rHyd + settings->output.dr;
	zBottom	= zHyd - settings->output.dz;
	zTop	= zHyd + settings->output.dz;
	
	DEBUG(1,"rLeft: %lf\n", rLeft);
	DEBUG(1,"rRight: %lf\n", rRight);
	DEBUG(1,"zBottom: %lf\n", zBottom);
	DEBUG(1,"zTop: %lf\n", zTop);
	
	//we have to check that these points are within the rBox:
	if(	rLeft	<	settings->source.rbox1 ||
		rRight	>=	settings->source.rbox2 ){
		DEBUG(1, "Either rLeft or rRight are outside rBox");
		return 0;
	}
	
	//TODO check if zBottom/zTop are below bottom/above surface?
	
	//find out at what index of the ray coordinates the hydrophone is located:
	if( bracket(ray->nCoords, ray->r, rLeft, &iHyd) ){
		/*
		 * NOTE:	this block will not be run if the index returned by
		 * 			bracket() is out of bounds.
		 */
		if ( iHyd<ray->nCoords-1 ){
			getRayParameters(ray, iHyd, q0, rLeft, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
			getRayPressureExplicit(settings, ray, iHyd, zHyd, tauRay, zRay, dzdr, ampRay, width, &pressure_H[LEFT]);
			//getRayPressure( settings, &ray[i], iHyd, q0, rLeft, zHyd,	&pressureStar1D[_LEFT]);
		}
	}else{
		return 0;
	}
	
	if( bracket(ray->nCoords, ray->r, rHyd, &iHyd)){
		if ( iHyd<ray->nCoords-1 ){
			getRayParameters(ray, iHyd, q0, rHyd, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
			getRayPressureExplicit(settings, ray, iHyd, zTop,	tauRay, zRay, dzdr, ampRay, width, &pressure_V[TOP]);
			getRayPressureExplicit(settings, ray, iHyd, zHyd,	tauRay, zRay, dzdr, ampRay, width, &pressure_V[CENTER]);
			getRayPressureExplicit(settings, ray, iHyd, zBottom,	tauRay, zRay, dzdr, ampRay, width, &pressure_V[BOTTOM]);
			pressure_H[CENTER] = pressure_V[CENTER];
		}
	}else{
		return 0;
	}
	
	if(	bracket(ray->nCoords, ray->r, rRight, &iHyd)){
		if ( iHyd<ray->nCoords-1 ){
			DEBUG(1, "r: %lf, z: %lf, amp:%lf, iHyd: %u\n", ray->r[iHyd], ray->z[iHyd], cabs(ray->amp[iHyd]), (uint32_t)iHyd);
			getRayParameters(ray, iHyd, q0, rRight, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
			getRayPressureExplicit(settings, ray, iHyd, zHyd,	tauRay, zRay, dzdr, ampRay, width, &pressure_H[RIGHT]);
		}
	}else{
		return 0;
	}
	
	/*
	pressure_H[0] = 1 +I;
	pressure_H[1] = 1 +I;
	pressure_H[2] = 1 +I;
	*/
	return 1;
	/*
	uintptr_t i;
	for (i=0; i<3 ; i++){
		printf(" %lf, %lf\n", cabs(pressure_H[i]), cabs(pressure_V[i]) );
	}
	*/
	
}
