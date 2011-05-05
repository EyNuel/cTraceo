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
	MATFile*			matfile		= NULL;
	mxArray*			pThetas		= NULL;
	mxArray*			pTitle		= NULL;
	mxArray*			mxR			= NULL;
	mxArray*			mxZ			= NULL;
	mxArray*			mxTau		= NULL;
	mxArray*			mxAmp		= NULL;
	mxArray*			mxRayStruct	= NULL;
	const char*			fieldNames[]= {"r", "z", "tau", "amp"};	//names of the fields contained in mxRayStruct
	mxArray*			pRefrac		= NULL;
	mxArray*			pRayInfo	= NULL;
	double				thetai, ctheta;
	ray_t*				ray			= NULL;
	double**	temp2D 		= NULL;
	double**			rayInfo 	= mallocDouble2D(5,settings->source.nThetas);
	uintptr_t			i, j;
	char* 				string	= mallocChar(10);


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
	
	//create mxStructArray:
	mxRayStruct = mxCreateStructMatrix(	(MWSIZE)settings->source.nThetas,	//number of rows
										(MWSIZE)1,		//number of columns
										4,				//number of fields in each element
										fieldNames);	//list of field names
	if( mxRayStruct == NULL ) {
		fatal("Memory Alocation error.");
	}
	
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
			/*NOTE:	when writing a mxArray to a mxStructArray, the mxArray cannot simply be reused after it was
			 *		copied to the mxStructArray, otherwise data corruption will occur.
			 *		Because of this, the variables mxR, mxZ, mxTau, mxAmp have to be allocated again at each pass.
			 *		Also note that these mxArrays shall not be deallocated, otherwise their content will be lost
			 * 		(even after they have been copied to the mxStruct.
			 */
			
			//create mxArrays:
			mxR	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)ray[i].nCoords, mxREAL);
			mxZ	= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)ray[i].nCoords, mxREAL);
			mxTau= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)ray[i].nCoords, mxREAL);
			mxAmp= mxCreateDoubleMatrix((MWSIZE)1,	(MWSIZE)ray[i].nCoords, mxCOMPLEX);
			if(	mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
				fatal("Memory alocation error.");
			}
			
			//copy data to mxArrays:
			copyDoubleToMxArray(ray[i].r,	mxR,	ray[i].nCoords);
			copyDoubleToMxArray(ray[i].z,	mxZ, 	ray[i].nCoords);
			copyDoubleToMxArray(ray[i].tau,	mxTau,	ray[i].nCoords);
			copyComplexToMxArray(ray[i].amp,mxAmp,	ray[i].nCoords);
			
			//copy mxArrays to mxRayStruct
			mxSetFieldByNumber(	mxRayStruct,		//pointer to the mxStruct
								(MWINDEX)i,			//index of the element (number of ray)
								0,					//position of the field (in this case, field 0 is "r"
								mxR);		//the mxArray we want to copy into the mxStruct
			mxSetFieldByNumber(	mxRayStruct, (MWINDEX)i, 1, mxZ);
			mxSetFieldByNumber(	mxRayStruct, (MWINDEX)i, 2, mxTau);
			mxSetFieldByNumber(	mxRayStruct, (MWINDEX)i, 3, mxAmp);
			
			//copy struct to file:
			matPutVariable(matfile, "rays", mxRayStruct);
			
			///ray has been written to matfile
/* TODO
			///save ray information:
			rayInfo[0][i] = (double)ray[i].iReturn;
			rayInfo[1][i] = ray[i].sRefl;
			rayInfo[2][i] = ray[i].bRefl;
			rayInfo[3][i] = ray[i].oRefl;
			rayInfo[4][i] = (double)ray[i].nRefrac;
			DEBUG(4, "Created temporary variables for reflections counters.\n");
*/
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
