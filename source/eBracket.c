/****************************************************************************************
 *  eBracket.c                                                                          *
 *  (formerly "ebrckt.for")                                                             *
 *  Extended bracket -bracketing with non-uniformly ordered data.                       *
 *  Given a sequence x0 ... < xn, and a number xi, such that:                           *
 *       x0 < xi < xn,                                                                  *
 *  with xi not belonging to the sequence, determine j such that:                       *
 *       xj < xi < x[j+1]                                                               *
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
 *          n:      number of elements in vector x                                      *
 *          x:      vector to be searched                                               *
 *          xi:     scalar who's bracketing elements are to be found                    *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          nb:     number of bracketing pairs found.                                   *
 *          ib:     the indexes of the lower bracketing elements.                       *
 *                  Note: shall be previously allocated.                                *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma     once
#include    "tools.h"

void    eBracket(uintptr_t, float*, float, uintptr_t*, uintptr_t*);

void    eBracket(uintptr_t n, float* x, float xi, uintptr_t* nb, uintptr_t* ib){
    uintptr_t   i;
    float_t    a, b;
    
    ib[0]   = 0;
    *nb     = 0;
    
    for(i=0; i< n-2; i++){
        a = min( x[i], x[i+1]);
        b = max( x[i], x[i+1]);
        if ( (xi >= a) && (xi < b) && (*nb < 50)){
            ib[*nb] = i;
            *nb = *nb +1;
            DEBUG(5,"Bracketing point found at index: %u\n", (uint32_t)i);
        }
    }
}
