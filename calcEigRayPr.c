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
 * 				n:		number of elements in vector x							*
 * 				x:		vector to be searched									*
 * 				xi:		scalar who's bracketing elements are to be found		*
 * 	Outputs:																	*
 * 				nb:		number of bracketing pairs found.						*
 *				ib:		the indexes of the lower bracketing elements.			*
 * 						Note: shall be previously allocated.					*
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


	//write hydrophone array ranges to file:
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
		DEBUG(2,"ray[%u].theta: %lf\n", (uint32_t)i, settings->source.thetas[i]);
		ctheta = fabs( cos(thetai));
		
		//Trace a ray as long as it is neither 90 or -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);

			//test for proximity of ray to each hydrophone 
			//(yes, this is slow, can you figure out a better way to do it?)
			for(j=0; j<settings->output.nArrayR; j++){
				rHyd = settings->output.arrayR[j];

				if ( (rHyd >= ray[i].rMin) && (rHyd < ray[i].rMax)){

					//	Check if the ray is returning back or not;
					//	if not:		we can bracket it without problems,
					//	otherwise:	we need to know how many times it passed by the given array range: 
					if (ray[i].iReturn == FALSE){

						//get the index of the lower bracketing element:
						bracket(			ray[i].nCoords,	ray[i].r, rHyd, &iHyd);
						//from index interpolate the rays' depth, travel time and amplitude:
						intLinear1D(		&ray[i].r[iHyd], &ray[i].z[iHyd],	rHyd, &zRay,	&junkDouble);
						intLinear1D(		&ray[i].r[iHyd], &ray[i].tau[iHyd],	rHyd, &tauRay,	&junkDouble);
						intComplexLinear1D(	&ray[i].r[iHyd], &ray[i].amp[iHyd],	rHyd, &ampRay,	&junkComplex);

						//for every hydrophone check distance to ray
						for(jj=1; jj<=settings->output.nArrayZ; jj++){
							zHyd = settings->output.arrayZ[jj];
							dz = fabs(zRay-zHyd);
							
							if (dz < settings->output.miss){
								nEigenRays += 1;

								//adjust the ray's last set of coordinates so that it matches up with the hydrophone
								ray[i].r[iHyd+1]	= rHyd;
								ray[i].z[iHyd+1]	= zRay;
								ray[i].tau[iHyd+1]	= tauRay;
								ray[i].amp[iHyd+1]	= ampRay;

								///prepare to write ray to matfile:
								temp2D 		= malloc(5*sizeof(uintptr_t));
								temp2D[0]	= ray[i].r;
								temp2D[1]	= ray[i].z;
								temp2D[2]	= ray[i].tau;
								temp2D[3]	= mallocDouble( iHyd + 2 );
								temp2D[4]	= mallocDouble( iHyd + 2 );
								for(k=0; k<iHyd+2; k++){
									temp2D[3][k] = creal( ray[i].amp[k] );
									temp2D[4][k] = cimag( ray[i].amp[k] );
								}

								//(copy data to mxArray and write ray to file:
								pRay = mxCreateDoubleMatrix(5, (int32_t)(iHyd+2), mxREAL);
								if(pRay == NULL){
									fatal("Memory alocation error.");
								}
								copyDoubleToPtr2D(temp2D, mxGetPr(pRay), iHyd+1,5);
								sprintf(string, "ray%u", (uint32_t)(i+1));
								matPutVariable(matfile, (const char*)string, pRay);

								//free memory:
								mxDestroyArray(pRay);
								free(temp2D[3]);
								free(temp2D[4]);
								free(temp2D);
								///ray has been written to matfile.
							}//	if (dz settings->output.miss)
						}//		for(jj=1; jj<=settings->output.nArrayZ; jj++)
					}else{//	if (ray[i].iReturn == FALSE)
					
						//get the indexes of the bracketing points.
						eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);

						//from each index interpolate the rays' depth, travel time and amplitude:
						for(l=0; l<nRet; l++){
							intLinear1D(		&ray[i].r[iRet[l]], &ray[i].z[iRet[l]],		rHyd, &zRay,	&junkDouble);
							intLinear1D(		&ray[i].r[iRet[l]], &ray[i].tau[iRet[l]],	rHyd, &tauRay,	&junkDouble);
							intComplexLinear1D(	&ray[i].r[iRet[l]], &ray[i].amp[iRet[l]],	(complex double)rHyd, &ampRay,	&junkComplex);

							//for every hydrophone check if the ray is close enough to be considered an eigenray:
							for(jj=0;jj<settings->output.nArrayZ; jj++){
								zHyd = settings->output.arrayZ[jj];
								dz = fabs( zRay - zHyd );
								
								if (dz < settings->output.miss){
									nEigenRays += 1;
									ray[i].r[iHyd+1]	= rHyd;
									ray[i].z[iHyd+1]	= zRay;
									ray[i].tau[iHyd+1]	= tauRay;
									ray[i].amp[iHyd+1]	= ampRay;
									
									///prepare to write ray to matfile:
									temp2D 		= malloc(5*sizeof(uintptr_t));
									temp2D[0]	= ray[i].r;
									temp2D[1]	= ray[i].z;
									temp2D[2]	= ray[i].tau;
									temp2D[3]	= mallocDouble( iHyd + 2 );
									temp2D[4]	= mallocDouble( iHyd + 2 );
									for(k=0; k<iHyd+2; k++){
										temp2D[3][k] = creal( ray[i].amp[k] );
										temp2D[4][k] = cimag( ray[i].amp[k] );
									}
									
									//(copy data to mxArray and write ray to file:
									pRay = mxCreateDoubleMatrix(5, (int32_t)(iHyd+2), mxREAL);
									if(pRay == NULL){
										fatal("Memory alocation error.");
									}
									copyDoubleToPtr2D(temp2D, mxGetPr(pRay), iHyd+1,5);
									sprintf(string, "ray%u", (uint32_t)(i+1));
									matPutVariable(matfile, (const char*)string, pRay);
									
									//free memory:
									mxDestroyArray(pRay);
									free(temp2D[3]);
									free(temp2D[4]);
									free(temp2D);
									///ray has been written to matfile.
								}
							}
						}
					}//	if (ray[i].iReturn == FALSE)
				}//if ( (rHyd >= ray[i].rMin) && (rHyd < ray[i].rMax))
			}//for(j=0; j<settings->output.nArrayR; j++){
		}//if (ctheta > 1.0e-7)
	}//for(i=0; i<settings->source.nThetas; i++)

	///Write number of eigenrays to matfile:
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
