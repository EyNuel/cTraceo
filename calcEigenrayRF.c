/********************************************************************************
 *	calcEigenrayPr.c		 													*
 * 	(formerly "calerf.for")														*
 *	Calculates eigenrays using Regula Falsi method.								*
 *	Note that this will only work with rays traveling from left to right and	*
 *	for cases where there are no returning rays.								*
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
 *				settings:	Pointer to structure containing all input info.		*
 *	Outputs:																	*
 *				none:		Writes to file "eig.mat".							*
 *																				*
 *******************************************************************************/

#pragma  once
#include <complex.h>
#include "globals.h"
#include "tools.h"
#include "solveDynamicEq.c"
#include "solveEikonalEq.c"
#include <mat.h>
#include "matrix.h"
#include "interpolation.h"
#include "bracket.c"

void 	calcEigenrayRF(settings_t*);

void	calcEigenrayRF(settings_t* settings){
	DEBUG(1,"in\n");
	MATFile*		matfile		= NULL;
	mxArray*		pThetas		= NULL;
	mxArray*		pTitle		= NULL;
	mxArray*		pHydArrayR	= NULL;
	mxArray*		pHydArrayZ	= NULL;
	mxArray*		pnEigenRays	= NULL;
	mxArray*		mxTheta		= NULL;
	mxArray*		mxR			= NULL;
	mxArray*		mxZ			= NULL;
	mxArray*		mxTau		= NULL;
	mxArray*		mxAmp		= NULL;
	mxArray*		mxRayStruct	= NULL;
	const char*		fieldNames[]= {	"theta",
									"r",
									"z",
									"tau",
									"amp"};		//the names of the fields contained in mxRayStruct
	
	double*			thetas		= NULL;
	double 			thetai, ctheta;
	double**		depths		= NULL;
	uintptr_t		i, j, k, l, nRays, iHyd;
	uintptr_t		nPossibleEigenRays, nFoundEigenRays;
	double			zRay, zHyd, rHyd;
	ray_t*			ray = NULL;
	double*			dz = NULL;
	//used for root-finding in actual Regula-Falsi Method:
	double			fl, fr, prod;
	double*			thetaL = NULL;
	double*			thetaR = NULL;
	double			junkDouble;
	uint32_t		nTrial;
	ray_t*			tempRay = NULL;
	double			theta0, f0;
	uint32_t		success = FALSE;
	
	
	//Open matfile for output:
	matfile 	= matOpen("eig.mat", "w");
	
	
	//write ray launching angles to matfile:
	pThetas		= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL){
		fatal("Memory alocation error.");
	}
	//copy angles in cArray to mxArray:
	copyDoubleToMxArray(	settings->source.thetas, pThetas , settings->source.nThetas);
	matPutVariable(matfile, "thetas", pThetas);
	mxDestroyArray(pThetas);
	
	
	//write title to matfile:
	pTitle = mxCreateString("TRACEO: EIGenrays (by Regula Falsi)");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);
	
	
	//write hydrophone array ranges to file:
	pHydArrayR = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
	if(pHydArrayR == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToMxArray( settings->output.arrayR, pHydArrayR, (uintptr_t)settings->output.nArrayR);
	matPutVariable(matfile, "rarray", pHydArrayR);
	mxDestroyArray(pHydArrayR);
	
	
	//write hydrophone array depths to file:
	pHydArrayZ			= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
	if(pHydArrayZ == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToMxArray( settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
	matPutVariable(matfile, "zarray", pHydArrayZ);
	mxDestroyArray(pHydArrayZ);
	
	
	//allocate memory for rays and auxiliary variables:
	ray = makeRay(settings->source.nThetas);
	
	/**********************************************************************************************
	 * 	1)	Create a set of arrays (thetas[], depths[][]) that relate the launching angles of the 
	 *		rays with their depth at each of the hydrophone array's depths:
	 */
	thetas = mallocDouble(settings->source.nThetas);
	depths = mallocDouble2D(settings->source.nThetas, settings->output.nArrayR);
	DEBUG(2,"Calculting preliminary rays:\n");
	nRays = 0;
	
	for(i=0; i<settings->source.nThetas; i++){
		DEBUG(3, "--\n\t\tRay Launching angle: %lf\n", settings->source.thetas[i]);
		thetai = -settings->source.thetas[i]*M_PI/180.0;
		ray[i].theta = thetai;
		ctheta = fabs( cos( thetai ) );

		//	Trace a ray as long as it is neither 90 nor -90:
		if (ctheta > 1.0e-7){
			thetas[nRays] = thetai;
			DEBUG(3, "thetas[%u]: %e\n", (uint32_t)nRays, thetas[nRays]);
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);
			
			if (ray[i].iReturn == TRUE){
				printf("Eigenray search by Regula Falsi detected a returning ray at angle %lf.\n", thetas[i]);
				fatal("Returning eigenrays can only be determined by Proximity.\nAborting");
			}
			
			//Ray calculted; now fill the matrix of depths: 
			for(j=0; j<settings->output.nArrayR; j++){
				rHyd = settings->output.arrayR[j];
				
				//check if the hydrophone range coord is whithin range of the ray
				if ( (rHyd >= ray[i].rMin) && (rHyd <= ray[i].rMax)){
					
					//find bracketing coords:
					bracket( ray[i].nCoords, ray[i].r, rHyd, &iHyd);
					
					//interpolate the ray depth at the range coord of hydrophone
					intLinear1D(&ray[i].r[iHyd], &ray[i].z[iHyd], rHyd, &zRay, &junkDouble);
					depths[nRays][j] = zRay;
					DEBUG(3,"rHyd: %lf; rMin: %lf; rMax: %lf\n", rHyd, ray[i].rMin, ray[i].rMax);
					DEBUG(3,"nCoords: %u, rHyd: %lf; iHyd: %u, zRay: %lf\n", (uint32_t)ray[i].nCoords, rHyd, (uint32_t)iHyd, zRay);
				}else{
					depths[nRays][j] = NAN;
				}
			}
			reallocRayMembers(&ray[i],0);
			nRays++;
		}
	}
	free(ray);
	/**	1)	Done.
	 */
	DEBUG(3, "Preliminary rays calculated.\n");
	
	/********************************************************************************
	 *	2)	Proceed to searching for possible eigenrays at each point of the array:
	 */

	//allocate memory for some temporary variables
	dz =		mallocDouble(nRays);
	thetaL =	mallocDouble(nRays);
	thetaR =	mallocDouble(nRays);

	//	iterate over....
	for (i=0; i<settings->output.nArrayR; i++){
		rHyd = settings->output.arrayR[i];
		//	...the entire array:
		for(j=0; j<settings->output.nArrayZ; j++){
			zHyd = settings->output.arrayZ[j];
			DEBUG(3, "i: %u; j: %u; rHyd:%lf, zHyd:%lf\n",(uint32_t)i, (uint32_t)j, rHyd, zHyd );
			
			//for each ray calculate the difference between the hydrophone and ray depths:
			for(k=0; k<nRays; k++){
				dz[k] = zHyd - depths[k][i];
				DEBUG(3,"dz[%u]= %lf\n", (uint32_t)k, dz[k]);
			}
			
			/** By looking at sign variations (or zero values) of dz[]:
			 *		:: determine the number of possible eigenrays 
			 * 		:: find the launching angles of adjacent rays that pass above and below (named L and R) a hydrophone
			 * 			(which implies that there may be an intermediate launching angle that corresponds to an eigenray.
			 */
			nPossibleEigenRays = 0;
			for(k=0; k<nRays-1; k++){
				fl = dz[k];
				fr = dz[k+1];
				prod = fl*fr;
				DEBUG(3, "k: %u; thetaL: %e; thetaR: %e\n", (uint32_t)k, thetaL[k], thetaR[k]);
				
				if(	isnan_d(depths[k][i]) == FALSE	&&
					isnan_d(depths[k+1][i]) == FALSE	){
					DEBUG(3, "Not a NAN\n");
					
					if(	(fl == 0.0) && (fr != 0.0)){
						thetaL[nPossibleEigenRays] = thetas[k];
						thetaR[nPossibleEigenRays] = thetas[k+1];
						nPossibleEigenRays++;
					
					}else if(	(fr == 0.0) && (fl != 0.0)){
						thetaL[nPossibleEigenRays] = thetas[k];
						thetaR[nPossibleEigenRays] = thetas[k+1];
						nPossibleEigenRays++;
					
					}else if(prod < 0.0){
						thetaL[nPossibleEigenRays] = thetas[k];
						thetaR[nPossibleEigenRays] = thetas[k+1];
						nPossibleEigenRays++;
					
					}
					DEBUG(3, "thetaL: %e, thetaR: %e\n", thetaL[nPossibleEigenRays-1], thetaR[nPossibleEigenRays-1]);
				}else{
					DEBUG(4, "Its a NAN\n");
				}
				if (nPossibleEigenRays > nRays){
					//this should not be possible. TODO replace by assertion?
					fatal("The impossible happened.\nNumber of possible eigenrays exceeds number of calculated rays.\nAborting.");
				}
			}
			
			//Time to find eigenrays; either we are lucky or we need to apply regula falsi:
			/** We now know how many possible eigenrays this hydrophone has (nPossibleEigenRays),
			 *	and for each of them we have the bracketing launching angles.
			 *	It is now time to determine the "exact" launching angle of each eigenray.
			 */
			DEBUG(3, "nPossibleEigenRays: %u\n", (uint32_t)nPossibleEigenRays);
			
			//create mxStructArray:
			mxRayStruct = mxCreateStructMatrix(	(MWSIZE)nPossibleEigenRays,	//number of rows
												(MWSIZE)1,					//number of columns
												5,							//number of fields in each element
												fieldNames);				//list of field names
			if( mxRayStruct == NULL ) {
				fatal("Memory Alocation error.");
			}
			
			tempRay = makeRay(1);
			nFoundEigenRays = 0;
			for(l=0; l<nPossibleEigenRays; l++){		//Note that if nPossibleEigenRays = 0 this loop will not be executed:
				settings->source.rbox2 = rHyd;
				DEBUG(3,"l: %u\n", (uint32_t)l);
				
				//Determine "left" ray's depth at rHyd:
				tempRay[0].theta = thetaL[l];
				solveEikonalEq(settings, tempRay);
				fl = tempRay[0].z[tempRay[0].nCoords-1] - zHyd;
				//reset the ray members to zero:
				reallocRayMembers(tempRay, 0);	
				
				//Determine "right" ray's depth at rHyd:
				tempRay[0].theta = thetaR[l];
				solveEikonalEq(settings, tempRay);
				fr = tempRay[0].z[tempRay[0].nCoords-1] - zHyd;
				//reset the ray members to zero:
				reallocRayMembers(tempRay, 0);	

				//check if either the "left" or "right" ray pass at a distance within the defined threshold
				if (fabs(fl) <= settings->output.miss){
					DEBUG(3, "\"left\" is eigenray.\n");
					theta0 = thetaL[l];
					nFoundEigenRays++;
					success = TRUE;
				
				}else if (fabs(fr) <= settings->output.miss){
					DEBUG(3, "\"right\" is eigenray.\n");
					theta0 = thetaR[l];
					nFoundEigenRays++;
					success = TRUE;

				//if not, try to find the "exact" launching angle
				}else{
					DEBUG(3, "Neither \"left\" nor \"right\" ray are close enough to be eigenrays.\nApplying Regula-Falsi...\n");
					nTrial = 0;
					success = FALSE;
					
					//here comes the actual Regula-Falsi loop:
					while(success == FALSE){
						nTrial++;
						
						if (nTrial > 21){
							printf("(rHyd,zHyd)= %e, %e\n", rHyd, zHyd);
							printf("Eigenray search failure, skipping to next case...\n");
							//iFail = TRUE;
							break;
						}
						
						theta0 = thetaR[l] - fr*( thetaL[l] - thetaR[l] )/( fl - fr );
						DEBUG(3, "l: %u; thetaR[l]: %e; thetaL[l]: %e; theta0: %e; fl: %e; fr: %e;\n",
								(uint32_t)l, thetaR[l], thetaL[l], 			theta0,		fl,		fr);
						
						//find the distance between the new ray and the hydrophone:
						tempRay[0].theta = theta0;
						solveEikonalEq(settings, tempRay);
						f0 = tempRay[0].z[tempRay[0].nCoords-1] - zHyd;
						//reset the ray members to zero:
						DEBUG(3, "nCoords: %u\n", (uint32_t)tempRay[0].nCoords);
						reallocRayMembers(tempRay, 0);
						DEBUG(3, "zHyd: %e; miss: %e, nTrial: %u, f0: %e\n", zHyd, settings->output.miss, (uint32_t)nTrial, f0);
						
						//check if the new rays is close enough to the hydrophone to be considered and eigenray:
						if (fabs(f0) < settings->output.miss){
//							iFail = FALSE
							DEBUG(3, "Found eigenray by applying Regula-Falsi.\n");
							success = TRUE;
							nFoundEigenRays++;
							break;
						
						//if the root wasn't found, do another Regula-Falsi iterarion:
						}else{
							DEBUG(3, "ASD\n");
							prod = fl*f0;
							
							if ( prod < 0.0 ){
								DEBUG(3, "ASD1\n");
								thetaR[l] = theta0;
								fr = f0;
							}else{
								DEBUG(3, "ASD2\n");
								thetaL[l] = theta0;
								fl = f0;
							}
						}
					}//while()
					//DEBUG(3, "iFail: %u\n", iFail);
				}
				//DEBUG(3,"iFail: %u\n", (uint32_t)iFail);
				if (success == TRUE){
					
					//finally: get the coordinates and amplitudes of the eigenray
					tempRay[0].theta = theta0;
					solveEikonalEq(settings, tempRay);
					solveDynamicEq(settings, tempRay);
					
					
					///prepare to save ray to mxStructArray:
					//create mxArrays:
					mxTheta	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)1,					mxREAL);
					mxR		= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(tempRay->nCoords),	mxREAL);
					mxZ		= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(tempRay->nCoords),	mxREAL);
					mxTau	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(tempRay->nCoords),	mxREAL);
					mxAmp	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(tempRay->nCoords),	mxCOMPLEX);
					if(	mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
						fatal("Memory alocation error.");
					}
					
					//copy data to mxArrays:
					copyDoubleToMxArray(&tempRay[0].theta,	mxTheta,1);
					copyDoubleToMxArray(tempRay->r,			mxR,	tempRay->nCoords);
					copyDoubleToMxArray(tempRay->z,			mxZ, 	tempRay->nCoords);
					copyDoubleToMxArray(tempRay->tau,		mxTau,	tempRay->nCoords);
					copyComplexToMxArray(tempRay->amp,		mxAmp,	tempRay->nCoords);
					
					//copy mxArrays to mxRayStruct
					mxSetFieldByNumber(	mxRayStruct,				//pointer to the mxStruct
										(MWINDEX)(nFoundEigenRays-1),	//index of the element (number of ray)
										0,							//position of the field (in this case, field 0 is "r"
										mxTheta);					//the mxArray we want to copy into the mxStruct
					mxSetFieldByNumber(	mxRayStruct, (MWINDEX)(nFoundEigenRays-1), 1, mxR);
					mxSetFieldByNumber(	mxRayStruct, (MWINDEX)(nFoundEigenRays-1), 2, mxZ);
					mxSetFieldByNumber(	mxRayStruct, (MWINDEX)(nFoundEigenRays-1), 3, mxTau);
					mxSetFieldByNumber(	mxRayStruct, (MWINDEX)(nFoundEigenRays-1), 4, mxAmp);
					///ray has been saved to mxStructArray
				}
			}
			DEBUG(3, "nFoundEigenRays: %u\n", (uint32_t)nFoundEigenRays);
		}
	}
	
	///Write number of eigenrays to matfile:
	pnEigenRays = mxCreateDoubleMatrix((MWSIZE)1,(MWSIZE)1,mxREAL);
	junkDouble = (double)nFoundEigenRays;
	copyDoubleToMxArray( &junkDouble, pnEigenRays, 1);
	matPutVariable(matfile, "nerays", pnEigenRays);
	mxDestroyArray(pnEigenRays);
	DEBUG(3, "nFoundEigenRays: %u\n", (uint32_t)nFoundEigenRays);
	
	///Write Eigenrays to matfile:
	matPutVariable(matfile, "rays", mxRayStruct);
	
	//Free memory
	matClose(matfile);
	mxDestroyArray(mxRayStruct);
	free(dz);
	DEBUG(1,"out\n");
}







