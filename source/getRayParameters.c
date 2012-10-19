/****************************************************************************************
 *  getRayParameters.c                                                                  *
 *  (formerly "grayp.for")                                                              *
 *  interpolates ray parameters                                                         *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 * Website:                                                                             *
 *          https://github.com/EyNuel/cTraceo/wiki                                      *
 *                                                                                      *
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
 *          ray:    Pointer to structure containing a ray.                              *
 *          iHyd:   Index at which to interpolate.                                      *
 *          q0:     TODO                                                                *
 *          rHyd:   Range of hydrophone.                                                *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          dzdr:   Derivative of z in order to r (i.e. its Slope).                     *
 *          tauRay: The ray's propagation delay at hydrophone.                          *
 *          zRay:   The ray's depth.                                                    *
 *          ampRay: The ray's complex Amplitude.                                        *
 *          qRay:   TODO                                                                *
 *          width:  Width of the beam.                                                  *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/
 
#pragma once
#include "globals.h"
#include "interpolation.h"
#include <complex.h>
#include "tools.h"

void    getRayParameters(ray_t*, uintptr_t, double, double, double*, double*, double*, double complex*, double*, double*);

void    getRayParameters(ray_t* ray, uintptr_t iHyd, double q0, double rHyd, double* dzdr, double* tauRay, double* zRay, double complex* ampRay, double* qRay, double* width){

    complex double  junkComplex;
    double          junkDouble;
    double          theta;

    if( ray->iRefl[iHyd+1] == true){
        iHyd = iHyd - 1;
    }

    intLinear1D(        &ray->r[iHyd], &ray->z[iHyd],   rHyd, zRay,     dzdr);
    intLinear1D(        &ray->r[iHyd], &ray->tau[iHyd], rHyd, tauRay,   &junkDouble);
    DEBUG(7, "iHyd = %u: ampRay = %e + j*%e\n", (uint32_t)iHyd, creal(ray->amp[iHyd]), cimag(ray->amp[iHyd]));
    intComplexLinear1D( &ray->r[iHyd], &ray->amp[iHyd], rHyd, ampRay,   &junkComplex);
    intLinear1D(        &ray->r[iHyd], &ray->q[iHyd],   rHyd, qRay,     &junkDouble);

    theta = atan( *dzdr );
    *width = max( fabs( ray->q[iHyd] ), fabs( ray->q[iHyd+1]) );
    *width = *width / ((q0) * cos(theta));
}
