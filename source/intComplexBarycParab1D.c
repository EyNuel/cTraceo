/****************************************************************************************
 *  intComplexBarycParab1D.c                                                            *
 *  (formerly "cbpa1d.for")                                                             *
 *  Perform Barycentric Parabolic interpolation at 1D on complex numbers.               *
 *  NOTE:   this file is practically identical to intBarycParab1D.c, the only           *
 *          difference is that this one operates on complex values.                     *
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
 *          x:      vector containing x0, x1, x2 (note that this value is real)         *
 *          f:      vector containing f0, f1, f2                                        *
 *          xi:     scalar (the interpolation point)                                    *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          fi:     interpolated value of f at xi                                       *
 *          fxi:    1st derivative of f at xi                                           *
 *          fxxi:   2nd derivative of f at xi                                           *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include <complex.h>

void intComplexBarycParab1D(double*, complex double*, complex double, complex double*, complex double*, complex double*);

void intComplexBarycParab1D(double* x, complex double* f, complex double xi, complex double* fi, complex double* fxi, complex double* fxxi){
    complex double      a1,a2,px1,px2,sx1,sx2;

    //TODO remove calculation of second derivative -it is never used.
    px1 = (x[1] -x[0]) * (x[1] -x[2]);
    px2 = (x[2] -x[0]) * (x[2] -x[1]);

    a1 = ( f[1] - f[0] )/px1;
    a2 = ( f[2] - f[0] )/px2;

    px1 = (xi - x[0]) * (xi - x[2]);
    px2 = (xi - x[0]) * (xi - x[1]);

    sx1 = 2*(xi) - x[0] - x[2];
    sx2 = 2*(xi) - x[0] - x[1];

    *fi     = f[0]  +   a1*px1  +a2*px2;
    *fxi    =           a1*sx1  +a2*sx2;
    *fxxi   =           a1*2    +a2*2;
}
