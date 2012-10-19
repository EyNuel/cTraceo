/****************************************************************************************
 *  dotProduct.c                                                                        *
 *  (formerly "dot.for")                                                                *
 *  Calculate the dot product of 2 vectors.                                             *
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
 *          u:      A pointer to a vector.                                              *
 *          v:      A pointer to a vector.                                              *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          None                                                                        *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          the dot product.                                                            *
 *                                                                                      *
 ****************************************************************************************/

#pragma once

float  dotProduct(vector_t*, vector_t*);

float  dotProduct(vector_t* u, vector_t* v){
    //TODO: replace this with a call to openCL's dotProduct
    return( u->r * v->r + u->z * v->z);
}
