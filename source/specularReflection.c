/****************************************************************************************
 *  specularReflection.c                                                                *
 *  (formerly "reflct.for")                                                             *
 *  Calculate a vector's reflection of a surface.                                       *
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
 *          normal: A pointer to the surface's normal vector.                           *
 *          tauI:   A pointer to the incident vector.                                   *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          tauR:   A pointer to the reflected vector.                                  *
 *          theta:  The angle between the surface normal and the reflected              *
 *                  vector                                                              *
 *                                                                                      *
 * Return Value:                                                                        *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include "globals.h"
#include "math.h"
#include "dotProduct.c"

void specularReflection(vector_t*, vector_t*, vector_t*, double*);
void specularReflection(vector_t* normal, vector_t* tauI, vector_t* tauR, double* theta){
    DEBUG(5,"in\n");
    double  c = dotProduct(normal, tauI);

    tauR->r = tauI->r - 2*c * normal->r;
    tauR->z = tauI->z - 2*c * normal->z;

    *theta = acos( dotProduct(normal, tauR));
    DEBUG(5,"out\n");
}
