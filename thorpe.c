/************************************************************************************
 *	thorpe.c		 																*
 * 	(formerly "thorpe.for")															*
 * 	Calculate Thorpe attenuation. (reference: Bellhop)								*
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
 * 				freq	Frequency.													*
 * 	Outputs:																		*
 * 				alpha:	Thorpe attenuation in dB/m.									*
 * 																					*
 ***********************************************************************************/
void	thorpe(float, float*);

void	thorpe(float freq, float* alpha){
	float		fxf;

	fxf		= pow( (freq/1000 ), 2);
	*alpha	= 0.0033 +0.11 * fxf / (1 +fxf ) + 44 * fxf / (4100 +fxf) +0.0003 * fxf;
	*alpha	= *alpha/8685.8896;
}
