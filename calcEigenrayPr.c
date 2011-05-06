/********************************************************************************
 *	calcEigenrayPr.c	 														*
 * 	(formerly "calepr.for")														*
 *	Calculates eigenrays using proximity method.								*
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
 * 				none:		Writes to file "eig.mat".							*
 * 																				*
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
#include "eBracket.c"

void	calcEigenrayPr(settings_t*);

void	calcEigenrayPr(settings_t* settings){
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
	
	double			thetai, ctheta;
	double			junkDouble;
	ray_t*			ray			= NULL;
	double			nEigenRays = 0;
	uintptr_t		i, j, jj, l;
	double 			rHyd, zHyd, zRay, tauRay;
	complex	double	junkComplex, ampRay; 
	double			dz;
	uintptr_t		nRet, iHyd;
	uintptr_t		iRet[51];
	
	
	//open matfile for output
	matfile 	= matOpen("eig.mat", "w");
	
	
	//write launching angles to file
	pThetas		= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL){
		fatal("Memory alocation error.");
	}
	//copy angles in cArray to mxArray:
	copyDoubleToMxArray(settings->source.thetas, pThetas, settings->source.nThetas);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "thetas", pThetas);
	mxDestroyArray(pThetas);
	
	
	//write title to matfile:
	pTitle = mxCreateString("TRACEO: EIGenrays (by proximity)");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);
	
	
	//write hydrophone array ranges to file:
	pHydArrayR			= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
	if(pHydArrayR == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToMxArray(	settings->output.arrayR, pHydArrayR, (uintptr_t)settings->output.nArrayR);
	matPutVariable(matfile, "rarray", pHydArrayR);
	mxDestroyArray(pHydArrayR);
	
	
	//write hydrophone array depths to file:
	pHydArrayZ			= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
	if(pHydArrayZ == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToMxArray(	settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
	matPutVariable(matfile, "zarray", pHydArrayZ);
	mxDestroyArray(pHydArrayZ);
	
	
	//create mxStructArray:
	mxRayStruct = mxCreateStructMatrix(	(MWSIZE)settings->source.nThetas,	//number of rows
										(MWSIZE)1,		//number of columns
										5,				//number of fields in each element
										fieldNames);	//list of field names
	if( mxRayStruct == NULL ) {
		fatal("Memory Alocation error.");
	}
	
	
	//allocate memory for the rays:
	ray = makeRay(settings->source.nThetas);
	
	
	/** Trace the rays:  */
	for(i=0; i<settings->source.nThetas; i++){
		thetai = -settings->source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		ctheta = fabs( cos(thetai));
		
		//Trace a ray as long as it is neither at 90 nor -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);
			
			//test for proximity of ray to each hydrophone 
			//(yes, this is slow, can you figure out a better way to do it?)
			for(j=0; j<settings->output.nArrayR; j++){
				rHyd = settings->output.arrayR[j];
				
				if ( (rHyd >= ray[i].rMin) && (rHyd <= ray[i].rMax)){
					
					//	Check if the ray is returning back or not;
					//	if not:		we can bracket it without problems,
					//	otherwise:	we need to know how many times it passed by the given array range 
					if (ray[i].iReturn == FALSE){
						
						/*
						//when debugging, save the coordinates of the last ray to a separate matfile before exiting.
						#if VERBOSE
						if((int)i==(int)4){
							DEBUG(1,"i:%u, nCoords: %u\n", (uint32_t)i, (uint32_t)ray[i].nCoords);
							DEBUG(1,"rMin: %e, rMax: %e\n",ray[i].rMin, ray[i].rMax);
							
							mxArray*	pDyingRay	= NULL;
							MATFile*	matfile2	= NULL;
							double**	dyingRay 	= malloc(2*sizeof(uintptr_t));
							char* 		string2		= mallocChar(10);
							
							dyingRay[0]	= ray[i].r;
							dyingRay[1]	= ray[i].z;
							matfile2	= matOpen("dyingRay.mat", "w");
							pDyingRay	= mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)ray[i].nCoords, mxREAL);
							if(pDyingRay == NULL || matfile2 == NULL)
								fatal("Memory alocation error.");
							copyDoubleToPtr2D(dyingRay, mxGetPr(pDyingRay), ray[i].nCoords,2);
							sprintf(string2, "dyingRay");
							matPutVariable(matfile2, (const char*)string2, pDyingRay);

							mxDestroyArray(pDyingRay);
							matClose(matfile2);
							DEBUG(1,"Dying ray written to file.\n");
						}
						#endif
						*/
						
						//get the index of the lower bracketing element:
						bracket(ray[i].nCoords,	ray[i].r, rHyd, &iHyd);
						DEBUG(3,"non-returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[i].nCoords, (uint32_t)iHyd);
						
						//from index interpolate the rays' depth:
						intLinear1D(		&ray[i].r[iHyd], &ray[i].z[iHyd],	rHyd, &zRay,	&junkDouble);
						
						//for every hydrophone check distance to ray
						for(jj=0; jj<settings->output.nArrayZ; jj++){
							zHyd = settings->output.arrayZ[jj];
							dz = fabs(zRay-zHyd);
							DEBUG(4, "dz: %e\n", dz);
							
							if (dz < settings->output.miss){
								DEBUG(3, "Eigenray found\n");
								
								
								//from index interpolate the rays' travel time and amplitude:
								intLinear1D(		&ray[i].r[iHyd], &ray[i].tau[iHyd],	rHyd, &tauRay,	&junkDouble);
								intComplexLinear1D(	&ray[i].r[iHyd], &ray[i].amp[iHyd],	rHyd, &ampRay,	&junkComplex);
								
								//adjust the ray's last set of coordinates so that it matches up with the hydrophone
								ray[i].r[iHyd+1]	= rHyd;
								ray[i].z[iHyd+1]	= zRay;
								ray[i].tau[iHyd+1]	= tauRay;
								ray[i].amp[iHyd+1]	= ampRay;
								
								///prepare to write ray to matfile:
								//create mxArrays:
								mxTheta	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)1,			mxREAL);
								mxR		= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxREAL);
								mxZ		= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxREAL);
								mxTau	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxREAL);
								mxAmp	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxCOMPLEX);
								if(	mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
									fatal("Memory alocation error.");
								}
								
								//copy data to mxArrays:
								copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
								copyDoubleToMxArray(ray[i].r,					mxR,	iHyd+1);
								copyDoubleToMxArray(ray[i].z,					mxZ, 	iHyd+1);
								copyDoubleToMxArray(ray[i].tau,					mxTau,	iHyd+1);
								copyComplexToMxArray(ray[i].amp,				mxAmp,	iHyd+1);
								
								//copy mxArrays to mxRayStruct
								mxSetFieldByNumber(	mxRayStruct,		//pointer to the mxStruct
													(MWINDEX)nEigenRays,//index of the element (number of ray)
													0,					//position of the field (in this case, field 0 is "r"
													mxTheta);			//the mxArray we want to copy into the mxStruct
								mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 1, mxR);
								mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 2, mxZ);
								mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 3, mxTau);
								mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 4, mxAmp);
								///ray has been saved to mxStructArray
								
								nEigenRays += 1;
							}//	if (dz settings->output.miss)
						}//	for(jj=1; jj<=settings->output.nArrayZ; jj++)
						
					}else{// if (ray[i].iReturn == FALSE)

						DEBUG(3,"returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[i].nCoords, (uint32_t)iHyd);
						//get the indexes of the bracketing points.
						eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);

						//for each index where the ray passes at the hydrophone, interpolate the rays' depth:
						for(l=0; l<nRet; l++){
							DEBUG(4, "nRet=%u, iRet[%u]= %u\n", (uint32_t)nRet, (uint32_t)l, (uint32_t)iRet[l]);
							intLinear1D(		&ray[i].r[iRet[l]], &ray[i].z[iRet[l]],		rHyd, &zRay,	&junkDouble);

							//for every hydrophone check if the ray is close enough to be considered an eigenray:
							for(jj=0;jj<settings->output.nArrayZ; jj++){
								zHyd = settings->output.arrayZ[jj];
								dz = fabs( zRay - zHyd );
								
								if (dz < settings->output.miss){
									nEigenRays += 1;

									//interpolate the ray's travel time and amplitude:
									intLinear1D(		&ray[i].r[iRet[l]], &ray[i].tau[iRet[l]],	rHyd, &tauRay,	&junkDouble);
									intComplexLinear1D(	&ray[i].r[iRet[l]], &ray[i].amp[iRet[l]],	(complex double)rHyd, &ampRay,	&junkComplex);
									
									
									//adjust the ray's last set of coordinates so that it matches up with the hydrophone
									ray[i].r[iHyd+1]	= rHyd;
									ray[i].z[iHyd+1]	= zRay;
									ray[i].tau[iHyd+1]	= tauRay;
									ray[i].amp[iHyd+1]	= ampRay;
									
									///prepare to write ray to matfile:
									//create mxArrays:
									mxTheta	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)1,			mxREAL);
									mxR		= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxREAL);
									mxZ		= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxREAL);
									mxTau	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxREAL);
									mxAmp	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)(iHyd+1),	mxCOMPLEX);
									if(	mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
										fatal("Memory alocation error.");
									}
									
									//copy data to mxArrays:
									copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
									copyDoubleToMxArray(ray[i].r,					mxR,	iHyd+1);
									copyDoubleToMxArray(ray[i].z,					mxZ, 	iHyd+1);
									copyDoubleToMxArray(ray[i].tau,					mxTau,	iHyd+1);
									copyComplexToMxArray(ray[i].amp,				mxAmp,	iHyd+1);
									
									//copy mxArrays to mxRayStruct
									mxSetFieldByNumber(	mxRayStruct,		//pointer to the mxStruct
														(MWINDEX)nEigenRays,//index of the element (number of ray)
														0,					//position of the field (in this case, field 0 is "r"
														mxTheta);			//the mxArray we want to copy into the mxStruct
									mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 1, mxR);
									mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 2, mxZ);
									mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 3, mxTau);
									mxSetFieldByNumber(	mxRayStruct, (MWINDEX)nEigenRays, 4, mxAmp);
									///ray has been saved to mxStructArray
									
									nEigenRays += 1;
								}
							}
						}
					}//	if (ray[i].iReturn == FALSE)
				}//if ( (rHyd >= ray[i].rMin) && (rHyd < ray[i].rMax))
			}//for(j=0; j<settings->output.nArrayR; j++){
			if(KEEP_RAYS_IN_MEM == FALSE){
				//free the ray's memory
				reallocRayMembers(&ray[i],0);
			}
		}//if (ctheta > 1.0e-7)
	}//for(i=0; i<settings->source.nThetas; i++)
	
	///Write number of eigenrays to matfile:
	DEBUG(1,"Number of eigenrays found: %u\n", (uint32_t)nEigenRays);
	pnEigenRays = mxCreateDoubleMatrix((MWSIZE)1,(MWSIZE)1,mxREAL);
	copyDoubleToMxArray( &nEigenRays, pnEigenRays, 1);
	matPutVariable(matfile, "nerays", pnEigenRays);
	mxDestroyArray(pnEigenRays);
	
	///Write Eigenrays to matfile:
	matPutVariable(matfile, "rays", mxRayStruct);
	
	matClose(matfile);
	mxDestroyArray(mxRayStruct);
	
	reallocRayMembers(ray, 0);
	free(ray);
	DEBUG(1,"out\n");
}
