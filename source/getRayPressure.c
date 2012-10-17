/****************************************************************************************
 *  getRayPressure.c                                                                    *
 *  (formerly "gpress.for")                                                             *
 *  Calculates "ray pressure".                                                          *
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
 *  Inputs (when using "explicit" version):                                             *
 *          settings:   Pointer to the settings structure.                              *
 *          ray:        Pointer to structure containing a ray.                          *
 *          iHyd:       Index at which to interpolate.                                  *
 *          rHyd:       Range of hydrophone.                                            *
 *          zHyd:       Depth of hydrophone.                                            *
 *          tauRay      Ray's propagation delay at hydrophone.                          *
 *          zRay:       The ray's depth.                                                *
 *          dzdr:       Derivative of z in order to r (i.e. its Slope).                 *
 *          ampRay:     The ray's complex Amplitude.                                    *
 *          width:      Width of the beam.                                              *
 *                                                                                      *
 *  Inputs (when using short version):                                                  *
 *          ray:        Pointer to structure containing a ray.                          *
 *          iHyd:       Index at which to interpolate.                                  *
 *          q0:         TODO                                                            *
 *          rHyd:       Range of hydrophone.                                            *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          pressure:   The acoustic pressure at the hydrophone.                        *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include "dotProduct.c"
#include "getRayParameters.c"
#include "globals.h"
#include <complex.h>

void    getRayPressure(settings_t*, ray_t*, uintptr_t, float, float, float, complex float*);
void    getRayPressureExplicit(settings_t*, ray_t*, uintptr_t, float, float, float, float, complex float, float, complex float*);


void    getRayPressure(settings_t* settings, ray_t* ray, uintptr_t iHyd, float q0, float rHyd, float zHyd, complex float* pressure){
    DEBUG(4, "in\n");
    float          dzdr, tauRay, zRay, qRay, width;
    complex float  ampRay;
    
    getRayParameters(ray, iHyd, q0, rHyd, &dzdr, &tauRay, &zRay, &ampRay, &qRay, &width);
    DEBUG(5, "iHyd: %u, dzdr: %e; tauRay: %e; zRay: %e; ampRay: %e +j%e; qRay: %e; width %e;\n", (uint32_t)iHyd, dzdr, tauRay, zRay, creal(ampRay), cimag(ampRay), qRay, width);
    getRayPressureExplicit(settings, ray, iHyd, zHyd, tauRay, zRay, dzdr, ampRay, width, pressure);
    DEBUG(4, "out\n");
}


void    getRayPressureExplicit(settings_t* settings, ray_t* ray, uintptr_t iHyd, float zHyd, float tauRay, float zRay, float dzdr, complex float ampRay, float width, complex float* pressure){
    DEBUG(4, "in\n");
    float       omega, theta;
    vector_t    es = {0,0};
    vector_t    e1 = {0,0};
    vector_t    deltaR = {0,0};
    float       dr, dz, n, sRay;
    float       delay;
    float       phi;

    omega = 2 * M_PI * settings->source.freqx;
    theta = atan( dzdr );

    es.r = cos( theta );
    es.z = sin( theta );
    e1.r = -es.z;
    e1.z =  es.r;
    deltaR.r = 0.0;
    deltaR.z = zHyd - zRay;
    dr = ray->r[iHyd+1] - ray->r[iHyd];
    dz = ray->z[iHyd+1] - ray->z[iHyd];

    n = dotProduct(&deltaR, &e1);
    sRay = dotProduct(&deltaR, &es);

    n = fabs( n );
    sRay = sRay/sqrt( dr*dr + dz*dz );

    delay = tauRay + sRay*( ray->tau[iHyd+1] - ray->tau[iHyd] );
    if (n < width){
        phi = (width - n) / width;
        
        //Acoustic pressure a la Bellhop:
        *pressure = phi * cabs( ampRay )* cexp( -I*( omega*delay - ray->phase[iHyd] - ray->caustc[iHyd] ));
    }else{
        *pressure = 0 + 0*I;
    }
    DEBUG(5, "pressure: %e +j*%e\n", creal(*pressure), cimag(*pressure));
    DEBUG(4, "out\n");
}
