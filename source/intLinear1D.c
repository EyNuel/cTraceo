/****************************************************************************************
 *  intLinear1D.c                                                                       *
 *  (formerly "lini1d.for)                                                              *
 *  Perform linear interpolation at 1D.                                                 *
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
 *          x:      vector containing x0, x1                                            *
 *          f:      vector containing f0, f1                                            *
 *          xi:     scalar (the interpolation point)                                    *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          fi:     interpolated value of f at xi                                       *
 *          fxi:    derivative of f at xi                                               *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
/*
 * TODO write a version of this function which doesn't calculate the derivative, and 
 * which doesn't return the result through a pointer (so that it can be inlined by the
 * compiler, thus increasing performance).
 */
void intLinear1D(double*, double*, double, double*, double*);

void intLinear1D(double* x, double* f, double xi, double* fi, double* fxi){
    DEBUG(8,"in\n");
    *fxi    =   ( f[1] -f[0]) / ( x[1] -x[0]);
    *fi     =   f[0] +(xi -x[0]) *(*fxi);
    DEBUG(8,"out\n");
}
