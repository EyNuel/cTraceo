/****************************************************************************************
 *  convertUnits.c                                                                      *
 *  (formerly "cnvnts.for")                                                             *
 *  Convert from several atenuation units to dB/lambda.                                 *
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
 *          aIn:    Atenuation value to be converted.                                   *
 *          lambda: Wavelength.                                                         *
 *          freq:   Frequency.                                                          *
 *          units:  Determines the input units (see globals.h for possible values).     *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          aOut:   Converted atenuation value.                                         *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma  once
#include "globals.h"
#include "tools.h"
#include <math.h>

void convertUnits(float*, float*, float*, uint32_t*, float*);

void convertUnits(float* aIn, float* lambda, float* freq, uint32_t* units, float* aOut){
    //TODO add support for "parameter loss", 'L' option from Bellhop's option1(3)
    
    #define c1 (8.68588963806504)
    
    switch(*units){
        case    SURFACE_ATTEN_UNITS__dBperkHz:      //"F",  dB/kHz
            *aOut = *aIn * (*lambda) * (*freq) * 1.0e-3;
            break;
            
        case    SURFACE_ATTEN_UNITS__dBperMeter:    //"M",  dB/meter
            *aOut = (*aIn) * (*lambda);
            break;
            
        case    SURFACE_ATTEN_UNITS__dBperNeper:    //"N",  dB/neper
            *aOut = (*aIn) * (*lambda) * c1;
            break;
            
        case    SURFACE_ATTEN_UNITS__qFactor:       //"Q",  Q factor
            //Avoid divide-by-zero
            if (*aIn == 0.0){
                *aOut = 0.0;
            }else{
                *aOut = c1 * M_PI / (*aIn);
            }
            break;
            
        case    SURFACE_ATTEN_UNITS__dBperLambda:   //"W",  dB/<lambda>
            *aOut = *aIn;
            break;
            
        default:
            fatal("Boundary attenuation: unknown units.\nAborting...");
            break;
    }
}
