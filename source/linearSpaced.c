/****************************************************************************************
 *  linearSpaced.c                                                                      *
 *  Generate a linearly spaced vector.                                                  *
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
 *          n:      Number of points to generate.                                       *
 *          xMin:   First value of x.                                                   *
 *          xMax:   Last value of x.                                                    *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          x:      a pointer to the generated vector                                   *
 *                  Note that this pointer has to be previously allocated with the      *
 *                  correct size.                                                       *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once

void    linearSpaced(uint32_t, float, float, float*);

void    linearSpaced(uint32_t n, float xMin, float xMax, float* x){
    uintptr_t   i;
    float  dx;

    DEBUG(10, "n:%u, xMin: %lf, xMax: %lf\n", n, xMin, xMax);
    dx = (xMax - xMin)/((float)(n - 1));
    for(i=0; i<n; i++){
        x[i] = xMin + dx*(float)(i);
    }
}
