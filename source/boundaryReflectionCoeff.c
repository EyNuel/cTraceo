/****************************************************************************************
*   boundaryReflectionCoeff.c                                                           *
 *  (formerly "bdryr.for")                                                              *
 *  Calculates the reflection coefficient of a boudary between two media.               *
 *                                                                                      *
 *  Reference: "Recovery of the properties of an elastic bottom using reflection        *
 *  coefficient measurements", P.J.Papadakis et al, Proc. of the 2nd. ECUA, Vol II,     *
 *  page 943, 1994.                                                                     *
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
 *          TODO:   correct this:                                                       *
 *          aIn:    Atenuation value to be converted.                                   *
 *          lambda: Wavelength.                                                         *
 *          freq:   Frequency.                                                          *
 *          units:  Determines the input units.                                         *
 *                  (see globals.h for possible values)                                 *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          aOut:   Converted atenuation value.                                         *
 *                                                                                      *
 ****************************************************************************************/

#pragma  once
#include <complex.h>
#include <math.h>

void    boundaryReflectionCoeff(float*, float*, float*, float*, float*, float*, float*, float*, complex float*);

void    boundaryReflectionCoeff(float* rho1, float* rho2, float* cp1, float* cp2, float* cs2, float* ap,
                                float* as, float* theta, complex float* refl){

    float          tilap, tilas;
    float          a1;
    complex float  a2, a3, a4, a5, a6, a7, d;
    complex float  tilcp2, tilcs2;

    tilap = *ap/( 40.0 * M_PI * M_LOG10E );
    tilas = *as/( 40.0 * M_PI * M_LOG10E );

    tilcp2 = *cp2 * (1 - I * tilap) / (1 + tilap * tilap);
    tilcs2 = *cs2 * (1 - I * tilas) / (1 + tilas * tilas);

    a1  = *rho2 / *rho1;
    a2  = tilcp2 / *cp1;
    a3  = tilcs2 / *cp1;
    a4  = a3 * sin( *theta );
    a5  = 2*a4 * a4;
    a6  = a2 * sin( *theta );
    a7  = 2*a5 - a5*a5;

    d   = a1 * ( a2 * (1 - a7 ) / csqrt( 1 - a6 * a6 ) + a3 * a7 / csqrt(1 - 0.5*a5) );

    *refl = ( d * ccos( *theta ) -1) / ( d * ccos( *theta ) + 1);
}

