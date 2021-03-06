/****************************************************************************************
 *  intBarycParab2D.c                                                                   *
 *  (formerly "bpai1d.for")                                                             *
 *  Perform 2D piecewise Barycentric Parabolic interpolation.                           *
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
 *          Copyright (C) 2011 - 2013                                                   *
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
 *          x:      vector containing x0, x1, x2.                                       *
 *          y:      vector containing y0, y1, y2.                                       *
 *          f:      vector containing f(x,y).                                           *
 *          xi:     x coordinate of the interpolation point.                            *
 *          xy:     y coordinate of the interpolation point.                            *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          fi:     interpolated value of f at xi                                       *
 *          fxi:    1st derivative of f in order to x.                                  *
 *          fyi:    1st derivative of f in order to y.                                  *
 *          fxxi:   2nd derivative of f in order to x.                                  *
 *          fyyi:   2nd derivative of f in order to y.                                  *
 *          fxyi:   2nd derivative of f in order to x and y.                            *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include "tools.h"

void intBarycParab2D(double*, double*, double**, double, double, double*, double*, double*, double*, double*, double*);

void intBarycParab2D(double* x, double* y, double** f, double xi, double yi, double* fi, double* fxi, double* fyi, double* fxxi, double* fyyi, double* fxyi){
    DEBUG(8, "in\n");

    double      px[3];
    double      py[3];
    double      sx[3];
    double      sy[3];
    double      a[3][3];
    uintptr_t   i, j;

    
    px[0] = ( x[0] - x[1] )*( x[0] - x[2] );
    DEBUG(10,"px[0]\t= ( x[0] - x[1] )*( x[0] - x[2] )\n");
    DEBUG(10,"%lf\t= ( %lf - %lf )*( %lf - %lf )\n", px[0], x[0], x[1], x[0], x[2] );
    px[1] = ( x[1] - x[0] )*( x[1] - x[2] );
    px[2] = ( x[2] - x[0] )*( x[2] - x[1] );
    
    py[0] = ( y[0] - y[1] )*( y[0] - y[2] );
    py[1] = ( y[1] - y[0] )*( y[1] - y[2] );
    py[2] = ( y[2] - y[0] )*( y[2] - y[1] );
    
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            a[i][j] = f[i][j] / ( px[j] * py[i] );
            DEBUG(10, "i,j: %u,%u => a= %e = {f[i][j] = %lf} / ( {px[j] = %lf} * {py[i] = %lf}\n", (uint32_t)i, (uint32_t)j, a[i][j], f[i][j], px[j], py[i]);
        }
    }

    px[0] = ( xi - x[1] )*( xi - x[2] );
    px[1] = ( xi - x[0] )*( xi - x[2] );
    px[2] = ( xi - x[0] )*( xi - x[1] );
    
    py[0] = ( yi - y[1] )*( yi - y[2] );
    py[1] = ( yi - y[0] )*( yi - y[2] );
    py[2] = ( yi - y[0] )*( yi - y[1] );
    
    sx[0] = 2.0* (xi) - x[1] - x[2];
    sx[1] = 2.0* (xi) - x[0] - x[2];
    sx[2] = 2.0* (xi) - x[0] - x[1];
    
    sy[0] = 2.0* (yi) - y[1] - y[2];
    sy[1] = 2.0* (yi) - y[0] - y[2];
    sy[2] = 2.0* (yi) - y[0] - y[1];

    *fi     = 0;
    *fxi    = 0;
    *fyi    = 0;
    *fxxi   = 0;
    *fyyi   = 0;
    *fxyi   = 0;

    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            *fi     += a[i][j] * px[j] * py[i];
            *fxi    += a[i][j] * sx[j] * py[i];
            *fyi    += a[i][j] * px[j] * sy[i];
            *fxxi   += a[i][j] * 2 * py[i];
            *fyyi   += a[i][j] * 2 * px[j];
            *fxyi   += a[i][j] * sx[j] * sy[i];
        }
    }

    DEBUG(8, "out\n");
}
