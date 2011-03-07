/********************************************************************************
 *	calcEigenRayPr.c		 													*
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

#include <complex.h>
#include "globals.h"
#include "tools.c"
#include "solveDynamicEq.c"
#include "solveEikonalEq.c"
#include <mat.h>
#include "matrix.h"
#include "interpolation.h"
#include "bracket.c"

void 	calcEigenRayRF(settings_t*);
/*
       character*60 ctitle
       character*10 eray
       character*9 cix
       character*8 cviii
       character*7 cvii
       character*6 cvi
       character*5 cv
       character*4 civ
       character*3 ciii
       character*2 cii
       character*1 ci
       
       integer*8 inan(nangle,nhyd2)
       integer*8 irefl(np),jbdry(np)
       integer*8 i,iFail,iHyd,imax,il,ir,j,jj,jjj,k,l,m
       integer*8 nFail,nEigenRays,nrays,nthtas,ntrial,nEigRays
       
       integer*8 matOpen, matClose
       integer*8 mxCreateDoubleMatrix, mxCreateString, mxGetPr
       integer*8 matfile, ptitle, pthtas, prh, pzh, pray, perays
       integer*8 matPutVariable, matDeleteVariable
       integer*8 status
       
       real*8 raydat(5,np)
       real*8  tbdry(2,np)
       
       real*8 depths(nangle,nhyd2)
       
       real*8 f(nangle),thetas[nangle)
       real*8 thetaL(np2),thetaR(np2)
       real*8 xl(2),yl(2)
       
       real*8 thetai,ctheta
       real*8 rHyd,zRay,zRay
       real*8 thtae,thtal,thtar,thta0
       real*8 fl,fr,f0,prod
       real*8 mynan,mnsone
       real*8 rerays
       
       complex*8 decay(np)
*/

