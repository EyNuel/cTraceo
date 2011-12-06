/****************************************************************************************
 *  bracket.c                                                                           *
 *  (formerly "brcket.for")                                                             *
 *  Given a sequence x0 < x1 < ... <xj < ... < xn, and a number xi, such that:          *
 *       x0 < xi < xn,                                                                  *
 *  with xi not belonging to the sequence, determine j such that:                       *
 *       xj < xi < x[j+1]                                                               *
 *                                                                                      *
 *  Used to determine between which values to interpolate.                              *
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
 * Based on previous work done in FORTRAN 77 by:                                        *
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
 *          n:  number of elements in vector x                                          *
 *          x:  vector to be searched                                                   *
 *          xi: scalar who's bracketing elements are to be found                        *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          i:  the index of the lower bracketing element.                              *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          0:  Bounding error, don't use the returned index "i" -this will             *
 *              likely result in a segfault if you use it.                              *
 *          1:  Success. Retuned index "i" is valid and safe to use.                    *
 *                                                                                      *
 ****************************************************************************************/

#pragma  once
#include <stdint.h>

uintptr_t   bracket(uintptr_t, double*, double, uintptr_t*);

uintptr_t   bracket(uintptr_t n, double* x, double xi, uintptr_t* i){
    DEBUG(5, "Entering bracket().\n");
    uintptr_t   ia,im,ib;
    
    ia = 0;
    ib = n-1;
    
    DEBUG(5,"n: %u\n",      (uint32_t)n);
    DEBUG(5,"xi: %e\n",     xi);
    DEBUG(5,"x[0]: %e\n",   x[0]);
    DEBUG(5,"[n-1]: %e\n",  x[n-1]);
    
    //if xi is outside the interval [ x[0], x[n-1] ], return zero
    if( (xi < x[0]) || (xi > x[n-1])){
        /* This was the old behaviour: when the index was out of bounds,
         * bracket would quit with an error.
        printf("n: %u\n",       (uint32_t)n);
        printf("xi: %e\n",      xi);
        printf("x[0]: %e\n",    x[0]);
        printf("[n-1]: %e\n",   x[n-1]);
        fatal("Bounding error in bracket().\nAborting.");
        */
        //now it will return 0, thus indicating a bounding error
        DEBUG(1, "bracket(): xi is outside of bounds.\n");
        return 0;
    }else{
        while( ib-ia > 1){
            //If xi is inside the interval [x(1) x(n)] just divide it by half until ib - ia = 1:
            im = (ia+ib)/2;
            if( (xi >= x[ia] ) && ( xi < x[im])){
                ib = im;
            }else{
                ia = im;
            }
            //printf("ia:%u; im:%u; ib:%u\n",ia, im, ib);
        }
        *i = ia;
        return 1;
    }
    DEBUG(5, "Leaving bracket().\n");
}
