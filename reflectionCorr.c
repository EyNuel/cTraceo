/************************************************************************************
 *	reflectionCorr.c	 															*
 * 	(formerly "rnrefl.for")															*
 * 	Reflection correction subroutine.												*
 * 																					*
 *	originally written in FORTRAN by:												*
 *  						Orlando Camargo Rodriguez:								*
 *							Copyright (C) 2010										*
 * 							Orlando Camargo Rodriguez								*
 *							orodrig@ualg.pt											*
 *							Universidade do Algarve									*
 *							Physics Department										*
 *							Signal Processing Laboratory							*
 *																					*
 *	Ported to C for project SENSOCEAN by:											*
 * 						Emanuel Ey													*
 *						emanuel.ey@gmail.com										*
 *						Signal Processing Laboratory								*
 *						Universidade do Algarve										*
 *																					*
 *	Inputs:																			*
 * 				iTop	indicates upper or lower boundary.							*
 * 						(see struct ray.boundaryJ in globals.h)						*
 * 				sigma	
 * 				tauB	Tangent to boundary.										*
 * 				gradC	Gradient of soundSpeed.										*
 * 				ci		sound speed at coordinate.									*
 * 	Outputs:																		*
 * 				rn:		
 * 																					*
 ***********************************************************************************/
#pragma 	once
#include "globals.h"
#include "dotProduct.c"

void	reflectionCorr(int32_t, vector_t, vector_t, vector_t, float, float*);
/*
subroutine rnrefl(itop,sigma,tauB,gradC,ci,rn)
integer*8 itop
real*8  sigma(2),sigman(2),gradC(2),tauB(2),nbdy(2)
real*8  tg,th,ci,cn,cs,rm,rn
*/
void	reflectionCorr(int32_t iTop, vector_t sigma, vector_t tauB, vector_t gradC, float ci, float* rn){
	vector_t	nBdy;	//normal to boundary
	vector_t	sigmaN;	//normal to ray?
	float		tg, th, cn, cs, rm;

	iTop = -iTop;
	nBdy.r = -tauB.z;
	nBdy.z =  tauB.r;

	//flip normal if ray is at bottom
	if (iTop == 1){
		nBdy.r = -nBdy.r;
		nBdy.z = -nBdy.z;
	}
	
	sigmaN.r = -sigma.z;
	sigmaN.z = sigma.r;
	
	tg = dotProduct( &sigma, &tauB);
	th = dotProduct( &sigma, &nBdy);
	cn = dotProduct( &gradC, &sigmaN);
	cs = dotProduct( &gradC, &sigma);
	
	rm = tg/th;
	if( iTop == 1){
		cn = -cn;
	}
	
	*rn = rm * (4 * cn - 2 * rm * cs)/ci;
}
