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
#include "pressureMStar.c"
#include <complex.h>

void	calcCohAcoustPress(settings_t*);

void	calcCohAcoustPress(settings_t* settings){
	DEBUG(1,"in\n");
	MATFile*			matfile	= NULL;
	mxArray*			pThetas	= NULL;
	mxArray*			pTitle	= NULL;
	mxArray*			pHydArrayR	= NULL;
	mxArray*			pHydArrayZ	= NULL;
	mxArray*			p	= NULL;
	double				omega, lambda;
	uintptr_t			i, j, jj, k, l, iHyd;
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
	double				dr, dz;	//used for star pressure contributions (for particle velocity)

	//get dimensions of hydrophone array:
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
			/*	in linear arrays, nArrayR and nArrayZ have to be equal
			* 	(this is checked in readIn.c when reading the file).
			* 	The pressure components will be written to the rightmost index
			* 	of the 2d-array.
			*/
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
	
	//open the corresponding file:
	switch(settings->output.calcType){
		case CALC_TYPE__COH_ACOUS_PRESS:
			matfile		= matOpen("cpr.mat", "w");
			break;
		case CALC_TYPE__PART_VEL:
			matfile		= matOpen("pvl.mat", "w");
			break;
		case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
			matfile		= matOpen("pav.mat", "w");
			break;
		case CALC_TYPE__COH_TRANS_LOSS:
			matfile		= matOpen("ctl.mat", "w");
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
	

	/**
	 * Allocate memory for pressure and some other case specific initialization
	 */
	if( settings->output.calcType == CALC_TYPE__PART_VEL ||
		settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL){
		/**
		 *	In these cases, we will need memory to save the horizontal/vertical pressure components
		 *	(pressure_H[3], presure_V[3])
		 *	see also: globals.h, output struct
		 */
		 
		//Determine the size of the "star" (the vertical/horizontal offset for the pressure contribuitions).
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
		DEBUG(1, "dr: %lf; dz: %lf\n", dr, dz);
		
		//malloc memory for horizontal and vertical pressure components:
		settings->output.pressure_H = malloc( dimR * sizeof(uintptr_t));
		settings->output.pressure_V = malloc( dimR * sizeof(uintptr_t));
		if (	settings->output.pressure_H == NULL || settings->output.pressure_V == NULL){
			fatal("Memory alocation error.");
		}
		for (i=0; i<dimR; i++){
			settings->output.pressure_H[i] = malloc(dimZ * sizeof(complex double[3]));
			settings->output.pressure_V[i] = malloc(dimZ * sizeof(complex double[3]));
			if(settings->output.pressure_H[i] == NULL || settings->output.pressure_V[i] == NULL){
				fatal("Memory allocation error.");
			}
		}
	}
	if( settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS	||
		settings->output.calcType == CALC_TYPE__COH_TRANS_LOSS	||
		settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL){
			/**
			 * when calculating only the Acoustic Pressure only need memory the simple pressure, no H/V components.
			 * when calculating both Acoustic Pressure and Particle Velocity, pressure2D is used as a temporary
			 * variable at the end of the file to obtain the simple pressure from the center elements of
			 * star pressure contributions.
			 */
			settings->output.pressure2D = mallocComplex2D(dimR, dimZ);
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
			//Now that the ray has been calculated let's determine the ray influence at each point of the array:
			//TODO: this is UGLY - find a better way to do it! (pull cases together?)
			switch(settings->output.calcType){
				case CALC_TYPE__PART_VEL:
				case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
					switch(settings->output.arrayType){
						case ARRAY_TYPE__HORIZONTAL:
						case ARRAY_TYPE__VERTICAL:
						case ARRAY_TYPE__RECTANGULAR:
							DEBUG(3,"Array type: Horizontal/Rectangular\n");
							DEBUG(4,"nArrayR: %u, nArrayZ: %u\n", (uint32_t)settings->output.nArrayR, (uint32_t)settings->output.nArrayZ );
							
							for(j=0; j<settings->output.nArrayR; j++){
								rHyd = settings->output.arrayR[j];
								
								//check whether the hydrophone is within the range coordinates of the ray:
								//if ( (rHyd - dr)>= ray[i].rMin	&&	(rHyd + dr) < ray[i].rMax){
								if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									
									if ( ray[i].iReturn == FALSE){
										DEBUG(5, "Ray doesn't return\n");
										for(k=0; k<dimZ; k++){
											zHyd = settings->output.arrayZ[k];
											
											if( pressureStar( settings, &ray[i], rHyd, zHyd, q0, pressure_H, pressure_V) ){
												DEBUG(7, "i=%u: (j,k)=(%u,%u): \n",(uint32_t)i, (uint32_t)j, (uint32_t)k);
												DEBUG(7, "in>>  (rH,zH)=(%.2lf,%.2lf), nCoords: %u, q0: %e\n", rHyd, zHyd, (uint32_t)ray[i].nCoords, q0);
												DEBUG(7, "out>> pL: %e,  pU, %e,  pR: %e,  pD: %e,  pC:%e\n\n", cabs(pressure_H[LEFT]), cabs(pressure_V[TOP]), cabs(pressure_H[RIGHT]), cabs(pressure_V[BOTTOM]), cabs(pressure_H[CENTER]));
															
												for (l=0; l<3; l++){
													settings->output.pressure_H[j][k][l] += pressure_H[l];
													settings->output.pressure_V[j][k][l] += pressure_V[l];
												}
											}
											//DEBUG(4, "k: %u; j: %u; pressure2D[k][j]: %e + j*%e\n", (uint32_t)k, (uint32_t)j, creal(settings->output.pressure2D[k][j]), cimag(settings->output.pressure2D[k][j]));
											//DEBUG(4, "rHyd: %lf; zHyd: %lf \n", rHyd, zHyd);
										}
									}else{
										DEBUG(5, "Ray returns\n");
										for(k=0; k<dimZ; k++){
											zHyd = settings->output.arrayZ[k];
											DEBUG(6, "i=%u: (j,k)=(%u, %u):\n",(uint32_t)i, (uint32_t)j, (uint32_t)k);
											if( pressureMStar( settings, &ray[i], rHyd, zHyd, q0, pressure_H, pressure_V) ){
												 DEBUG(6, "pL: %e, pU: %e, pR: %e, pD: %e, pC: %e\n",
														cabs(pressure_H[LEFT]),
														cabs(pressure_V[TOP]), cabs(pressure_H[RIGHT]),
														cabs(pressure_V[BOTTOM]), cabs(pressure_H[CENTER]));
												for (l=0; l<3; l++){
													settings->output.pressure_H[j][k][l] += pressure_H[l];
													settings->output.pressure_V[j][k][l] += pressure_V[l];
												}
											}else{
												DEBUG(6,"pressureMStar returned FALSE => at least one of the pressure contribution points is outside rBox\n");
											}
										}
									}
								}else{
									DEBUG(5, "Hydrophone not within range of ray coordinates.\n");
								}
							}
							break;
						case ARRAY_TYPE__LINEAR:
							for(j=0; j<dimR; j++){
								rHyd = settings->output.arrayR[j];
								if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									if ( ray[i].iReturn == FALSE){
											zHyd = settings->output.arrayZ[j];
											
											if( pressureStar( settings, &ray[i], rHyd, zHyd, q0, pressure_H, pressure_V) ){
												DEBUG(1, "i=%u: (j,k)=(%u,%u): \n",(uint32_t)i, (uint32_t)j, (uint32_t)k);
												DEBUG(1, "in>>  (rH,zH)=(%.2lf,%.2lf), nCoords: %u, q0: %e\n", rHyd, zHyd, (uint32_t)ray[i].nCoords, q0);
												DEBUG(1, "out>> pL: %e,  pU, %e,  pR: %e,  pD: %e,  pC:%e\n\n", cabs(pressure_H[LEFT]), cabs(pressure_V[TOP]), cabs(pressure_H[RIGHT]), cabs(pressure_V[BOTTOM]), cabs(pressure_H[CENTER]));
															
												for (l=0; l<3; l++){
													settings->output.pressure_H[0][j][l] += pressure_H[l];
													settings->output.pressure_V[0][j][l] += pressure_V[l];
												}
											}
											//DEBUG(4, "k: %u; j: %u; pressure2D[k][j]: %e + j*%e\n", (uint32_t)k, (uint32_t)j, creal(settings->output.pressure2D[k][j]), cimag(settings->output.pressure2D[k][j]));
											//DEBUG(4, "rHyd: %lf; zHyd: %lf \n", rHyd, zHyd);
									}else{
										zHyd = settings->output.arrayZ[j];
										
										if( pressureMStar( settings, &ray[i], rHyd, zHyd, q0, pressure_H, pressure_V) ){
											for (l=0; l<3; l++){
												settings->output.pressure_H[0][j][l] += pressure_H[l];
												settings->output.pressure_V[0][j][l] += pressure_V[l];
											}
										}
									}
								}
							}
							break;
						default:
							fatal("calcCohAcoustPress(): unknown array type.");
							break;
					}
					break;
				
				case CALC_TYPE__COH_ACOUS_PRESS:
				case CALC_TYPE__COH_TRANS_LOSS:
					switch(settings->output.arrayType){
						case ARRAY_TYPE__LINEAR:
							//NOTE: in linear arrays, nArrayR and nArrayZ have to be equal (this is checked in readIn.c when reading the file)
							DEBUG(3,"Array type: Linear\n");
							
							for(j=0; j<dimZ; j++){
								rHyd = settings->output.arrayR[j];
								zHyd = settings->output.arrayZ[j];
								
								if (	rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax	){
									
									if (ray[i].iReturn == FALSE){
										bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
										getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
										settings->output.pressure2D[0][j] += pressure;
										
									}else{
										eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
										
										for(jj=0; jj<nRet; jj++){
											
											//if the ray returns we have to check all hydrophone depths:
											for(k=0; k<dimZ; k++){
												zHyd = settings->output.arrayZ[k];
												getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
												settings->output.pressure2D[0][j] += pressure;	//TODO make sure this value is initialized
											}
										}
									}
								}
							}
							break;
						
						case ARRAY_TYPE__HORIZONTAL:
						case ARRAY_TYPE__VERTICAL:
						case ARRAY_TYPE__RECTANGULAR:
							DEBUG(3,"Array type: Rectangular/Horizontal/Vertical\n");
							DEBUG(4,"nArrayR: %u, nArrayZ: %u\n", (uint32_t)dimR, (uint32_t)dimZ );
							//TODO invert indices, so that the innermost loop loops over the rightmost index of pressure2D
							for(j=0; j<dimR; j++){
								rHyd = settings->output.arrayR[j];
								
								//Start by checking if the array range is inside the min and max ranges of the ray:
								if (	rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
									
									if (ray[i].iReturn == FALSE){
										bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
										for(k=0; k<dimZ; k++){
											
											zHyd = settings->output.arrayZ[k];
											getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
											settings->output.pressure2D[j][k] += pressure;	//verify if initialization is necessary. Done -makes no difference.
											DEBUG(4, "k: %u; j: %u; pressure2D[k][j]: %e + j*%e\n", (uint32_t)k, (uint32_t)j, creal(settings->output.pressure2D[k][j]), cimag(settings->output.pressure2D[k][j]));
											DEBUG(4, "rHyd: %lf; zHyd: %lf \n", rHyd, zHyd);
										}
									
									}else{
										eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
										for(k=0; k<dimZ; k++){
											zHyd = settings->output.arrayZ[k];
											
											for(jj=0; jj<nRet; jj++){
												getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
												settings->output.pressure2D[j][k] += pressure;
											}
										}
									}
								}
							}
							break;
						
						default:
							fatal("calcCohAcoustPress(): unknown array type.");
							break;
					}
					break;
				default:
					fatal("calcCohAcoustPress(): Unknown output type.");
					break;
			}//switch(settings->output.calcType){
		}//if (ctheta > 1.0e-7)
	}//for(i=0; i<settings->source.nThetas; i++
	
	DEBUG(3,"Rays and pressure calculated\n");
	/*********************************************
	 * Write Acoustic pressure to file, if needed.
	 */
	if(	settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS ||
		settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL){
		
		/* -- When the desired output is only the Coherent Acoustic Pressure,
		 * 	  then the values from pressure2D are used.
		 * -- When calculating Coherent Acoustic Pressure and Particle Velocity,
		 * 	  the acoustic pressure is obtained from the center elements of pressure_H and pressure_V.
		 */
		if( settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL ){
			//obtain acoustic pressure from center elements of directional components
			for(j=0; j<dimR; j++){
				for(i=0; i<dimZ; i++){
					settings->output.pressure2D[j][i] = settings->output.pressure_H[j][i][CENTER];
				}
			}
		}
		//create mxArray
		p = mxCreateDoubleMatrix((MWSIZE)dimZ, (MWSIZE)dimR, mxCOMPLEX);
		
		//verify if memory allocation was successfull:
		if( p == NULL){
			fatal("Memory alocation error.");
		}
		
		//copy pressure to mxArray:
		switch(	settings->output.arrayType){
			case ARRAY_TYPE__LINEAR:
				/*
				copyComplexToPtr(settings->output.pressure1D, p, dimZ);
				break;
				*/
			
			case ARRAY_TYPE__VERTICAL:
			case ARRAY_TYPE__HORIZONTAL:
			case ARRAY_TYPE__RECTANGULAR:
				//Note: the output for rectangular arrays has to be transposed.
				copyComplexToPtr2D_transposed(settings->output.pressure2D, p, dimZ, dimR);
				break;
		}
		
		//write mxArray to matfile:
		matPutVariable(matfile, "p", p);
		mxDestroyArray(p);
	}
	
	//free memory for pressure, only if not needed for calculating Transmission Loss (or others):
	//this is now done at the end of cTraceo.c, using freeSettings() from toolsMemory.c
	
	//free ray memory.
	for(i=0; i<settings->source.nThetas; i++){
		reallocRayMembers(&ray[i], 0);
	}
	free(ray);

	//TODO free H/V pressure components
	
	matClose(matfile);
	DEBUG(1,"out\n");
}
