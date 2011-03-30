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
	double				omega;
	uintptr_t			i, j, jj, k, iHyd, dim;
	ray_t*				ray = NULL;
	double 				ctheta, thetai, cx, q0;
	double 				junkDouble;
	vector_t			junkVector;
	double 				rHyd, zHyd;
	complex double 		pressure;
	complex double*		press1D = NULL;
	complex double**	press2D = NULL;
	uintptr_t			nRet;
	uintptr_t			iRet[51];
	double**			temp2D_a = NULL;
	double**			temp2D_b = NULL;


	matfile		= matOpen("cpr.mat", "w");
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

	//allocate memory for the results:
	switch(settings->output.arrayType){
		case ARRAY_TYPE__HORIZONTAL:
			press1D = mallocComplex(settings->output.nArrayR);
			break;
			
		case ARRAY_TYPE__VERTICAL:
			press1D = mallocComplex(settings->output.nArrayZ);
			break;
			
		case ARRAY_TYPE__LINEAR:
			press1D = mallocComplex(settings->output.nArrayZ);
			break;
			
		case ARRAY_TYPE__RECTANGULAR:
			press2D = mallocComplex2D(settings->output.nArrayZ, settings->output.nArrayR);
			//inicialize to 0:
			/*
			for (j=0; j<settings->output.nArrayZ; j++){
				for (k=0; k<settings->output.nArrayR; k++){
					press2D[j][k] = 0;
				}
			}
			DEBUG(5, "memory initialized\n");
			*/
			break;
			
		default:
			fatal("calcCohAcoustPress(): unknown array type.\nAborting.");
			break;
	}

	///Solve the EIKonal and the DYNamic sets of EQuations:
	omega  = 2.0 * M_PI * settings->source.freqx;
	
	for(i=0; i<settings->source.nThetas; i++){
		thetai = -settings->source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		ctheta = fabs( cos(thetai));

		//Trace a ray as long as it is neither at 90 nor -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);
			
			//get sound speed at source (cx):
			csValues( 	settings, settings->source.rx, settings->source.zx, &cx,
						&junkDouble, &junkDouble, &junkDouble, &junkDouble,
						&junkVector, &junkDouble, &junkDouble, &junkDouble);
			
			q0 = cx / ( M_PI * settings->source.dTheta/180.0 );
			DEBUG(3,"q0: %e\n", q0);
			//TODO: this is UGLY - find a better way to do it! (pull cases together?)
			//Now that the ray has been calculated let's determine the ray influence at each point of the array:
			switch(settings->output.arrayType){
				case ARRAY_TYPE__HORIZONTAL:
					DEBUG(3,"Array type: Horizontal\n");
					zHyd = settings->output.arrayZ[0];
					
					
					for(j=0; j<settings->output.nArrayR; j++){
						rHyd = settings->output.arrayR[j];
						
						//check wether the hydrophone is within the range coordinates of the ray:
						if ( rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
							press1D[j] = 0 + 0*I;
							
							//Check if the ray is returning back or not;
							//if not we can bracket it without problems, otherwise we need to know how many
							//times it passed by the given array range: 
							if ( ray[i].iReturn == FALSE){
								//find the index of the ray coordinate that brackets the hydrophone
								bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
								getRayPressure( /*in:*/	settings, &ray[i], iHyd, q0, rHyd, zHyd, /*out:*/	&pressure);
								
								press1D[j] += pressure;
							}else{
								eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
								for(jj=0; jj<nRet; jj++){
									getRayPressure( /*in:*/	settings, &ray[i], iRet[jj], q0, rHyd, zHyd,  /*out:*/	&pressure);
									
									press1D[j] += pressure;
								}
							}
						}
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
								press1D[j] = pressure;
							}
						}else{
							eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
							for(jj=0; jj<nRet; jj++){
								for(j=0; j<settings->output.nArrayZ; j++){
									zHyd = settings->output.arrayZ[j];

									getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
									press1D[j] += pressure;	//TODO make sure this value is initialized
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
						press1D[j] = 0;

						if (	rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax	){

							if (ray[i].iReturn == FALSE){
								bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
								getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
								press1D[j] += pressure;
								
							}else{
								eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
								
								for(jj=0; jj<nRet; jj++){
									
									//if the ray returns we have to check all hydrophone depths:
									for(k=0; k<settings->output.nArrayZ; k++){
										zHyd = settings->output.arrayZ[k];
										getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
										press1D[j] += pressure;	//TODO make sure this value is initialized
									}
								}
							}
						}
					}
					break;
				
				case ARRAY_TYPE__RECTANGULAR:
					DEBUG(3,"Array type: Rectangular\n");
					DEBUG(4,"nArrayR: %u, nArrayZ: %u\n", (uint32_t)settings->output.nArrayR, (uint32_t)settings->output.nArrayZ );
					
					//TODO invert indices, so that the innermost loop loops over the rightmost index of press2D
					for(j=0; j<settings->output.nArrayR; j++){
						rHyd = settings->output.arrayR[j];
						
						//Start by checking if the array range is inside the min and max ranges of the ray:
						if (	rHyd >= ray[i].rMin	&&	rHyd < ray[i].rMax){
							
							if (ray[i].iReturn == FALSE){
								bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
								for(k=0; k<settings->output.nArrayZ; k++){
									
									zHyd = settings->output.arrayZ[k];
									getRayPressure(settings, &ray[i], iHyd, q0, rHyd, zHyd, &pressure);
									press2D[k][j] += pressure;	//verify if initialization is necessary. Done -makes no difference.
									DEBUG(4, "k: %u; j: %u; press2D[k][j]: %e + j*%e\n", (uint32_t)k, (uint32_t)j, creal(press2D[k][j]), cimag(press2D[k][j]));
									DEBUG(4, "rHyd: %lf; zHyd: %lf \n", rHyd, zHyd);
								}
							
							}else{
								eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);
								for(k=0; k<settings->output.nArrayZ; k++){
									zHyd = settings->output.arrayZ[k];
									//press2D[k][j] = 0;
									for(jj=0; jj<nRet; jj++){
										getRayPressure(settings, &ray[i], iRet[jj], q0, rHyd, zHyd, &pressure);
										press2D[k][j] += pressure;
									}
								}
							}
						}
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
			temp2D_a[1][j] = creal( press1D[j]);
			temp2D_a[2][j] = cimag( press1D[j]);
		}
		
		pPressure_a = mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)dim, mxREAL);
		copyDoubleToPtr2D(temp2D_a, mxGetPr(pPressure_a), dim,2);
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
				temp2D_a[i][j] = creal( press2D[i][j]);
				temp2D_b[i][j] = cimag( press2D[i][j]);
			}
		}
		
		//write the real part to the mat-file:
		pPressure_a = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		copyDoubleToPtr2D(temp2D_a, mxGetPr(pPressure_a), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "rp", pPressure_a);
		mxDestroyArray(pPressure_a);
		
		//write the imaginary part to the mat-file:
		pPressure_b = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		copyDoubleToPtr2D(temp2D_b, mxGetPr(pPressure_b), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "ip", pPressure_b);
		mxDestroyArray(pPressure_b);
		
		freeDouble2D(temp2D_a, settings->output.nArrayZ);
		freeDouble2D(temp2D_b, settings->output.nArrayZ);
	}

	//free memory:
	switch(settings->output.arrayType){
		case ARRAY_TYPE__HORIZONTAL:
		case ARRAY_TYPE__VERTICAL:
		case ARRAY_TYPE__LINEAR:
			free(press1D);
			break;
			
		case ARRAY_TYPE__RECTANGULAR:
			freeComplex2D(press2D, settings->output.nArrayZ);		//TODO this is a memory leak -write a freeComplex2D function.
			break;
	}
	matClose(matfile);
	DEBUG(1,"out\n");
}
