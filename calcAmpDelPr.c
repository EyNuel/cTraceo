/********************************************************************************
 *	calcAmpDelPr.c		 														*
 * 	(formerly "caladp.for")														*
 *	Calculates Amplitudes and arrivals using proximity method.					*
 *	Note that in fact this is simply a matter of finding the eigenrays and		*
 *	outputting their amplitudes and arrivals as a stem plot, so the only		*
 *	difference is in the m-files that interprets the results.					*
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
 * 				none:		Writes to file "eig.mat".							*
 * 																				*
 *******************************************************************************/

#include "globals.h"
#include "calcEigenrayPr.c"

void calcAmpDelPr(settings_t*);

void calcAmpDelPr(settings_t* settings){
	//TODO write something like Bellhop's .arr file, containing only the actual amplitudes and delays of the eigenrays.
	calcEigenrayPr(settings);
}
