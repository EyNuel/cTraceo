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
 * 				"rco.mat":	A file containing the coordinates of all rays.			*
 * 																					*
 ***********************************************************************************/

#pragma  once
#include <mat.h>
#include "matrix.h"
#include "tools.c"
#include <math.h>
#include "solveEikonalEq.c"

void	calcRayCoords(settings_t*);

void	calcRayCoords(settings_t* settings){
	DEBUG(1,"in\n");

	MATFile*	matfile	= NULL;
	mxArray*	pThetas	= NULL;
	mxArray*	pTitle	= NULL;
	mxArray*	pRay	= NULL;
	
	double		thetai, ctheta;
	ray_t*		ray		= NULL;
	double**	temp2D 	= malloc(2*sizeof(uintptr_t));
	uintptr_t	i;
	char* 		string	= mallocChar(10);

	matfile		= matOpen("rco.mat", "w");
	pThetas		= mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL)
		fatal("Memory alocation error.");
	
	//copy cArray to mxArray:
	copyDoubleToPtr(	settings->source.thetas,
						mxGetPr(pThetas),
						settings->source.nThetas);
	//move mxArray to file:
	matPutVariable(matfile, "thetas", pThetas);

	//write title to matfile:
	pTitle = mxCreateString("TRACEO: Ray COordinates");
	if(pTitle == NULL)
		fatal("Memory alocation error.");
	matPutVariable(matfile, "caseTitle", pTitle);

	//allocate the rays:
	ray = makeRay(settings->source.nThetas);

	/* Trace the rays:	*/
	for(i=0; i<settings->source.nThetas; i++){
		thetai = -settings->source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		DEBUG(2,"ray[%u].theta: %lf\n", (uint32_t)i, settings->source.thetas[i]);
		ctheta = fabs( cos(thetai));
		
		//Trace a ray as long as it is neither 90 or -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			
			temp2D[0]	= ray[i].r;
			temp2D[1]	= ray[i].z;
			pRay		= mxCreateDoubleMatrix((MWSIZE)2, (int32_t)ray[i].nCoords, mxREAL);
			if(pRay == NULL)
				fatal("Memory alocation error.");
			copyDoubleToPtr2D(temp2D, mxGetPr(pRay), ray[i].nCoords,2);

			sprintf(string, "ray%u", (uint32_t)(i+1));
			matPutVariable(matfile, (const char*)string, pRay);
			mxDestroyArray(pRay);
			if(KEEP_RAYS_IN_MEM == FALSE){
				//free the ray's memory
				reallocRayMembers(&ray[i],0);
			}
		}//if (ctheta > 1.0e-7)
	}//for(i=0; i<settings->source.nThetas; i++)
	mxDestroyArray(pThetas);
	mxDestroyArray(pTitle);
	matClose(matfile);
	free(temp2D);
	free(string);
	free(ray);
	DEBUG(1,"out\n");
}
