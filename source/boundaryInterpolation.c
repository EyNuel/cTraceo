/****************************************************************************************
 *  boundaryInterpolation.c                                                             *
 *  (formerly "bdryi.for")                                                              *
 *  Interpolate depth of unknown boundary points.                                       *
 *  Used for altimetry/bathymetry as well as objects.                                   * 
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
 *          The code may contain bugs and future changes are probable.                  *
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
 *          interface:  A pointer of type interface_t, containing the parameters        *
 *                      of the interface.                                               *
 *          ri:         range of interpolation point.                                   *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          zi:         interpolated depth at interpolation point.                      *
 *          taub:       surface's tangent vector at intersection.                       *
 *          normal:     surface's normal vector at intersection point.                  *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma     once
#include    "globals.h"
#include    "interpolation.h"



void boundaryInterpolationExplicit(uint32_t*, double*, double*, uint32_t*, double, double*, vector_t*, vector_t*);
void boundaryInterpolation(interface_t*, double, double*, vector_t*, vector_t*);

void boundaryInterpolationExplicit(uint32_t* numSurfaceCoords, double* r, double* z, uint32_t* surfaceInterpolation, double ri, double* zi, vector_t* taub, vector_t* normal){
    DEBUG(5,"in\n");
        
    double      zri = 0;    //1st derivative of z at ri
    double      zrri;   //2nd derivative of z at ri
    uintptr_t   i = 0;
    
    switch(*surfaceInterpolation){
        case SURFACE_INTERPOLATION__FLAT:
            DEBUG(5,"Flat surface interpolation\n");
            intLinear1D( &(r[0]), &(z[0]),ri,zi,&zri);
            break;
            
        case SURFACE_INTERPOLATION__SLOPED:
            DEBUG(5,"sloped surface interpolation\n");
            intLinear1D( &(r[0]), &(z[0]),ri,zi,&zri);
            break;
            
        case SURFACE_INTERPOLATION__2P:
            DEBUG(5,"2P surface interpolation\n");
            bracket(*numSurfaceCoords, r, ri, &i);
            intLinear1D( &(r[i]), &(z[i]),ri,zi,&zri);
            break;
            
        case SURFACE_INTERPOLATION__3P:
            fatal("Error: Parabolic (3P) interpolation is no longer supported as it causes incorrect results.\nPlease use a different type of Boundary interpolation.");
            break;
            
        case SURFACE_INTERPOLATION__4P:
            DEBUG(5,"4P surface interpolation\n");
            if (ri <= r[1]){
                i = 1;
            }else if(ri >= r[*numSurfaceCoords - 2]){
                i = *numSurfaceCoords - 3;
            }else{
                bracket( *numSurfaceCoords, &(r[0]), ri, &i);
            }
            intBarycCubic1D( &(r[i-1]), &(z[i-1]), ri, zi, &zri, &zrri);
    }

    taub->r = cos( atan(zri));
    taub->z = sin( atan(zri));

    //Avoid rounding errors:
    if ( fabs(taub->r) == 1.0 ){
        taub->z = 0;
    }else if ( fabs(taub->z) == 1.0 ){
       taub->r = 0;
    }
    
    normal->r = -taub->z; 
    normal->z =  taub->r;
    DEBUG(5,"out\n");
}

void boundaryInterpolation(interface_t* interface, double ri, double* zi, vector_t* taub, vector_t* normal){
    boundaryInterpolationExplicit(  &(interface->numSurfaceCoords),
                                    interface->r,
                                    interface->z,
                                    &(interface->surfaceInterpolation),
                                    ri, zi, taub, normal);
}
