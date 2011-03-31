/********************************************************************************
 *	calcCohTransLoss.c	 														*
 * 	(formerly "calctl.for")														*
 *	Calculates Coherent Transmission Loss.										*
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
 * 				none:		Writes to file "ctl.mat".							*
 * 	Note:																		*
 * 		This function requires that the coherent acoustic pressure has been		*
 * 		calculated (by calcCohAcoustPress) and will of no use otherwise.		*
 * 																				*
 *******************************************************************************/

#include "globals.h"
#include <mat.h>
#include <matrix.h>
#include <math.h>
#include <complex.h>

void calcCohTransLoss(settings_t*);

void calcCohTransLoss(settings_t* settings){
	uint32_t	i, j, jj;
	double*		tl		= NULL;
	double**	tl2D	= NULL;
	MATFile*	matfile	= NULL;
	mxArray*	ptl		= NULL;
	mxArray*	ptl2D		= NULL;

	matfile		= matOpen("ctl.mat", "w");
	if(matfile == NULL){
		fatal("Memory alocation error.");
	}
	
	if (settings->output.arrayType != ARRAY_TYPE__RECTANGULAR){
		jj = (uint32_t)max((double)settings->output.nArrayR, (double)settings->output.nArrayZ);
		tl = mallocDouble(jj);
		
		for(j=0; j<jj; j++){
			tl[j] = -20.0*log10( cabs( settings->output.press1D[j] ) );
		}
		
		ptl = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)jj, mxREAL);
		if(ptl == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr(tl, mxGetPr(ptl), jj);
		matPutVariable(matfile,"tl",ptl);
		mxDestroyArray(ptl);

		free(tl);
	}else{
		tl2D = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		
		for(i=0; i<settings->output.nArrayZ; i++){
			for(j=0; j<settings->output.nArrayR; j++){
				tl2D[i][j] = -20.0*log10( cabs( settings->output.press2D[i][j] ) );
			}
		}
		
		ptl2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(ptl2D == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(tl2D, mxGetPr(ptl2D), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile,"tl",ptl2D);
		mxDestroyArray(ptl2D);
		
		freeDouble2D(tl2D, settings->output.nArrayZ);
	}
	
}
