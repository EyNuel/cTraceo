/********************************************************************************
 *	calcEigenRayPr.c		 													*
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

#include <complex.h>
#include "globals.h"
#include "tools.c"
#include "solveDynamicEq.c"
#include "solveEikonalEq.c"
#include <mat.h>
#include "matrix.h"
#include "interpolation.h"
#include "bracket.c"
#include "eBracket.c"

void	calcEigenRayPr(settings_t*);

void	calcEigenRayPr(settings_t* settings){
	DEBUG(1,"in\n");
	
	MATFile*		matfile		= NULL;
	mxArray*		pThetas		= NULL;
	mxArray*		pTitle		= NULL;
	mxArray*		pHydArrayR	= NULL;
	mxArray*		pHydArrayZ	= NULL;
	mxArray*		pRay		= NULL;
	mxArray*		pnEigenRays	= NULL;
	double			thetai, ctheta;
	double			junkDouble;
	ray_t*			ray			= NULL;
	double			nEigenRays = 0;
	uintptr_t		i, j, jj, k, l;
	double 			rHyd, zHyd, zRay, tauRay;
	complex	double	junkComplex, ampRay; 
	double			dz;
	char* 			string	= mallocChar(10);
	double**		temp2D 	= NULL;
	uintptr_t		nRet, iHyd;
	uintptr_t		iRet[51];
	
	matfile 	= matOpen("eig.mat", "w");
	pThetas		= mxCreateDoubleMatrix(1, (int32_t)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL){
		fatal("Memory alocation error.");
	}

	//copy angles in cArray to mxArray:
	copyDoubleToPtr(	settings->source.thetas,
						mxGetPr(pThetas),
						settings->source.nThetas);
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
							pDyingRay	= mxCreateDoubleMatrix(2, (int32_t)ray[i].nCoords, mxREAL);
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
								nEigenRays += 1;

								//from index interpolate the rays' travel time and amplitude:
								intLinear1D(		&ray[i].r[iHyd], &ray[i].tau[iHyd],	rHyd, &tauRay,	&junkDouble);
								intComplexLinear1D(	&ray[i].r[iHyd], &ray[i].amp[iHyd],	rHyd, &ampRay,	&junkComplex);

								///prepare to write ray to matfile:
								//create a temporary container for the files:
								temp2D 		= mallocDouble2D(5,iHyd+2);

								//copy content to the new variable:
								for (k=0; k<iHyd+1; k++){
									temp2D[0][k]=	ray[i].r[k];
									temp2D[1][k]=	ray[i].z[k];
									temp2D[2][k]=	ray[i].tau[k];
									temp2D[3][k]=	creal( ray[i].amp[k] );
									temp2D[4][k]=	cimag( ray[i].amp[k] );
								}
								
								//adjust the ray's last set of coordinates so that it matches up with the hydrophone
								temp2D[0][iHyd+1]	= rHyd;
								temp2D[1][iHyd+1]	= zRay;
								temp2D[2][iHyd+1]	= tauRay;
								temp2D[3][iHyd+1]	= creal(ampRay);
								temp2D[4][iHyd+1]	= cimag(ampRay);
								
								//(copy data to mxArray and write ray to file:
								pRay = mxCreateDoubleMatrix(5, (int32_t)(iHyd+1), mxREAL);
								if(pRay == NULL){
									fatal("Memory alocation error.");
								}
								copyDoubleToPtr2D(temp2D, mxGetPr(pRay), iHyd+1,5);
								sprintf(string, "ray%u", (uint32_t)(nEigenRays));
								matPutVariable(matfile, (const char*)string, pRay);

								//free memory:
								mxDestroyArray(pRay);
								freeDouble2D(temp2D,5);
								///ray has been written to matfile.
							}//	if (dz settings->output.miss)
						}//	for(jj=1; jj<=settings->output.nArrayZ; jj++)
						
					}else{// if (ray[i].iReturn == FALSE)

						DEBUG(3,"returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[i].nCoords, (uint32_t)iHyd);
						//get the indexes of the bracketing points.
						eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);

						//from each index interpolate the rays' depth:
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

									///prepare to write ray to matfile:
									//create a temporary container for the files:
									temp2D 		= mallocDouble2D(5,iRet[l]+1);

									//copy content to the new variable:
									//TODO this is UGLY -find a better way to do it.
									for (k=0; k<iRet[l]+1; k++){
										temp2D[0][k]=	ray[i].r[k];
										temp2D[1][k]=	ray[i].z[k];
										temp2D[2][k]=	ray[i].tau[k];
										temp2D[3][k]=	creal( ray[i].amp[k] );
										temp2D[4][k]=	cimag( ray[i].amp[k] );
									}
									
									//adjust the ray's last set of coordinates so that it matches up with the hydrophone
									temp2D[0][iRet[l]+1]	= rHyd;
									temp2D[1][iRet[l]+1]	= zRay;
									temp2D[2][iRet[l]+1]	= tauRay;
									temp2D[3][iRet[l]+1]	= creal(ampRay);
									temp2D[4][iRet[l]+1]	= cimag(ampRay);
									
									//(copy data to mxArray and write ray to file:
									pRay = mxCreateDoubleMatrix(5, (int32_t)(iRet[l]+1), mxREAL);
									if(pRay == NULL){
										fatal("Memory alocation error.");
									}
									copyDoubleToPtr2D(temp2D, mxGetPr(pRay), iRet[l]+1,5);
									sprintf(string, "ray%u", (uint32_t)(nEigenRays));
									matPutVariable(matfile, (const char*)string, pRay);
									
									//free memory:
									mxDestroyArray(pRay);
									freeDouble2D(temp2D,5);
									///ray has been written to matfile.
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
	DEBUG(2,"Number of eigenrays found: %u\n", (uint32_t)nEigenRays);
	pnEigenRays = mxCreateDoubleMatrix(1,1,mxREAL);
	copyDoubleToPtr(	&nEigenRays,
						mxGetPr(pnEigenRays),
						1);
	matPutVariable(matfile, "nerays", pnEigenRays);
	mxDestroyArray(pnEigenRays);

	matClose(matfile);
	free(string);
	DEBUG(1,"out\n");
}
