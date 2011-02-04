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
 *	Ported to C by:			Emanuel Ey												*
 *							emanuel.ey@gmail.com									*
 *							Signal Processing Laboratory							*
 *							Universidade do Algarve									*
 *																					*
 *	Inputs:																			*
 * 				globals		All input information.									*
 * 	Outputs:																		*
 * 				"rco.mat":	A file containing the coordinates of all rays.			*
 * 																					*
 ***********************************************************************************/

#include "mat.h"
#include "matrix.h"
#include "tools.c"
#include "math.h"
#include "solveEikonalEq.c"

void	calcRayCoords(globals_t*);

void	calcRayCoords(globals_t* globals){
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
/*
for(i=0; i<2; i++){		//TODO remove this
	printf("globals->settings.altimetry.r[%lu]: %lf\n", i, globals->settings.altimetry.r[i]);
	printf("globals->settings.altimetry.z[%lu]: %lf\n", i, globals->settings.altimetry.z[i]);
}
*/
	matfile		= matOpen("rco.mat", "w");
	pThetas		= mxCreateDoubleMatrix(1, (int32_t)globals->settings.source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL)
		fatal("Memory alocation error.");
	
	//data		= mxGetPtr(thetas);
	//copy cArray to mxArray:
	copyDoubleToPtr(	globals->settings.source.thetas,
						mxGetPr(pThetas),
						globals->settings.source.nThetas);
	//move mxArray to file:
	matPutVariable(matfile, "thetas", pThetas);

	//write title to matfile:
	pTitle = mxCreateString("TRACEO: Ray COordinates");
	if(pTitle == NULL)
		fatal("Memory alocation error.");
	matPutVariable(matfile, "caseTitle", pTitle);

	//allocate the rays:
	ray = malloc(globals->settings.source.nThetas * sizeof(ray_t));
	if(ray == NULL)
		fatal("Memory alocation error.");

	/* Trace the rays:	*/
	for(i=0; i<globals->settings.source.nThetas; i++){
		thetai = -globals->settings.source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		ctheta = fabs( cos(thetai));
		
		//Trace a ray as long as it is neither 90 or -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(globals, &ray[i]);
			temp2D[0]	= ray[i].r;
			temp2D[1]	= ray[i].z;
			pRay		= mxCreateDoubleMatrix(2, (int32_t)ray[i].nCoords, mxREAL);
			if(pRay == NULL)
				fatal("Memory alocation error.");
			copyDoubleToPtr2D(temp2D, mxGetPr(pRay), ray[i].nCoords,2);

			sprintf(string, "ray%lu", i+1);
			matPutVariable(matfile, (const char*)string, pRay);
			mxDestroyArray(pRay);
		}
	}
	mxDestroyArray(pThetas);
	mxDestroyArray(pTitle);
	matClose(matfile);
	free(temp2D);
	DEBUG(1,"out\n");
}
