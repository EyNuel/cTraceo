/****************************************************************************************
 *  csValues.c                                                                          *
 *  (formerly "csvals.for")                                                             *
 *  Perform Interpolation of soundspeed and slowness, and its derivatives.              *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 * License: This file is part of the cTraceo Raytracing Model and is released under the *
 *          Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License  *
 *          http://creativecommons.org/licenses/by-nc-sa/3.0/                           *
 *                                                                                      *
 * NOTE:    cTraceo is research code under active development.                          *
 *          The code may contain bugs and updates are possible in the future.           *
 *                                                                                      *
 * Written for project SENSOCEAN by:                                                    *
 *          Emanuel Ey                                                                  *
 *          emanuel.ey@gmail.com                                                        *
 *          Copyright (C) 2011                                                          *
 *          Signal Processing Laboratory                                                *
 *          Universidade do Algarve                                                     *
 *                                                                                      *
 * cTraceo is the C port of the FORTRAN 77 TRACEO code written by:                      *
 *          Orlando Camargo Rodriguez:                                                  *
 *          Copyright (C) 2010                                                          *
 *          Orlando Camargo Rodriguez                                                   *
 *          orodrig@ualg.pt                                                             *
 *          Universidade do Algarve                                                     *
 *          Physics Department                                                          *
 *          Signal Processing Laboratory                                                *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *  Inputs:                                                                             *
 *          settings:   Pointer to structure containing all input info.                 *
 *          ri:         range of interpolation point.                                   *
 *          zi:         depth of interpolation point.                                   *
 *  Outputs:                                                                            *
 *          ci:         interpolated value of c.                                        *
 *          cc:         ci squared.                                                     *
 *          si:         inverse of ci. ( 1/ci = sigmai)                                 *
 *          cri:        1st derivative of c with respect to r.                          *
 *          czi:        1st derivative of c with respect to z.                          *
 *          slowness:   slowness vector at interpolation point.                         *
 *          crri:       2nd derivative of c with respect to r.                          *
 *          czzi:       2nd derivative of c with respect to z.                          *
 *          crzi:       partial derivative of c with respect to z and r                 *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include    "globals.h"
#include    "math.h"
#include    "cValues1D.c"
#include    "cValues2D.c"

void    csValues(settings_t*, double, double, double*, double*, double*, double*, double*,
                vector_t*, double*, double*, double*);

void    csValues(settings_t* settings, double ri, double zi, double* ci, double* cc, double* si, double* cri, double* czi,
                vector_t* slowness, double* crri, double* czzi, double* crzi){
    DEBUG(8,"csValues(),\t in\n");
    
    double      k,a,eta, root, root32, root52;
    double*     c1D;    //used locally to make code more readable
    double**    c2D;    //used locally to make code more readable
    double*     r;      //used locally to make code more readable
    double*     z;      //used locally to make code more readable
    double      epsilon, bmunk, bmunk2;

    epsilon = 7.4e-3;
    bmunk  = 1300.0;
    bmunk2 = bmunk*bmunk;

    c1D = settings->soundSpeed.c1D;
    c2D = settings->soundSpeed.c2D;
    r =  settings->soundSpeed.r;
    z =  settings->soundSpeed.z;
    
    switch(settings->soundSpeed.cDist){
        case C_DIST__PROFILE:
            //in the case of soundspeed profiles, all derivatives with respect to range are 0:
            *cri = 0;
            *crri = 0;
            *crzi = 0;
            switch(settings->soundSpeed.cClass){
                /// *****   analytical sound speed profiles     *****
                case C_CLASS__ISOVELOCITY:          //"ISOV"
                    *ci = c1D[0];
                    *czi = 0;
                    *czzi = 0;
                    break;
                    
                case C_CLASS__LINEAR:               //"LINP"
                    k   = ( c1D[1] - c1D[0] ) / ( z[1] - z[0]);
                    *ci = c1D[0] + k*( zi - z[0] );
                    *czi = k;
                    *czzi= 0;
                    break;
                    
                case C_CLASS__PARABOLIC:            //"PARP"
                    k   = ( c1D[1] - c1D[0] ) / pow( ( z[1] - z[0]), 2);
                    *ci = c1D[0] + k * pow(( zi - z[0] ), 2);
                    *czi = 2*k*( zi - z[0] );
                    *czzi= 2*k;
                    break;
                    
                case C_CLASS__EXPONENTIAL:          //"EXPP"
                    k   = log( c1D[0]/c1D[1] )/( z[1] - z[0] );
                    *ci = c1D[0]*exp( -k*(zi - z[0]) );
                    *czi    = -k * (*ci);
                    *czzi= k*k * (*ci);
                    break;
                    
                case C_CLASS__N2_LINEAR:            //"N2LP"
                    k       = ( pow( c1D[0]/c1D[1] ,2) -1) / ( z[1] - z[0] );
                    root    = sqrt( 1 + k*( zi - z[0] ) );
                    root32  = pow(root, 3/2 );
                    root52  = pow(root, 5/2 );
                    *ci         = c1D[0]/sqrt( 1 + k*( zi - z[0] ));
                    *czi    = -k*c1D[0]/( 2*root32 );
                    *czzi   = 3*k*k*c1D[0]/( 4*root52 );
                    break;
                    
                case C_CLASS__INV_SQUARE:           //"ISQP"
                    a       = pow(( c1D[1]/c1D[0]) -1 , 2);
                    root    = sqrt( a/(1-a) );
                    k       = root/( z[1] - z[0] );
                    root    = sqrt( 1 + pow( k*( zi - z[0] ),2) );
                    root32  = pow(root, 3/2 );
                    root52  = pow(root, 5/2 );
                    *ci         = c1D[0] * ( 1 + k*( zi - z[0] )/root );
                    *czi    = c1D[0] * k / root32;
                    *czzi   = -3 * c1D[0] * pow(k,3) / root52;
                    break;
                    
                case C_CLASS__MUNK:                 //"MUNK"
                    eta     = 2*( zi - z[0] )/bmunk;
                    *ci         = c1D[0]*( 1 + epsilon*( eta + exp(-eta) - 1 ) );
                    *czi    = 2*epsilon * c1D[0]*( 1 - exp(-eta) )/bmunk;
                    *czzi   = 4*epsilon * c1D[0]*exp( -eta )/bmunk2;
                    break;
                    
                case C_CLASS__TABULATED:            //"TABL"
                    //cValues1D(uintptr_t n, double* xTable, double* cTable, double* xi, double* ci, double* cxi, double* cxxi){
                    cValues1D( settings->soundSpeed.nz, z, c1D, zi, ci, czi, czzi);
                    break;
                    
                default:
                    fatal("Unknown sound speed profile.\nAborting...");
            }
            break;
        case C_DIST__FIELD:
            /// *****   tabulated sound speed fields        *****
            cValues2D(settings->soundSpeed.nr,settings->soundSpeed.nz,r,z,c2D,ri,zi,ci,cri,czi,crri,czzi,crzi);
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

