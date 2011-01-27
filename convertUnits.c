/************************************************************************************
 *	convertUnits.c																	*
 *	(formerly "cnvnts.for")															*
 *	Convert between several atenuation units.										*
 * 																					*
 *	originally written in FORTRAN by:												*
 *						Orlando Camargo Rodriguez:									*
 *						Copyright (C) 2010											*
 *						Orlando Camargo Rodriguez									*
 *						orodrig@ualg.pt												*
 *						Universidade do Algarve										*
 *						Physics Department											*
 *						Signal Processing Laboratory								*
 *																					*
 *	Ported to C for project SENSOCEAN by:											*
 * 						Emanuel Ey													*
 *						emanuel.ey@gmail.com										*
 *						Signal Processing Laboratory								*
 *						Universidade do Algarve										*
 *																					*
 *	Inputs:																			*
 * 				aIn:	Atenuation value to be converted.							*
 * 				lambda:	Wavelength.													*
 * 				freq:	Frequency.													*
 * 				units:	Determines the input units.									*
 * 						(see globals.h for possible values)							*
 *																					*
 * 	Outputs:																		*
 * 				aOut:	Converted atenuation value.									*
 * 																					*
 ***********************************************************************************/

#include "globals.h"
#include "tools.h"
#include <math.h>

void convertUnits(double*, double*, double*, uint32_t, double*);
	
void convertUnits(double* aIn, double* lambda, double* freq, uint32_t units, double* aOut){
	
	double c1 = 8.68588963806504;
	//TODO double c2 = 8685.88963806504;
	
	switch(units){
		case	SURFACE_ATTEN_UNITS__dBperkHz:		//"F",	dB/kHz
			*aOut = *aIn * (*lambda) * (*freq) * 1.0e-3;
			break;
			
		case	SURFACE_ATTEN_UNITS__dBperMeter:	//"M",	dB/meter
			*aOut = (*aIn) * (*lambda);
			break;
			
		case	SURFACE_ATTEN_UNITS__dBperNeper:	//"N",	dB/neper
			*aOut = (*aIn) * (*lambda) * c1;
			break;
			
		case	SURFACE_ATTEN_UNITS__qFactor:		//"Q",	Q factor
			//Avoid divide-by-zero
			if (*aIn == 0.0){
				*aOut = 0.0;
			}else{
				*aOut = c1 * M_PI / (*aIn);
			}
			break;
			
		case	SURFACE_ATTEN_UNITS__dBperLambda:	//"W",	dB/<lambda>
			*aOut = *aIn;
			break;
			
		default:
			fatal("Boundary attenuation: unknown units.\nAborting...");
			break;
	}
}
