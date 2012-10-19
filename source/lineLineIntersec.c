/****************************************************************************************
 *  lineLineIntersec.c                                                                  *
 *  Calculate intersection point between two Lines (p and q) each defined by 2          *
 *  points in space. Note that the intersection point is actual lines defined by        *
 *  the points, rather than the line segments between the points, and can produce       *
 *  an intersection point beyond the lengths of the individual line segments.           *
 *                                                                                      *
 *  Reference:  http://en.wikipedia.org/wiki/Line-line_intersection                     *
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
 *          p1:     Coords to first point of line p.                                    *
 *          p2:     Coords to second point of line p.                                   *
 *          q1:     Coords to first point of line q.                                    *
 *          q2:     Coords to second point of line q.                                   *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          i:      Indicates wether the lines intersect.                               *
 *          isect:  Coordc of intersection point.                                       *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma once
#include "globals.h"

void    lineLineIntersec(point_t*, point_t*, point_t*, point_t*, uint32_t*, point_t*);

void    lineLineIntersec(point_t* p1, point_t* p2, point_t* q1, point_t* q2, uint32_t* i, point_t* isect){
    double      d;

    *i = 0;
    d = (p1->r - p2->r) * (q1->z - q2->z) - (p1->z - p2->z) * (q1->r - q2->r);

    if( fabs(d) > 1e-16){
        *i = 1;

        isect->r = ( (p1->r * p2->z - p1->z * p2->r) * (q1->r - q2->r) - (p1->r - p2->r) * (q1->r * q2->z - q1->z * q2->r) )/d;
        isect->z = ( (p1->r * p2->z - p1->z * p2->r) * (q1->z - q2->z) - (p1->z - p2->z) * (q1->r * q2->z - q1->z * q2->r) )/d;
    }
}

