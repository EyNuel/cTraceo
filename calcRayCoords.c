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
	MATFile*	matfile	= NULL;
	mxArray*	pThetas	= NULL;
	mxArray*	pTitle	= NULL;
	mxArray*	pRay	= NULL;
	
	double		thetai, ctheta;
	ray_t*		ray		= NULL;
	double**	temp2D 	= NULL;
	uintptr_t	i,k;
	char* 		string	= mallocChar(10);
	
	matfile		= matOpen("rco.mat", "w");
	pThetas		= mxCreateDoubleMatrix(1, globals->settings.source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL)
		fatal("Memory alocation error.");
	
	//data		= mxGetPtr(thetas);
	//copy cArray to mxArray:
	copyDoubleToPtr(	globals->settings.source.thetas,
						mxGetPtr(thetas),
						globals->settings.source.nThetas);
	//move mxArray to file:
	status = matPutVariable(matfile, 'thetas', pThetas);

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
	for(i=0; i<globals->settings.source.nThetas); i++){
		thetai = -globals->settings.source.thetas[i] * M_PI/180.0;
		ctheta = abs( cos(thetai));
		
		//Trace a ray as long as it is neither 90 or -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(globals, ray[i]);
			//call seikeq(thetai,imax,irefl,decay,jbdry,tbdry)
			/*
			for k = 1,imax
				raydat(1,k) = r(k)
				raydat(2,k) = z(k)
			end do
			*/
			temp2D[0]	= ray[i].r;
			temp2D[1]	= ray[i].z;
			pRay		= mxCreateDoubleMatrix(2, ray[i]->nCoords, mxReal);
			if(pRay == NULL)
				fatal("Memory alocation error.");
				
			copyDoubleToPtr2D(temp2D, mxGetPtr(pRay), 2, ray[i].nCoords);
			//mxCopyReal8ToPtr(raydat,mxGetPr(pRay),2*imax)
			sprintf(string, "ray%d", i);
			matPutVariable(matfile, (const char*)buffer, pRay)
			mxDestroyArray(pRay)
		}
	}
	mxDestroyArray(thetas);
	mxDestroyArray(title);
	matClose(matfile)
}
