/********************************************************************************
 *	calcCohAcoustPress.c	 													*
 * 	(formerly "calcpr.for")														*
 *	Calculates Coherent Acoustic Pressure.										*
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
 * 				none:		Writes to file "cpr.mat".							*
 * 																				*
 *******************************************************************************/

#include "globals.h"
#include "getRayPressure.c"
#include <mat.h>
#include <matrix.h>
#include <math.h>
#include "solveEikonalEq.c"
#include "solveDynamicEq.c"
#include "getRayPressure.c"
#include "pressureStar.c"

void	calcCohAcoustPress(settings_t*);

void	calcCohAcoustPress(settings_t* settings){
	DEBUG(1,"in\n");
	MATFile*			matfile	= NULL;
	mxArray*			pThetas	= NULL;
	mxArray*			pTitle	= NULL;
	mxArray*			pHydArrayR	= NULL;
	mxArray*			pHydArrayZ	= NULL;
	mxArray*			pPressure_a	= NULL;
	mxArray*			pPressure_b	= NULL;
	double				omega, lambda;
	uintptr_t			i, j, jj, k, l, iHyd, dim;
	uintptr_t			dimR, dimZ;
	ray_t*				ray = NULL;
	double 				ctheta, thetai, cx, q0;
	double 				junkDouble;
	vector_t			junkVector;
	double 				rHyd, zHyd;
	complex double 		pressure;
	complex double		pressure_H[3];
	complex double		pressure_V[3];
	uintptr_t			nRet;
	uintptr_t			iRet[51];
	double**			temp2D_a = NULL;
	double**			temp2D_b = NULL;
	double				dr, dz;	//used for star pressure contributions (for particle velocity)

	switch(settings->output.calcType){
		case CALC_TYPE__COH_ACOUS_PRESS:
		case CALC_TYPE__COH_TRANS_LOSS:
			matfile		= matOpen("cpr.mat", "w");
			break;
		case CALC_TYPE__PART_VEL:
			matfile		= matOpen("pvl.mat", "w");
			break;
		default:
			fatal("Uh-oh - calcCohAcoustPress(): unknown output type.");
			break;
	}
	pThetas		= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL)
		fatal("Memory alocation error.");
	
	//copy angles in cArray to mxArray:
	copyDoubleToPtr(	settings->source.thetas,
						mxGetPr(pThetas),
						settings->source.nThetas);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "thetas", pThetas);
	mxDestroyArray(pThetas);

	//write title to matfile:
	pTitle = mxCreateString("TRACEO: Coherent Acoustic Pressure");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);

	//write hydrophone array ranges to file:
	pHydArrayR	= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
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
	pHydArrayZ	= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
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
	
	
	//get sound speed at source (cx):
	csValues( 	settings, settings->source.rx, settings->source.zx, &cx,
				&junkDouble, &junkDouble, &junkDouble, &junkDouble,
				&junkVector, &junkDouble, &junkDouble, &junkDouble);
			
	q0 = cx / ( M_PI * settings->source.dTheta/180.0 );
	omega  = 2.0 * M_PI * settings->source.freqx;
	
	
	if(	settings->output.calcType == CALC_TYPE__PART_VEL ||
		settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL){
		/**
		 *	In these cases, we will need memory to save the horizontal/vertical pressure components
		 *	(pressure_H[3], presure_V[3])
		 *	see also: globals.h, output struct
		 */

		 
		//Determine the size of the "star".
		//This is the vertical/horizontal offset for the pressure contribuitions.
		lambda	= cx/settings->source.freqx;
		dr = lambda/10;
		dz = lambda/10;
		
		for (i=1; i<settings->output.nArrayR; i++){
			dr = min( fabs( settings->output.arrayR[i] - settings->output.arrayR[i-1]), dr);
		}
		for (i=1; i<settings->output.nArrayZ; i++){
			dr = min( fabs( settings->output.arrayZ[i] - settings->output.arrayZ[i-1]), dz);
		}
		
		settings->output.dr = dr;
		settings->output.dz = dz;

		//determine necessary memory for pressure components:
		switch(settings->output.arrayType){
			case ARRAY_TYPE__HORIZONTAL:
				dimR = settings->output.nArrayR;
				dimZ = 1;
				break;
				
			case ARRAY_TYPE__VERTICAL:
				dimR = 1;
				dimZ = settings->output.nArrayZ;
				break;
				
			case ARRAY_TYPE__LINEAR:
				//in linear arrays, nArrayR and nArrayZ have to be equal (this is checked in readIn.c when reading the file)
				dimR = 1;
				dimZ = settings->output.nArrayZ;	//this should be equal to nArrayR
				break;
				
			case ARRAY_TYPE__RECTANGULAR:
				dimR = settings->output.nArrayR;
				dimZ = settings->output.nArrayZ;
				break;
				
			default:
				fatal("calcCohAcoustPress(): unknown array type.\nAborting.");
				break;
		}
		//malloc memory for horizontal and vertical pressure components:
		settings->output.pressure_H = malloc( dimR * sizeof(uintptr_t));
		settings->output.pressure_V = malloc( dimR * sizeof(uintptr_t));
		for (i=0; i<dimR; i++){
			settings->output.pressure_H = malloc(dimZ * sizeof(complex double[3]));
			settings->output.pressure_V = malloc(dimZ * sizeof(complex double[3]));
		}
		
	}else{
		/**
		 * in this case we only need memory the simple pressure, no H/V components
		 */
		//allocate memory for the results:
		switch(settings->output.arrayType){
			case ARRAY_TYPE__HORIZONTAL:
				settings->output.pressure1D = mallocComplex(settings->output.nArrayR);
				break;
				
			case ARRAY_TYPE__VERTICAL:
				settings->output.pressure1D = mallocComplex(settings->output.nArrayZ);
				break;
				
			case ARRAY_TYPE__LINEAR:
				//in linear arrays, nArrayR and nArrayZ have to be equal (this is checked in readIn.c when reading the file)
				settings->output.pressure1D = mallocComplex(settings->output.nArrayZ);
				break;
				
			case ARRAY_TYPE__RECTANGULAR:
				settings->output.pressure2D = mallocComplex2D(settings->output.nArrayZ, settings->output.nArrayR);
				//inicialize to 0:
				/*
				for (j=0; j<settings->output.nArrayZ; j++){
					for (k=0; k<settings->output.nArrayR; k++){
						settings->output.pressure2D[j][k] = 0;
					}
				}
				DEBUG(5, "memory initialized\n");
				*/
				break;
				
			default:
				fatal("calcCohAcoustPress(): unknown array type.\nAborting.");
				break;
		}
	}
	
	///Solve the EIKonal and the DYNamic sets of EQuations:
	for(i=0; i<settings->source.nThetas; i++){
		thetai = -settings->source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		ctheta = fabs( cos(thetai));

		//Trace a ray as long as it is neither at 90 nor -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);
			
			DEBUG(3,"q0: %e\n", q0);
			//TODO: this is UGLY - find a better way to do it! (pull cases together?)
			//Now that the ray has been calculated let's determine the ray influence at each point of the array:
			switch(settings->output.arrayType){
				case ARRAY_TYPE__HORIZONTAL:
					DEBUG(3,"Array type: Horizontal\n");
					zHyd = settings->output.arrayZ[0];
					
					switch(settings->output.calcType){
						/*
						 * NOTE:	the code for both cases of this "switch" is almost identical,
						 * 			yet it is kept unrolled for performance reasons.
						 */
						case CALC_TYPE__PART_VEL:
							for(j=0; j<settings->output.nArrayR; j++){
								rHyd = settings->output.arrayR[j];
								
								//check whether the hydrophone is within the range coordinates of the ray:
								if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									
									if ( ray[i].iReturn == FALSE){
										pressureStar( settings, &ray[i], rHyd, zHyd, q0, pressure_H, pressure_V);

										//settings->output.pressure1D[j] += pressure;
										for (l=0; l<3; l++){
											settings->output.pressure_H[0][j][l] += pressure_H[l];
											settings->output.pressure_V[0][j][l] += pressure_V[l];
										}
										
									}else{
										fatal("the end");	//TODO
										/*
										eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
										for(jj=0; jj<nRet; jj++){
											getRayPressure( settings, &ray[i], iRet[jj], q0, rHyd, zHyd,	&pressure);
											
											settings->output.pressure1D[j] += pressure;
										}
										*/
									}
								}
							}
							break;
						
						default:
							for(j=0; j<settings->output.nArrayR; j++){
								rHyd = settings->output.arrayR[j];
								
								//check whether the hydrophone is within the range coordinates of the ray:
								if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									
									//Check if the ray is returning back or not;
									//if not we can bracket it without problems, otherwise we need to know how many
									//times it passed by the given array range: 
									if ( ray[i].iReturn == FALSE){
										//find the index of the ray coordinate that brackets the hydrophone
										bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
										getRayPressure( settings, &ray[i], iHyd, q0, rHyd, zHyd,	&pressure);
										
										settings->output.pressure1D[j] += pressure;
									}else{
										eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
										for(jj=0; jj<nRet; jj++){
											getRayPressure( settings, &ray[i], iRet[jj], q0, rHyd, zHyd,	&pressure);
											
											settings->output.pressure1D[j] += pressure;
										}
									}
								}
							}
							break;
					}
					break;
				
				case ARRAY_TYPE__VERTICAL:
					DEBUG(3,"Array type: Vertical\n");
					rHyd = settings->output.arrayR[0];
					

					if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax	){
						//Check if the ray is returning back or not;
						//if not we can bracket it without problems, otherwise we need to know how many times
						//it passed by the given array range: 

						if (ray[i].iReturn == FALSE){
							bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
							
							for(j=0; j<settings->output.nArrayZ; j++){
								zHyd = settings->output.arrayZ[j];
								getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
								settings->output.pressure1D[j] = pressure;
							}
						}else{
							eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
							for(jj=0; jj<nRet; jj++){
								for(j=0; j<settings->output.nArrayZ; j++){
									zHyd = settings->output.arrayZ[j];

									getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
									settings->output.pressure1D[j] += pressure;	//TODO make sure this value is initialized
								}
							}
						}
					}
					break;
				
				case ARRAY_TYPE__LINEAR:
					DEBUG(3,"Array type: Linear\n");
					
					for(j=0; j<settings->output.nArrayZ; j++){
						rHyd = settings->output.arrayR[j];
						zHyd = settings->output.arrayZ[j];

						if (	rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax	){

							if (ray[i].iReturn == FALSE){
								bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
								getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
								settings->output.pressure1D[j] += pressure;
								
							}else{
								eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
								
								for(jj=0; jj<nRet; jj++){
									
									//if the ray returns we have to check all hydrophone depths:
									for(k=0; k<settings->output.nArrayZ; k++){
										zHyd = settings->output.arrayZ[k];
										getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
										settings->output.pressure1D[j] += pressure;	//TODO make sure this value is initialized
									}
								}
							}
						}
					}
					break;
				
				case ARRAY_TYPE__RECTANGULAR:
					DEBUG(3,"Array type: Rectangular\n");
					DEBUG(4,"nArrayR: %u, nArrayZ: %u\n", (uint32_t)settings->output.nArrayR, (uint32_t)settings->output.nArrayZ );

					switch(settings->output.calcType){
						/*
						 * NOTE:	the code for both cases of this "switch" is almost identical,
						 * 			yet it is kept unrolled for performance reasons.
						 */
						case CALC_TYPE__PART_VEL:
							for(k=0; j<settings->output.nArrayR; j++){
								rHyd = settings->output.arrayR[j];
								
								//check whether the hydrophone is within the range coordinates of the ray:
								if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									
									if ( ray[i].iReturn == FALSE){
										for(k=0; k<settings->output.nArrayZ; k++){
											zHyd = settings->output.arrayZ[k];

											pressureStar( settings, &ray[i], rHyd, zHyd, q0, pressure_H, pressure_V);
											for (l=0; l<3; l++){
												settings->output.pressure_H[j][k][l] += pressure_H[l];
												settings->output.pressure_V[j][k][l] += pressure_V[l];
											}
											DEBUG(4, "k: %u; j: %u; pressure2D[k][j]: %e + j*%e\n", (uint32_t)k, (uint32_t)j, creal(settings->output.pressure2D[k][j]), cimag(settings->output.pressure2D[k][j]));
											DEBUG(4, "rHyd: %lf; zHyd: %lf \n", rHyd, zHyd);
										}
									}else{
										fatal("the end");	//TODO
										/*
										eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
										for(jj=0; jj<nRet; jj++){
											getRayPressure( settings, &ray[i], iRet[jj], q0, rHyd, zHyd,	&pressure);
											
											settings->output.pressure1D[j] += pressure;
										}
										*/
									}
								}
							}
							break;
						
						default:
							//TODO invert indices, so that the innermost loop loops over the rightmost index of pressure2D
							for(j=0; j<settings->output.nArrayR; j++){
								rHyd = settings->output.arrayR[j];
								
								//Start by checking if the array range is inside the min and max ranges of the ray:
								if (	rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									
									if (ray[i].iReturn == FALSE){
										bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
										for(k=0; k<settings->output.nArrayZ; k++){
											
											zHyd = settings->output.arrayZ[k];
											getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
											settings->output.pressure2D[k][j] += pressure;	//verify if initialization is necessary. Done -makes no difference.
											DEBUG(4, "k: %u; j: %u; pressure2D[k][j]: %e + j*%e\n", (uint32_t)k, (uint32_t)j, creal(settings->output.pressure2D[k][j]), cimag(settings->output.pressure2D[k][j]));
											DEBUG(4, "rHyd: %lf; zHyd: %lf \n", rHyd, zHyd);
										}
									
									}else{
										eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
										for(k=0; k<settings->output.nArrayZ; k++){
											zHyd = settings->output.arrayZ[k];
											
											for(jj=0; jj<nRet; jj++){
												getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
												settings->output.pressure2D[k][j] += pressure;
											}
										}
									}
								}
							}
							break;
					}
					break;
				
				default:
					fatal("calcCohAcoustPress(): unknown array type.");
					break;
				
			}//switch()
		}//if (ctheta > 1.0e-7)
	}//for(i=0; i<settings->source.nThetas; i++
	
	DEBUG(3,"Rays and pressure calculated\n");
	/***********************************************************************
	 *      Write the data to the output file:
	 **********************************************************************/
	if (settings->output.arrayType != ARRAY_TYPE__RECTANGULAR){
		dim = (uintptr_t)max((double)settings->output.nArrayR, (double)settings->output.nArrayZ);
		temp2D_a = mallocDouble2D(2, dim);
		//In the fortran version there were problems when passing complex matrices to Matlab; 
		//therefore the real and complex parts will be saved separately: TODO correct this
		for(j=0; j<dim; j++){
			temp2D_a[0][j] = creal( settings->output.pressure1D[j]);
			temp2D_a[1][j] = cimag( settings->output.pressure1D[j]);
		}
		
		pPressure_a = mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)dim, mxREAL);
		if(pPressure_a == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(temp2D_a, mxGetPr(pPressure_a), dim, 2);
		matPutVariable(matfile, "p", pPressure_a);
		mxDestroyArray(pPressure_a);

		freeDouble2D(temp2D_a, 2);
	}else{
		
		DEBUG(3,"Writing pressure output of rectangular array to file:\n");
		//In the fortran version there were problems when passing complex matrices to Matlab; 
		//therefore the real and complex parts will be saved separately: TODO correct this
		temp2D_a = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		temp2D_b = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		for(i=0; i<settings->output.nArrayZ; i++){
			for(j=0; j<settings->output.nArrayR; j++){
				temp2D_a[i][j] = creal( settings->output.pressure2D[i][j]);
				temp2D_b[i][j] = cimag( settings->output.pressure2D[i][j]);
			}
		}
		
		//write the real part to the mat-file:
		pPressure_a = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(pPressure_a == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(temp2D_a, mxGetPr(pPressure_a), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "rp", pPressure_a);
		mxDestroyArray(pPressure_a);
		
		//write the imaginary part to the mat-file:
		pPressure_b = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(pPressure_b == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(temp2D_b, mxGetPr(pPressure_b), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "ip", pPressure_b);
		mxDestroyArray(pPressure_b);
		
		freeDouble2D(temp2D_a, settings->output.nArrayZ);
		freeDouble2D(temp2D_b, settings->output.nArrayZ);
	}

	//free memory for pressure, only if not needed for calculating Transmission Loss (or others):
	//this is now done at the end of cTraceo.c, using freeSettings() from tools.c
	
	//free ray memory.
	for(i=0; i<settings->source.nThetas; i++){
		reallocRayMembers(&ray[i], 0);
	}
	free(ray);

	//TODO free H/V pressure components
	
	matClose(matfile);
	DEBUG(1,"out\n");
}
