/****************************************************************************************
 *  thorpe.c                                                                            *
 *  (formerly "thorpe.for")                                                             *
 *  Calculate Thorpe attenuation. (reference: Bellhop)                                  *
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
 * Inputs:                                                                              *
 *          freq:   Frequency.                                                          *
 *                                                                                      *
 * Outputs:                                                                             *
 *          alpha:  Thorpe attenuation in dB/m.                                         *
 *                                                                                      *
 * Return Value:                                                                        *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

void    thorpe(float, float*);

void    thorpe(float freq, float* alpha){
    float      fxf;

    fxf     = pow( (freq/1000 ), 2);
    *alpha  = 0.0033 +0.11 * fxf / (1 +fxf ) + 44 * fxf / (4100 +fxf) +0.0003 * fxf;
    *alpha  = *alpha/8685.8896;
}
