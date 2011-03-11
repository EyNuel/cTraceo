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
#include	"cValues2D.c"

void	csValues(settings_t*, float, float, float*, float*, float*, float*, float*,
				vector_t*, float*, float*, float*);

void	csValues(settings_t* settings, float ri, float zi, float* ci, float* cc, float* si, float* cri, float* czi,
				vector_t* slowness, float* crri, float* czzi, float* crzi){
	DEBUG(8,"csValues(),\t in\n");
	
	float 		k,a,eta, root, root32, root52;
	float*		c01d;	//used locally to make code more readable
	float**		c02d;	//used locally to make code more readable
	float*		r0;		//used locally to make code more readable
	float*		z0;		//used locally to make code more readable
	float		epsilon, bmunk, bmunk2;

	epsilon = (float)7.4e-3;
	bmunk  = 1300.0;
	bmunk2 = bmunk*bmunk;

	c01d = settings->soundSpeed.c01d;
	c02d = settings->soundSpeed.c02d;
	r0 =  settings->soundSpeed.r0;
	z0 =  settings->soundSpeed.z0;
	
	switch(settings->soundSpeed.cDist){
		case C_DIST__PROFILE:
			//in the case of soundspeed profiles, all derivatives with respect to range are 0:
			*cri = 0;
			*crri = 0;
			*crzi = 0;
			switch(settings->soundSpeed.cClass){
				///	*****	analytical sound speed profiles		*****
				case C_CLASS__ISOVELOCITY:			//"ISOV"
					*ci = c01d[0];
					*czi = 0;
					*czzi = 0;
					break;
					
				case C_CLASS__LINEAR:				//"LINP"
					k	= ( c01d[1] - c01d[0] ) / ( z0[1] - z0[0]);
					*ci	= c01d[0] + k*( zi - z0[0] );
					*czi = k;
					*czzi= 0;
					break;
					
				case C_CLASS__PARABOLIC:			//"PARP"
					k	= ( c01d[1] - c01d[0] ) / powf( ( z0[1] - z0[0]), 2);
					*ci	= c01d[0] + k * powf(( zi - z0[0] ), 2);
					*czi = 2*k*( zi - z0[0] );
					*czzi= 2*k;
					break;
					
				case C_CLASS__EXPONENTIAL:			//"EXPP"
					k	= logf( c01d[0]/c01d[1] )/( z0[1] - z0[0] );
					*ci	= c01d[0]*expf( -k*(zi - z0[0]) );
					*czi	= -k * (*ci);
					*czzi= k*k * (*ci);
					break;
					
				case C_CLASS__N2_LINEAR:			//"N2LP"
					k		= ( powf( c01d[0]/c01d[1] ,2) -1) / ( z0[1] - z0[0] );
					root	= sqrtf( 1 + k*( zi - z0[0] ) );
					root32	= powf(root, 3/2 );
					root52	= powf(root, 5/2 );
					*ci 	= c01d[0]/sqrtf( 1 + k*( zi - z0[0] ));
					*czi 	= -k*c01d[0]/( 2*root32 );
					*czzi 	= 3*k*k*c01d[0]/( 4*root52 );
					break;
					
				case C_CLASS__INV_SQUARE:			//"ISQP"
					a		= powf(( c01d[1]/c01d[0]) -1 , 2);
					root 	= sqrtf( a/(1-a) );
					k 		= root/( z0[1] - z0[0] );
					root 	= sqrtf( 1 + powf( k*( zi - z0[0] ),2) );
					root32 	= powf(root, 3/2 );
					root52 	= powf(root, 5/2 );
					*ci 	= c01d[0] * ( 1 + k*( zi - z0[0] )/root );
					*czi 	= c01d[0] * k / root32;
					*czzi 	= -3 * c01d[0] * powf(k,3) / root52;
					break;
					
				case C_CLASS__MUNK:					//"MUNK"
					eta 	= 2*( zi - z0[0] )/bmunk;
					*ci 	= c01d[0]*( 1 + epsilon*( eta + expf(-eta) - 1 ) );
					*czi 	= 2*epsilon * c01d[0]*( 1 - expf(-eta) )/bmunk;
					*czzi	= 4*epsilon * c01d[0]*expf( -eta )/bmunk2;
					break;
					
				case C_CLASS__TABULATED:			//"TABL"
					//cValues1D(uintptr_t n, float* xTable, float* cTable, float* xi, float* ci, float* cxi, float* cxxi){
					cValues1D( settings->soundSpeed.nz0, z0, c01d, zi, ci, czi, czzi);
					break;
					
				default:
					fatal("Unknown sound speed profile.\nAborting...");
			}
			break;
		case C_DIST__FIELD:
			///	*****	tabulated sound speed fields		*****
			cValues2D(settings->soundSpeed.nr0,settings->soundSpeed.nz0,r0,z0,c02d,ri,zi,ci,cri,czi,crri,czzi,crzi);
			break;
			
		default:
			fatal("Unknown sound speed distribution.\nAborting...");
	}

	*cc = powf(*ci,2);
	*si =  1.0/(*ci);
	slowness->r = -(*cri) / (*cc);
	slowness->z = -(*czi) / (*cc);
	
	DEBUG(8,"csValues(),\t out\n");
}

