/********************************************************************************
 *	calcParticleVel.c	 														*
 * 	(formerly "calpvl.for")														*
 *	Calculates particle velocity from coherent acoustic pressure.				*
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
 * 				settings:	Pointer to structure containing all input info.		*
 * 	Outputs:																	*
 * 				none:		Writes to file "pvl.mat".							*
 * 																				*
 *******************************************************************************/

#include "globals.h"
#include <complex.h>
#include <mat.h>
#include <matrix.h>
#include "interpolation.h"

void calcParticleVel(settings_t*);

void calcParticleVel(settings_t* settings){
	MATFile*			matfile	= NULL;
	mxArray*			pTitle	= NULL;
	mxArray*			pu2D;
	mxArray*			pw2D;
	uintptr_t			j, k;
	double				rHyd, zHyd;
	double				xp[3];
	double 				dr, dz;		//used locally to make code more efficient (and more readable)
	complex double 		junkComplex, dP_dRi, dP_dZi;
	complex double**	dP_dR2D = NULL;
	complex double**	dP_dZ2D = NULL;


	matfile		= matOpen("pvl.mat", "u");		//open file in "update" mode
	
	//write title to matfile:
	pTitle = mxCreateString("TRACEO: Coherent Acoustic Pressure");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);
	
	//get dr, dz:
	dr = settings->output.dr;
	dz = settings->output.dz;
	
	/*
	 *	Horizontal and vertical pressure components where calculated in calcCohAcoustpress.
	 *	We can now use this to calculate the actual particle velocity components:
	 */
	
	DEBUG(1, "absolute pressure values at the array:\n");
	switch(settings->output.arrayType){
		case ARRAY_TYPE__HORIZONTAL:
		case ARRAY_TYPE__VERTICAL:
		case ARRAY_TYPE__RECTANGULAR:
			dP_dR2D = mallocComplex2D(settings->output.nArrayR, settings->output.nArrayZ);
			dP_dZ2D = mallocComplex2D(settings->output.nArrayR, settings->output.nArrayZ);
			for(j=0; j<settings->output.nArrayR; j++){
				//TODO invert for-loops to improove performance (by looping over rightmost element of array)
				rHyd = settings->output.arrayR[j];
				for(k=0; k<settings->output.nArrayZ; k++){
					zHyd = settings->output.arrayZ[k],
					
					//TODO	get these values from a struct, instead of calculating them again?
					//		(they where previously calculated in pressureStar.c)
					
					xp[0] = rHyd - dr;
					xp[1] = rHyd;
					xp[2] = rHyd + dr;
					
					intComplexBarycParab1D(xp, settings->output.pressure_H[j][k], rHyd, &junkComplex, &dP_dRi, &junkComplex);
					
					dP_dR2D[j][k] = -I*dP_dRi;
					
					xp[0] = zHyd - dz;
					xp[1] = zHyd;
					xp[2] = zHyd + dz;
					
					intComplexBarycParab1D(xp, settings->output.pressure_V[j][k], zHyd, &junkComplex, &dP_dZi, &junkComplex);
					
					dP_dZ2D[j][k] = I*dP_dZi;
					
					//show the pressure contribuitions:
					/*
					DEBUG(1, "(j,k)=(%u,%u)>> pL: %e,  pU, %e,  pR: %e,  pD: %e,  pC:%e\n",
							(uint32_t)j, (uint32_t)k, cabs(settings->output.pressure_H[j][k][LEFT]),
							cabs(settings->output.pressure_V[j][k][TOP]), cabs(settings->output.pressure_H[j][k][RIGHT]),
							cabs(settings->output.pressure_V[j][k][BOTTOM]), cabs(settings->output.pressure_H[j][k][CENTER]));
					*/
					DEBUG(7, "(j,k)=(%u,%u)>> dP_dR: %e, dP_dZ: %e\n",
							(uint32_t)j, (uint32_t)k,
							cabs(dP_dR2D[j][k]), cabs(dP_dZ2D[j][k]));
				}
			}
			break;
		case ARRAY_TYPE__LINEAR:
		/*	TODO
			dP_dR2D = mallocComplex2D(settings->output.nArrayR, settings->output.nArrayZ);
			dP_dZ2D = mallocComplex2D(settings->output.nArrayR, settings->output.nArrayZ);
			for(j=0; j<settings->output.nArrayR; j++){
				//TODO invert for-loops to improove performance (by looping over rightmost element of array)
				rHyd = settings->output.arrayR[j];
				for(k=0; k<settings->output.nArrayZ; k++){
					zHyd = settings->output.arrayZ[k],
					
					//TODO	get these values from a struct, instead of calculating them again?
					//		(they where previously calculated in pressureStar.c)
					
					xp[0] = rHyd - dr;
					xp[1] = rHyd;
					xp[2] = rHyd + dr;
					
					intComplexBarycParab1D(xp, settings->output.pressure_H[j][k], rHyd, &junkComplex, &dP_dRi, &junkComplex);
					
					dP_dR2D[j][k] = -I*dP_dRi;
					
					xp[0] = zHyd - dz;
					xp[1] = zHyd;
					xp[2] = zHyd + dz;
					
					intComplexBarycParab1D(xp, settings->output.pressure_V[j][k], zHyd, &junkComplex, &dP_dZi, &junkComplex);
					
					dP_dZ2D[j][k] = -I*dP_dZi;
				}
			}
			break;
		*/
	}

	/**
	 *	Write the data to the output file:
	 */
	switch(	settings->output.arrayType){
		case ARRAY_TYPE__RECTANGULAR:
		case ARRAY_TYPE__VERTICAL:
		case ARRAY_TYPE__HORIZONTAL:
			DEBUG(3,"Writing pressure output of rectangular array to file:\n");
			
			/// **************************************
			/// write the U-component to the mat-file:
			pu2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayR, (MWSIZE)settings->output.nArrayZ, mxCOMPLEX);
			if( pu2D == NULL){
				fatal("Memory alocation error.");
			}
			copyComplexToPtr2D(dP_dR2D, pu2D, settings->output.nArrayZ, settings->output.nArrayR);
			matPutVariable(matfile, "u", pu2D);
			mxDestroyArray(pu2D);


			/// **************************************
			/// write the W-component to the mat-file:
			pw2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayR, (MWSIZE)settings->output.nArrayZ, mxCOMPLEX);
			if( pw2D == NULL){
				fatal("Memory alocation error.");
			}
			copyComplexToPtr2D(dP_dZ2D, pw2D, settings->output.nArrayZ, settings->output.nArrayR);
			matPutVariable(matfile, "w", pw2D);
			mxDestroyArray(pw2D);
			break;
	}
	/*
	else{
	if (artype.ne.'RRY') then
		jj = max(nra,nza)

		do j = 1,jj
			uu(1,j) = realpart( dpdr(j) )
			uu(2,j) = imagpart( dpdr(j) )
			ww(1,j) = realpart( dpdz(j) )
			ww(2,j) = imagpart( dpdz(j) )
		end do

		puu = mxCreateDoubleMatrix(2,jj,0)
		call mxCopyReal8ToPtr(uu,mxGetPr(puu),2*jj)
		status = matPutVariable(mp,'u',puu)
		call mxDestroyArray(puu)

		pww = mxCreateDoubleMatrix(2,jj,0)
		call mxCopyReal8ToPtr(ww,mxGetPr(pww),2*jj)
		status = matPutVariable(mp,'w',pww)
		call mxDestroyArray(pww)
		
	}
	*/

	//free memory
	matClose(matfile);
	freeComplex2D(dP_dR2D, settings->output.nArrayR);
	freeComplex2D(dP_dZ2D, settings->output.nArrayR);
}
