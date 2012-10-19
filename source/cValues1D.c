/****************************************************************************************
 *  cValues1D.c                                                                         *
 *  (formerly "cvals1.for")                                                             *
 *  Perform 1-Dimensional Interpolation of sound speed and its derivatives.             *
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
 *          n:      number of elements in vectors x anc c.                              *
 *          xTable: vector containing independent variable.                             *
 *          cTable: vector containing dependent variable c(x)                           *
 *          xi:     interpolation point.                                                *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          ci:     interpolated value c(xi).                                           *
 *          cxi:    1st derivative of c at xi.                                          *
 *          cxxi:   2nd derivative of c at xi.                                          *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/
 
#pragma once
#include "interpolation.h"
#include "bracket.c"

void    cValues1D(uintptr_t, float*, float*, float, float*, float*, float*);

void    cValues1D(uintptr_t n, float* xTable, float* cTable, float xi, float* ci, float* cxi, float* cxxi){
    DEBUG(10,"Entering cValues1D().\n");
    uintptr_t   i = 0;

    //Note: the case that occurs most frequently is listed at top, so as to eliminate uneeded comparison operations.

    if( xi >= xTable[1] &&  xi <xTable[n-2]){
        //for all other cases do barycentric cubic interpolation
        bracket(n, xTable, xi, &i);
        intBarycCubic1D(    &xTable[i-1],
                            &cTable[i-1],
                            xi, ci, cxi, cxxi);
    
    }else if( xi < xTable[1]){
        //if xi is in first interval of xTable, do linear interpolation
        intLinear1D(    &xTable[0],
                        &cTable[0],
                        xi, ci, cxi);
        *cxxi = 0.0;
    
    }else if( xi >= xTable[n-2]){
        //if xi is in last interval of xTable, do linear interpolation
        intLinear1D(    &xTable[n-2],
                        &cTable[n-2],
                        xi, ci, cxi);
        *cxxi = 0.0;
    
    }else{
        printf("Interpolating sound speed at: %lf [m]\n", xi);
        fatal("in cValues1D(): Something is wrong.");
    }
    DEBUG(10,"Leaving cValues1D.\n");
}

