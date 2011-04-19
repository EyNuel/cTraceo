/********************************************************************************
*	pressureStar.c	 															*
* 	(formerly "star.for")														*
*	Determines the star pressure contributions for particle velocity components	*
*	of a specific ray at a specify coordinate for rays that bounce back			*
*	(ie "returning rays").														*
*	When rays return they may influence a hydrophone more than once, hence we	*
*	need to use eBracket to find all indices at which the ray passes the hydr.	*
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

/*
	   subroutine mstar(imax,rc,zc,q0,irefl,dr,dz,pl,pc,pr,pu,pd)

c***********************************************************************
c	  Multiple STAR pressure contributions for velocity components
c	  Written by Orlando Camargo Rodriguez
c	  orodrig@ualg.pt
c	  Universidade do Algarve
c	  Physics Department
c	  Signal Processing Laboratory
c	  Faro, 22/11/2010 at 19:30
c****&******************************************************************

c	  Include global (common) variables:

	   include 'global.for'

c	  Define local variables:

	   integer*8 irefl(np)
	   integer*8 iret(51)
	   integer*8 i,imax,jj,nret

	   real*8 xl(2),yl(2)

	   real*8 rl,rc,rr,zu,zc,zd,dr,dz
	   real*8 tauray,zray,qray,dzdr,thta,q0,w

	   real*8 dummy

	   complex*8 aray,pl,pc,pr,pu,pd,p0

	   complex*8 dummi

c***********************************************************************

	   p0 = (0.0 , 0.0)
	   pl = (0.0 , 0.0)
	   pc = (0.0 , 0.0)
	   pr = (0.0 , 0.0)
	   pu = (0.0 , 0.0)
	   pd = (0.0 , 0.0)
*/

#pragma once
#include "globals.h"
#include <complex.h>
#include "eBracket.c"

uintptr_t	pressureMStar(settings_t*, ray_t*, double, double, double, complex double*, complex double[]);

uintptr_t	pressureMStar( settings_t* settings, ray_t* ray, double rHyd, double zHyd, double q0, complex double* pressure_H, complex double* pressure_V){

	double				rLeft, rRight, zTop, zBottom;
	uintptr_t			i, jj;
	double				dzdr, tauRay, zRay, qRay, width;
	complex double		ampRay;
	uintptr_t			nRet;
	uintptr_t			iRet[51];
	complex double		tempPressure[3];
	
	/* start with determining the coordinates for which we will need to calculate
	 * acoustic pressure.
	 */
	rLeft	= rHyd - settings->output.dr;
	rRight	= rHyd + settings->output.dr;
	zBottom	= zHyd - settings->output.dz;
	zTop	= zHyd + settings->output.dz;

	//we have to check that these points are within the rBox:
	if(	rLeft	<	settings->source.rbox1 ||
		rRight	>=	settings->source.rbox2 ){
		DEBUG(8, "Either rLeft or rRight are outside rBox\n");
		return 0;
	}
	
	
	
	//if(eBracket(ray->nCoords, ray[i].r, rLeft, &nRet, iRet)){
		eBracket(ray->nCoords, ray[i].r, rLeft, &nRet, iRet);
		DEBUG(1,"nRet: %u\n", (uint32_t)nRet);
		// NOTE:	this block will not be run if the index returned by bracket() is out of bounds.
		for(jj=0; jj<nRet; jj++){
			getRayParameters(ray, iRet[jj], q0, rLeft, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
			DEBUG(8, "dzdr: %e, tauRay: %e, zRay: %e, ampRay: %e, qRay: %e, w: %e\n", dzdr, tauRay, zRay, cabs(ampRay), qRay, width);
			getRayPressureExplicit(settings, ray, iRet[jj], zHyd, tauRay, zRay, dzdr, ampRay, width, &tempPressure[LEFT]);
			pressure_H[LEFT] += tempPressure[LEFT];
		}
	/*
	}else{
		DEBUG(6, "rLeft (%lf) can not be bracketed.\n", rLeft);
		return 0;
	}
	*/
	
	
	
	//if(eBracket(ray->nCoords, ray[i].r, rHyd, &nRet, iRet)){
		eBracket(ray->nCoords, ray[i].r, rHyd, &nRet, iRet);
		DEBUG(1,"nRet: %u\n", (uint32_t)nRet);
		for(jj=0; jj<nRet; jj++){
			getRayParameters(ray, iRet[jj], q0, rHyd, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
			getRayPressureExplicit(settings, ray, iRet[jj], zTop, tauRay, zRay, dzdr, ampRay, width, &tempPressure[TOP]);
			getRayPressureExplicit(settings, ray, iRet[jj], zHyd, tauRay, zRay, dzdr, ampRay, width, &tempPressure[CENTER]);
			getRayPressureExplicit(settings, ray, iRet[jj], zBottom, tauRay, zRay, dzdr, ampRay, width, &tempPressure[BOTTOM]);
			pressure_V[TOP]		+= tempPressure[TOP];
			pressure_V[CENTER]	+= tempPressure[CENTER];
			pressure_H[CENTER]	+= tempPressure[CENTER];
			pressure_V[BOTTOM]	+= tempPressure[BOTTOM];
		}
	/*
	}else{
		DEBUG(6, "rHyd (%lf) can not be bracketed.\n", rHyd);
		return 0;
	}
	*/
	
	
	
	//if(eBracket(ray->nCoords, ray[i].r, rRight, &nRet, iRet)){
	eBracket(ray->nCoords, ray[i].r, rRight, &nRet, iRet);
	DEBUG(1,"nRet: %u\n", (uint32_t)nRet);
		for(jj=0; jj<nRet; jj++){
			getRayParameters(ray, iRet[jj], q0, rRight, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
			getRayPressureExplicit(settings, ray, iRet[jj], zHyd, tauRay, zRay, dzdr, ampRay, width, &tempPressure[RIGHT]);
			pressure_H[RIGHT]	+= tempPressure[RIGHT];
		}
	/*
	}else{
		DEBUG(6, "rRight (%lf) can not be bracketed.\n", rRight);
		return 0;
	}
	*/

	//success
	return 1;
}
