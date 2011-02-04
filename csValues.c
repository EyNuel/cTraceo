/************************************************************************************
 *	csValues.c		 																*
 * 	(formerly "csvals.for")															*
 * 	Perform Interpolation of soundspeed and slowness, and its derivatives.			*
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
 * 				ri:			range of interpolation point.							*
 * 				zi:			depth of interpolation point.							*
 * 	Outputs:																		*
 * 				ci:			interpolated value of c.								*
 * 				cc:			ci squared.												*
 * 				si:			inverse of ci. ( 1/ci = sigmai)							*
 * 				cri:		1st derivative of c with respect to r.					*
 * 				czi:		1st derivative of c with respect to z.					*
 * 				slowness:	slowness vector at interpolation point.					*
 * 				crri:		2nd derivative of c with respect to r.					*
 * 				czzi:		2nd derivative of c with respect to z.					*
 * 				crzi:		partial derivative of c with respect to z and r			*
 * 																					*
 ***********************************************************************************/
#pragma once
#include	"globals.h"
#include	"math.h"
#include	"cValues1D.c"
//#include	"cValues2D.c"

void	csValues(globals_t*, double*, double*, double*, double*, double*, double*, double*,
				vector_t*, double*, double*, double*);

void	csValues(globals_t* globals, double* ri, double* zi, double* ci, double* cc, double* si, double* cri, double* czi,
				vector_t* slowness, double* crri, double* czzi, double* crzi){
	DEBUG(8,"csValues(),\t in\n");
	
	double 		k,a,eta, root, root32, root52;
	double*		c01d;	//used locally to make code more readable
	double*		r0;		//used locally to make code more readable
	double*		z0;		//used locally to make code more readable
	double		epsilon, bmunk, bmunk2;

	epsilon = 7.4e-3;
	bmunk  = 1300.0;
	bmunk2 = bmunk*bmunk;

	c01d = globals->settings.soundSpeed.c01d;
	r0 =  globals->settings.soundSpeed.r0;
	z0 =  globals->settings.soundSpeed.z0;
	
	switch(globals->settings.soundSpeed.cDist){
		case C_DIST__PROFILE:
			//in the case of soundspeed profiles, all derivatives with respect to range are 0:
			*cri = 0;
			*crri = 0;
			*crzi = 0;
			switch(globals->settings.soundSpeed.cClass){
				///	*****	analytical sound speed profiles		*****
				case C_CLASS__ISOVELOCITY:			//"ISOV"
					*ci = c01d[0];
					*czi = 0;
					*czzi = 0;
					break;
					
				case C_CLASS__LINEAR:				//"LINP"
					k	= ( c01d[1] - c01d[0] ) / ( z0[1] - z0[0]);
					*ci	= c01d[0] + k*( *zi - z0[0] );
					*czi = k;
					*czzi= 0;
					break;
					
				case C_CLASS__PARABOLIC:			//"PARP"
					k	= ( c01d[1] - c01d[0] ) / pow( ( z0[1] - z0[0]), 2);
					*ci	= c01d[0] + k * pow(( *zi - z0[0] ), 2);
					*czi = 2*k*( *zi - z0[0] );
					*czzi= 2*k;
					break;
					
				case C_CLASS__EXPONENTIAL:			//"EXPP"
					k	= log( c01d[0]/c01d[1] )/( z0[1] - z0[0] );
					*ci	= c01d[0]*exp( -k*(*zi - z0[0]) );
					*czi	= -k * (*ci);
					*czzi= k*k * (*ci);
					break;
					
				case C_CLASS__N2_LINEAR:			//"N2LP"
					k		= ( pow( c01d[0]/c01d[1] ,2) -1) / ( z0[1] - z0[0] );
					root	= sqrt( 1 + k*( *zi - z0[0] ) );
					root32	= pow(root, 3/2 );
					root52	= pow(root, 5/2 );
					*ci 		= c01d[0]/sqrt( 1 + k*( *zi - z0[0] ));
					*czi 	= -k*c01d[0]/( 2*root32 );
					*czzi 	= 3*k*k*c01d[0]/( 4*root52 );
					break;
					
				case C_CLASS__INV_SQUARE:			//"ISQP"
					a		= pow(( c01d[1]/c01d[0]) -1 , 2);
					root 	= sqrt( a/(1-a) );
					k 		= root/( z0[1] - z0[0] );
					root 	= sqrt( 1 + pow( k*( *zi - z0[0] ),2) );
					root32 	= pow(root, 3/2 );
					root52 	= pow(root, 5/2 );
					*ci 		= c01d[0] * ( 1 + k*( *zi - z0[0] )/root );
					*czi 	= c01d[0] * k / root32;
					*czzi 	= -3 * c01d[0] * pow(k,3) / root52;
					break;
					
				case C_CLASS__MUNK:					//"MUNK"
					eta 	= 2*( *zi - z0[0] )/bmunk;
					*ci 		= c01d[0]*( 1 + epsilon*( eta + exp(-eta) - 1 ) );
					*czi 	= 2*epsilon * c01d[0]*( 1 - exp(-eta) )/bmunk;
					*czzi	= 4*epsilon * c01d[0]*exp( -eta )/bmunk2;
					break;
					
				case C_CLASS__TABULATED:			//"TABL"
					//cValues1D(uintptr_t n, double* xTable, double* cTable, double* xi, double* ci, double* cxi, double* cxxi){
					cValues1D( globals->settings.soundSpeed.nz0, z0, c01d, zi, ci, czi, czzi);
					break;
					
				default:
					fatal("Unknown sound speed profile.\nAborting...");
			}
			break;
		case C_DIST__FIELD:
			//cValues2D(globals->settings.soundSpeed.nr0,globals->settings.soundSpeed.nz0,r0,z0,c02d,ri,zi,ci,cri,czi,crri,czzi,crzi)
			break;
			
		default:
			fatal("Unknown sound speed distribution.\nAborting...");
	}

	*cc = pow(*ci,2);
	*si =  1.0/(*ci);
	slowness->r = -(*cri) / (*cc);
	slowness->z = -(*czi) / (*cc);
	
	DEBUG(8,"csValues(),\t out\n");
}

