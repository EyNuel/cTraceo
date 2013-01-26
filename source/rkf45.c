/****************************************************************************************
 * rkf45.c                                                                              *
 * Perform Rung-Kutta-Fehlberg integartion.                                             *
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
 *          dsi:    Step size used for interpolation (often callen h)                   *
 *          yOld:   Vector containing initial value of y:                               *
 *                      yOld[0]:    r:      range coordinate                            *
 *                      yOld[1]:    z:      depth coordinate                            *
 *                      yOld[2]:    sigmaR: range component of slowness vector          *
 *                      yOld[3]:    sigmaZ: depth component of slowness vector          *
 *          fOld:   Vector containing initial value of F, as defined for yOld.          *
 *                                                                                      *
 * Outputs:                                                                             *
 *          yNew:   Vector containing new values of of y.                               *
 *          fNew:   Vector containing new values of of F.                               *
 *          ds4:    Step size derived of RK4.                                           *
 *          ds5:    Step size derived of RK5.                                           *
 *                  ds4 and ds5 are checked in the calling function to verify           *
 *                  that the precision corresponds to requirements.                     *
 *                                                                                      *
 * Return Value:                                                                        *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma     once
#include    "csValues.c"
#include    "math.h"


void rkf45(settings_t*, double*, double*, double*, double*, double*, double*, double*);

void rkf45(settings_t* settings, double* dsi, double* yOld, double* fOld, double* yNew, double* fNew, double* ds4, double* ds5){
    DEBUG(6,"in\n");
    uintptr_t   j;
    double      dr,dz;
    double      k1[4],k2[4],k3[4],k4[4],k5[4],k6[4];
    double      yk[4],yrk4[4],yrk5[4];

    double      ri;
    double      zi;
    double      ci;
    double      cc;
    double      sigmaI;
    double      cri;
    double      czi;
    double      crri;
    double      czzi;
    double      crzi;
    double      sigmaR;
    double      sigmaZ;
    
    vector_t    es;
    vector_t    slowness;
    
    //define coeficients required for Runge-Kutta-Fehlberg method:
    //RK4:
    #define A1   (25.0/216.0)
    #define A3 (1408.0/2565.0)
    #define A4 (2197.0/4101.0)
    #define A5   (-1.0/5.0)
    //RK5:     
    #define B1    (16.0/135.0)
    #define B3  (6656.0/12825.0)
    #define B4 (28561.0/56430.0)
    #define B5    (-9.0/50.0)
    #define B6     (2.0/55.0)

    ri = yOld[0];
    zi = yOld[1];
    /* determine k1:                                            */
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
    for(j=0; j<4; j++){
        k1[j] = fOld[j];
        yk[j] = yOld[j] + 0.25 * (*dsi) * k1[j];
    }
    /* determine k2:                                            */
    ri  = yk[0];
    zi      = yk[1];
    sigmaR  = yk[2];
    sigmaZ  = yk[3];
    sigmaI = sqrt( pow(sigmaR,2) + pow(sigmaZ,2) );
    es.r = (sigmaR)/(sigmaI);
    es.z = (sigmaZ)/(sigmaI);
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);  //interpolate slowness vector
    k2[0] = es.r;
    k2[1] = es.z;
    k2[2] = slowness.r;
    k2[3] = slowness.z;
    for(j=0; j<4; j++){
        yk[j] = yOld[j] + (*dsi) *(3.0/32.0 * k1[j] + 9.0/32.0 * k2[j]);
    }

    /* determine k3:                                            */
    ri = yk[0];
    zi = yk[1];
    sigmaR = yk[2];
    sigmaZ = yk[3];
    sigmaI = sqrt( pow(sigmaR,2) + pow(sigmaZ,2) );
    es.r = (sigmaR)/(sigmaI);
    es.z = (sigmaZ)/(sigmaI);
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
    k3[0] = es.r;
    k3[1] = es.z;
    k3[2] = slowness.r;
    k3[3] = slowness.z;
    for(j=0; j<4; j++){
        yk[j] = yOld[j] + (*dsi) * ( 1932.0/2197.0*k1[j] -7200.0/2197.0*k2[j] + 7296.0/2197.0*k3[j]);
    }

    /* determine k4:                                            */
    ri = yk[0];
    zi = yk[1];
    sigmaR = yk[2];
    sigmaZ = yk[3];
    sigmaI = sqrt( pow(sigmaR,2) + pow(sigmaZ,2) );
    es.r = (sigmaR)/(sigmaI);
    es.z = (sigmaZ)/(sigmaI);
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
    k4[0] = es.r;
    k4[1] = es.z;
    k4[2] = slowness.r;
    k4[3] = slowness.z;
    for(j=0; j<4; j++){
        yk[j] = yOld[j] + (*dsi) * (439.0/216.0*k1[j] - 8.0*k2[j] + 3680.0/513.0*k3[j] - 845.0/4104*k4[j]);
    }

    /* determine k5:         (last RK4 step)                */
    ri = yk[0];
    zi = yk[1];
    sigmaR = yk[2];
    sigmaZ = yk[3];
    sigmaI = sqrt( pow(sigmaR,2) + pow(sigmaZ,2) );
    es.r = (sigmaR)/(sigmaI);
    es.z = (sigmaZ)/(sigmaI);
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
    k5[0] = es.r;
    k5[1] = es.z;
    k5[2] = slowness.r;
    k5[3] = slowness.z;
    for(j=0; j<4; j++){
        yk[j] = yOld[j] + (*dsi) * (2.0*k2[j] - 8.0/27.0*k1[j] + 3544.0/2565.0*k3[j] + 1859.0/4104*k4[j] - 11.0/40.0*k5[j]);
    }
    
    /* determine k6:        (last RK5 step)                 */
    ri = yk[0];
    zi = yk[1];
    sigmaR = yk[2];
    sigmaZ = yk[3];
    sigmaI = sqrt( pow(sigmaR,2) + pow(sigmaZ,2) );
    es.r = (sigmaR)/(sigmaI);
    es.z = (sigmaZ)/(sigmaI);
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
    k6[0] = es.r;
    k6[1] = es.z;
    k6[2] = slowness.r;
    k6[3] = slowness.z;

    
    for(j=0; j<4; j++){
        yrk4[j] = yOld[j] + (*dsi) * ( A1 * k1[j] + A3 * k3[j] + A4 * k4[j] + A5 * k5[j]);
        yrk5[j] = yOld[j] + (*dsi) * ( B1 * k1[j] + B3 * k3[j] + B4 * k4[j] + B5 * k5[j] + B6 * k6[j]);
        yNew[j] = yrk5[j];
    }

    /* Determine ds4 and ds5:       */
    ri = yNew[0];
    zi = yNew[1];
    sigmaR = yNew[2];
    sigmaZ = yNew[3];
    dr = yrk4[0] - yOld[0];
    dz = yrk4[1] - yOld[1];
    *ds4 = sqrt(dr*dr + dz*dz);

    dr = yrk5[0] - yOld[0];
    dz = yrk5[1] - yOld[1];
    *ds5 = sqrt(dr*dr + dz*dz);

    /* Calculate the actual output value:       */
    sigmaI = sqrt( pow(sigmaR,2) + pow(sigmaZ,2) );
    es.r = (sigmaR)/(sigmaI);
    es.z = (sigmaZ)/(sigmaI);
    csValues(settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
    fNew[0] = es.r;
    fNew[1] = es.z;
    fNew[2] = slowness.r;
    fNew[3] = slowness.z;
    DEBUG(6,"out\n");
}
