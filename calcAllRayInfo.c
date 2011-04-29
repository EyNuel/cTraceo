/************************************************************************************
 *	calcRayCoords.c		 															*
 * 	(formerly "calrco.for")															*
 * 	Uses solveEikonalEq for raytracing and writes ray coordinates to .mat file.		*
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
 * 				settings	All input information.									*
 * 	Outputs:																		*
 * 				"ari.mat":	A file containing the coordinates of all rays.			*
 * 																					*
 ***********************************************************************************/

#include <complex.h>
#include "globals.h"
#include "tools.h"
#include "solveDynamicEq.c"
#include "solveEikonalEq.c"
#include <mat.h>
#include "matrix.h"

void	calcAllRayInfo(settings_t*);

void	calcAllRayInfo(settings_t* settings){
	DEBUG(1,"in\n");
	MATFile*	matfile	= NULL;
	mxArray*	pThetas	= NULL;
	mxArray*	pTitle	= NULL;
	mxArray*	pRay	= NULL;
	mxArray*	pRefrac	= NULL;
	mxArray*	pRayInfo= NULL;
	double		thetai, ctheta;
	ray_t*		ray		= NULL;
	double**	temp2D 	= NULL;
	double**	rayInfo = mallocDouble2D(5,settings->source.nThetas);
	uintptr_t	i, j;
	char* 		string	= mallocChar(10);


	matfile = matOpen("ari.mat", "w");
	pThetas		= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
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
	pTitle = mxCreateString("TRACEO: All Ray Information");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);
	
	//allocate memory for the rays:
	ray = makeRay(settings->source.nThetas);
	
	/** Trace the rays:	*/
	for(i=0; i<settings->source.nThetas; i++){
		thetai = -settings->source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		DEBUG(2,"ray[%u].theta: %lf\n", (uint32_t)i, settings->source.thetas[i]);
		ctheta = fabs( cos(thetai));
		
		//Trace a ray as long as it is neither 90 or -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);
			DEBUG(4, "Equations solved.\n");
			
			///prepare to write ray to matfile:
			temp2D 		= malloc(5*sizeof(uintptr_t));
			temp2D[0]	= ray[i].r;
			temp2D[1]	= ray[i].z;
			temp2D[2]	= ray[i].tau;
			temp2D[3]	= mallocDouble(ray[i].nCoords);
			temp2D[4]	= mallocDouble(ray[i].nCoords);
			for(j=0; j<ray[i].nCoords; j++){
				temp2D[3][j] = creal( ray[i].amp[j] );
				temp2D[4][j] = cimag( ray[i].amp[j] );
			}
			DEBUG(4, "Created temporary variables for ray.\n");
			
			//copy data to mxArray and write ray to file:
			pRay = mxCreateDoubleMatrix((MWSIZE)5, (MWSIZE)ray[i].nCoords, mxREAL);
			if(pRay == NULL){
				fatal("Memory alocation error.");
			}
			copyDoubleToPtr2D(temp2D, mxGetPr(pRay), ray[i].nCoords,5);
			DEBUG(4, "Copied ray to matlab array.\n");

			sprintf(string, "ray%u", (uint32_t)(i+1));
			matPutVariable(matfile, (const char*)string, pRay);
			DEBUG(4, "Wrote ray to matfile.\n");

			//free memory
			mxDestroyArray(pRay);
			free(temp2D[3]);
			free(temp2D[4]);
			free(temp2D);
			DEBUG(4, "Freed temporary variables.\n");
			///ray has been written to matfile

			///save ray information:
			rayInfo[0][i] = (double)ray[i].iReturn;
			rayInfo[1][i] = ray[i].sRefl;
			rayInfo[2][i] = ray[i].bRefl;
			rayInfo[3][i] = ray[i].oRefl;
			rayInfo[4][i] = (double)ray[i].nRefrac;
			DEBUG(4, "Created temporary variables for reflections counters.\n");

			if (ray[i].nRefrac > 0){
				temp2D 		= malloc(2*sizeof(uintptr_t));
				temp2D[0]	= mallocDouble(ray[i].nRefrac);
				temp2D[1]	= mallocDouble(ray[i].nRefrac);
				DEBUG(4, "\n");
				
				for(j=0; j<ray[i].nRefrac; j++){
					temp2D[0][j] = ray[i].refrac[j].r;
					temp2D[1][j] = ray[i].refrac[j].z;
				}
				DEBUG(4, "\n");
				
				pRefrac = mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)ray[i].nRefrac, mxREAL);
				if(pRefrac == NULL){
					fatal("Memory alocation error.");
				}
				copyDoubleToPtr2D(temp2D, mxGetPr(pRefrac), ray[i].nRefrac,2);
				DEBUG(4, "\n");
				
				sprintf(string, "rayrfr%u", (uint32_t)(i+1));
				matPutVariable(matfile, (const char*)string, pRefrac);
				DEBUG(4, "\n");
				
				mxDestroyArray(pRefrac);
				free(temp2D[0]);
				free(temp2D[1]);
				free(temp2D);
				DEBUG(4, "\n");
			}
			if(KEEP_RAYS_IN_MEM == FALSE){
				//free the ray's memory
				reallocRayMembers(&ray[i],0);
			}
		}
	}
	DEBUG(4, "\n");

	/// All ray information to matfile:
	pRayInfo = mxCreateDoubleMatrix((MWSIZE)5, (MWSIZE)settings->source.nThetas, mxREAL);
	if(pRayInfo == NULL){
		fatal("Memory alocation error.");
	}
	DEBUG(4, "\n");
	copyDoubleToPtr2D(rayInfo, mxGetPr(pRayInfo), settings->source.nThetas,5);
	sprintf(string, "rayinfo");
	matPutVariable(matfile, (const char*)string, pRayInfo);
	mxDestroyArray(pRayInfo);
	DEBUG(4, "\n");
	
	matClose(matfile);
	free(string);
	DEBUG(1,"out\n");
}
