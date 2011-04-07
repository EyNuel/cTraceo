/********************************************************************************
*	pressureStar.c	 															*
* 	(formerly "star.for")														*
*	STAR pressure contributions for particle velocity components.				*
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
* 				settings:	Pointer to structure containing all input info.		*
* 	Outputs:																	*
* 				none:		Writes to file "cpr.mat".							*
* 																				*
*******************************************************************************/

#pragma once
#include "globals.h"

void	pressureStar(settings_t*, ray_t*, double, double, double, complex double*, complex double[]);

void	pressureStar( settings_t* settings, ray_t* ray, double rHyd, double zHyd, double q0, complex double* pressure, complex double pressureStar1D[]){

	double			rLeft, rRight, zDown, zUp;
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
	
	rLeft	= rHyd - settings->output.dr;
	rRight	= rHyd + settings->output.dr;
	zDown	= zHyd - settings->output.dz;
	zUp		= zHyd + settings->output.dz;
	
	bracket(ray->nCoords, ray->r, rLeft, &iHyd);
	
	if ( iHyd<ray->nCoords-1 ){
		getRayParameters(ray, iHyd, q0, rLeft, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
		getRayPressureExplicit(settings, ray, iHyd, zHyd, tauRay, zRay, dzdr, ampRay, width, &pressureStar1D[_LEFT]);
		//getRayPressure( settings, &ray[i], iHyd, q0, rLeft, zHyd,	&pressureStar1D[_LEFT]);
	}
	
	bracket(ray->nCoords, ray->r, rHyd, &iHyd);
	
	if ( iHyd<ray->nCoords-1 ){
		getRayParameters(ray, iHyd, q0, rHyd, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
		getRayPressureExplicit(settings, ray, iHyd, zUp,	tauRay, zRay, dzdr, ampRay, width, &pressureStar1D[_UP]);
		getRayPressureExplicit(settings, ray, iHyd, zHyd,	tauRay, zRay, dzdr, ampRay, width, pressure);
		getRayPressureExplicit(settings, ray, iHyd, zDown,	tauRay, zRay, dzdr, ampRay, width, &pressureStar1D[_DOWN]);
	}
	
	bracket(ray->nCoords, ray->r, rRight, &iHyd);
	
	if ( iHyd<ray->nCoords-1 ){
		getRayParameters(ray, iHyd, q0, rRight, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
		getRayPressureExplicit(settings, ray, iHyd, zHyd,	tauRay, zRay, dzdr, ampRay, width, &pressureStar1D[_RIGHT]);
	}
}