void	calcEigenRayRF(settings_t* settings){
	DEBUG(1,"in\n");
	MATFile*		matfile		= NULL;
	mxArray*		pThetas		= NULL;
	mxArray*		pTitle		= NULL;
	mxArray*		pHydArrayR	= NULL;
	mxArray*		pHydArrayZ	= NULL;
	mxArray*		pnEigenRays	= NULL;
	mxArray*		pRay		= NULL;
	double*			thetas		= NULL;
	double 			thetai, ctheta;
	double**		depths		= NULL;
	uintptr_t		i, j, k, l, nRays, nEigenRays, iHyd;
	double			zRay, zHyd, rHyd;
	ray_t*			ray = NULL;
	double*			dz = NULL;
	//used for root-finding in actual Regula-Falsi Method:
	double			fl, fr, prod;
	double*			thetaL = NULL;
	double*			thetaR = NULL;
	double			junkDouble;
	uint32_t		nFail, iFail = FALSE, nTrial;
	ray_t*			tempRay = NULL;
	double			thetaE, theta0, f0;
	double**		temp2D = NULL;
	char* 			string	= mallocChar(10);
	uint32_t		success = FALSE;


	nEigenRays = 0;

	matfile 	= matOpen("eig.mat", "w");
	pThetas		= mxCreateDoubleMatrix(1, (int32_t)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL){
		fatal("Memory alocation error.");
	}

	//copy angles in cArray to mxArray:
	copyDoubleToPtr(	settings->source.thetas,
						mxGetPr(pThetas),
						settings->source.nThetas);
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
	pHydArrayR			= mxCreateDoubleMatrix(1, (int32_t)settings->output.nArrayR, mxREAL);
	if(pHydArrayR == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToPtr(	settings->output.arrayR,
						mxGetPr(pHydArrayR),
						(uintptr_t)settings->output.nArrayR);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "rarray", pHydArrayR);
	mxDestroyArray(pHydArrayR);


	//write hydrophone array depths to file:
	pHydArrayZ			= mxCreateDoubleMatrix(1, (int32_t)settings->output.nArrayZ, mxREAL);
	if(pHydArrayZ == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToPtr(	settings->output.arrayZ,
						mxGetPr(pHydArrayZ),
						(uintptr_t)settings->output.nArrayZ);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "zarray", pHydArrayZ);
	mxDestroyArray(pHydArrayZ);


	//allocate memory for the rays:
	thetas = mallocDouble(settings->source.nThetas);
	depths = mallocDouble2D(settings->source.nThetas, settings->output.nArrayR);
	ray = makeRay(settings->source.nThetas);
	
	//Solve the EIKonal and the Dynamic sets of EQuations:
	nRays = 0;
	nEigenRays = 0;
	
	for(i=0; i<settings->source.nThetas; i++){
		DEBUG(3, "--\n\t\tRay Launching angle: %lf\n", settings->source.thetas[i]);
		thetai = -settings->source.thetas[i]*M_PI/180.0;
		ray[i].theta = thetai;
		ctheta = fabs( cos( thetai ) );

		//	Trace a ray as long as it is neither 90 nor -90:
		if (ctheta > 1.0e-7){
			thetas[nRays] = thetai;
			DEBUG(3, "thetas[%u]: %e\n", (uint32_t)nRays, thetas[nRays]);
			nRays++;
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);
			
			if (ray[i].iReturn == TRUE){
				printf("Eigenray search by Regula Falsi detected a returning ray at angle %lf.\n", thetas[i]);
				fatal("Returning eigenrays can only be determined by Proximity.\nAborting");
			}
			
			//Got the ray? then fill the matrix of depths: 
			for(j=0; j<settings->output.nArrayR; j++){
				rHyd = settings->output.arrayR[j];
				
				//check if the hydrophone range coord is whithin range of the ray
				if ( (rHyd >= ray[i].rMin) && (rHyd <= ray[i].rMax)){
					
					//find bracketing coords:
					bracket( ray[i].nCoords, ray[i].r, rHyd, &iHyd);
					
					//interpolate the ray depth at the range coord of hydrophone
					intLinear1D(&ray[i].r[iHyd], &ray[i].z[iHyd], rHyd, &zRay, &junkDouble);
					depths[i][j] = zRay;
					DEBUG(3,"rHyd: %lf; rMin: %lf; rMax: %lf\n", rHyd, ray[i].rMin, ray[i].rMax);
					DEBUG(3,"nCoords: %u, rHyd: %lf; iHyd: %u, zRay: %lf\n", (uint32_t)ray[i].nCoords, rHyd, (uint32_t)iHyd, zRay);
				}else{
					depths[i][j] = NAN;
				}
			}
			reallocRayMembers(&ray[i],0);
		}
	}
	free(ray);
	
	/********************************************************************************
	 *	Angle loop finished? proceed searching eigenrays at each point of the array
	 */

	//allocate memmory for some temporary variables
	dz = mallocDouble(nRays);
	thetaL = mallocDouble(nRays);
	thetaR = mallocDouble(nRays);

	//	iterate over....
	for (i=0; i<settings->output.nArrayR; i++){
		rHyd = settings->output.arrayR[i];
		//	...the entire array:
		for(j=0; j<settings->output.nArrayZ; j++){
			zHyd = settings->output.arrayZ[j];
			DEBUG(3, "i: %u; j: %u; rHyd:%lf, zHyd:%lf\n",(uint32_t)i, (uint32_t)j, rHyd, zHyd );
			//At each hydrophone calculate the difference between the hydrophone and ray depths:
			for(k=0; k<nRays; k++){
				dz[k] = zHyd - depths[k][i];
				DEBUG(3,"dz[%u]= %lf\n", (uint32_t)k, dz[k]);
			}
			
			// Determine the number of eigenrays by looking at sign variations (or zero values) of the difference:
			nEigenRays = 0;
			for(k=0; k<nRays-1; k++){
				fl = dz[k];
				fr = dz[k+1];
				prod = fl*fr;
				
				if(	isnan((float)depths[k][i]) == FALSE	&&
					isnan((float)depths[k+1][i]) == FALSE	){
					DEBUG(3, "Not a NAN\n");
					
					if(	(fl == 0.0) && (fr != 0.0)){
						thetaL[nEigenRays] = thetas[k];
						thetaR[nEigenRays] = thetas[k+1];
						nEigenRays++;
					
					}else if(	(fr == 0.0) && (fl != 0.0)){
						thetaL[nEigenRays] = thetas[k];
						thetaR[nEigenRays] = thetas[k+1];
						nEigenRays++;
					
					}else if(prod < 0.0){
						thetaL[nEigenRays] = thetas[k];
						thetaR[nEigenRays] = thetas[k+1];
						nEigenRays++;
					
					}
					DEBUG(3, "thetaL: %e, thetaR: %e\n", thetaL[nEigenRays], thetaR[nEigenRays]);
				}
				DEBUG(3, "nEigenRays: %u\n", (uint32_t)nEigenRays);
				if (nEigenRays > nRays){
					//this should not be possible. TODO replace by assertion?
					fatal("Number of eigenrays exceeds number of calculated rays.\nAborting.");
				}
			}
		
			//Time to find eigenrays; either we are lucky or we need to apply regula falsi:
			nFail = 0;
DEBUG(3,"1\n");

tempRay = makeRay(1);
DEBUG(3,"2\n");
			//If nEigenRays = 0 this loop will not do anything:
			DEBUG(3, "nEigenRays: %u\n", (uint32_t)nEigenRays);
			for(l=0; l<nEigenRays; l++){
				settings->source.rbox2 = rHyd;
				
				DEBUG(3,"3; l: %u\n", (uint32_t)l);
				//Determine "left" ray's depth at rHyd:
				tempRay->theta = thetaL[l];
				solveEikonalEq(settings, tempRay);
				fl = tempRay->z[tempRay->nCoords-1] - zHyd;
				//reset the ray members to zero:
				reallocRayMembers(tempRay, 0);	

				DEBUG(3,"4\n");
				//Determine "right" ray's depth at rHyd:
				tempRay->theta = thetaR[l];
				solveEikonalEq(settings, tempRay);
				fr = tempRay->z[tempRay->nCoords-1] - zHyd;
				//reset the ray members to zero:
				reallocRayMembers(tempRay, 0);	
				DEBUG(3,"5\n");
				if (fabs(fl) <= settings->output.miss){
					thetaE = thetaL[l];
					iFail = FALSE;
				
				}else if (fabs(fr) <= settings->output.miss){
					thetaE = thetaR[l];
					iFail = FALSE;
				
				}else{
					nTrial = 0;
					//here comes the actual Regula-Falsi loop:
					while(success == FALSE){
						theta0 = thetaR[l] - fr*( thetaL[l] - thetaR[l] )/( fl - fr );
						DEBUG(3, "l: %u; thetaR[l]: %e; thetaL[l]: %e; theta0: %e; fl: %e; fr: %e;\n",
								(uint32_t)l, thetaR[l], thetaL[l], 			theta0,		fl,		fr);
						if (nTrial > 21){
							iFail = TRUE;
							printf("(rHyd,zHyd)= %e, %e\n", rHyd, zHyd);
							printf("Eigenray search failure, skipping to next case...\n");
						}
						
						tempRay->theta = theta0;
						solveEikonalEq(settings, tempRay);
						f0 = tempRay->z[tempRay->nCoords-1] - zHyd;
						//reset the ray members to zero:
						DEBUG(3, "nCoords: %u\n", (uint32_t)tempRay->nCoords);
						reallocRayMembers(tempRay, 0);
						DEBUG(3, "zHyd: %e; miss: %e, nTrial: %u, f0: %e\n", zHyd, settings->output.miss, (uint32_t)nTrial, f0);
						
						if (fabs(f0) < settings->output.miss){
							iFail = FALSE;
							success = TRUE;
							break;
						
						}else if (iFail == FALSE ){
							DEBUG(3, "ASD\n");
							prod = fl*f0;
							nTrial++;
							if ( prod < 0.0 ){
								DEBUG(3, "ASD1\n");
								thetaR[l] = theta0;
								fr = f0;
							}else{
								DEBUG(3, "ASD2\n");
								thetaL[l] = theta0;
								fl = f0;
							}
							success = FALSE;
						}
					}//while()
					success = FALSE;
					DEBUG(3, "iFail: %u\n", iFail);
				}
				DEBUG(3,"iFail: %u\n", (uint32_t)iFail);
				if (iFail == FALSE){
					nEigenRays++;

					tempRay->theta = theta0;
					solveEikonalEq(settings, tempRay);
					solveDynamicEq(settings, tempRay);

					///prepare to write ray to matfile:
					temp2D 		= malloc(5*sizeof(uintptr_t));
					temp2D[0]	= tempRay->r;
					temp2D[1]	= tempRay->z;
					temp2D[2]	= tempRay->tau;
					temp2D[3]	= mallocDouble(tempRay->nCoords);
					temp2D[4]	= mallocDouble(tempRay->nCoords);
					for(k=0; k<tempRay->nCoords; k++){
						temp2D[3][k] = creal( tempRay->amp[k] );
						temp2D[4][k] = cimag( tempRay->amp[k] );
					}

					//copy data to mxArray and write ray to file:
					pRay = mxCreateDoubleMatrix(5, (int32_t)tempRay->nCoords, mxREAL);
					if(pRay == NULL){
						fatal("Memory alocation error.");
					}
					copyDoubleToPtr2D(temp2D, mxGetPr(pRay), tempRay->nCoords,5);

					sprintf(string, "ray%u", (uint32_t)(nEigenRays+1));
					matPutVariable(matfile, (const char*)string, pRay);

					//reset the ray members to zero:
					reallocRayMembers(tempRay, 0);
					//free memory:
					mxDestroyArray(pRay);
					free(temp2D[3]);
					free(temp2D[4]);
					free(temp2D);
					///ray has been written to matfile
				}
			}
		}
	}

	///Write number of eigenrays to matfile:
	pnEigenRays = mxCreateDoubleMatrix(1,1,mxREAL);
	junkDouble = (double)nEigenRays;
	copyDoubleToPtr(	&junkDouble,
						mxGetPr(pnEigenRays),
						1);
	matPutVariable(matfile, "nerays", pnEigenRays);
	mxDestroyArray(pnEigenRays);

	//Free memory
	matClose(matfile);
	free(string);
	free(dz);
	DEBUG(1,"out\n");
}







