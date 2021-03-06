/****************************************************************************************
 *  calcParticleVel.c                                                                   *
 *  (formerly "calpvl.for")                                                             *
 *  Calculates particle velocity from coherent acoustic pressure.                       *
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
 *          settings:   Pointer to structure containing all input info.                 *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          "pvl.mat":  File containing Particle Velocity information.                  *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 *  NOTE:                                                                               *
 *          This function requires that the coherent acoustic pressure has been         *
 *          calculated (by calcCohAcoustPress) and will be of no use otherwise.         *
 *                                                                                      *
 ****************************************************************************************/

#include "globals.h"
#include <complex.h>
#if USE_MATLAB == 1
    #include <mat.h>
    #include "matrix.h"
#else
    #include    "matOut/matOut.h"
#endif
#include "interpolation.h"

void calcParticleVel(settings_t*);

void calcParticleVel(settings_t* settings){
    DEBUG(1, "\tin\n");
    
    mxArray*            pu2D;
    mxArray*            pw2D;
    uintptr_t           i, j, k;
    uintptr_t           dimR=0, dimZ=0;
    double              rHyd, zHyd;
    double              xp[3];
    double              dr, dz;     //used locally to make code more efficient (and more readable)
    complex double      junkComplex, dP_dRi, dP_dZi;
    complex double**    dP_dR2D = NULL;
    complex double**    dP_dZ2D = NULL;
    
    
    //get dr, dz:
    dr = settings->output.dr;
    dz = settings->output.dz;

    //determine array dimensions for pressure components:
    switch(settings->output.arrayType){
        case ARRAY_TYPE__HORIZONTAL:
            dimR = settings->output.nArrayR;
            dimZ = 1;
            break;
            
        case ARRAY_TYPE__VERTICAL:
            dimR = 1;
            dimZ = settings->output.nArrayZ;
            break;
            
        case ARRAY_TYPE__LINEAR:
            /*  in linear arrays, nArrayR and nArrayZ have to be equal
            *   (this is checked in readIn.c when reading the file).
            *   The pressure components will be written to the rightmost index
            *   of the 2d-array.
            */
            dimR = 1;
            dimZ = settings->output.nArrayZ;    //this should be equal to nArrayR
            break;
            
        case ARRAY_TYPE__RECTANGULAR:
            dimR = settings->output.nArrayR;
            dimZ = settings->output.nArrayZ;
            break;
            
        default:
            fatal("calcCohAcoustPress(): unknown array type.\nAborting.");
            break;
    }
    
    //alocate memory for output:
    dP_dR2D = mallocComplex2D(dimR, dimZ);
    dP_dZ2D = mallocComplex2D(dimR, dimZ);
    
    
    
    /**
     *  Horizontal and vertical pressure components where calculated in calcCohAcoustpress.
     *  We can now use this to calculate the actual particle velocity components:
     */
    DEBUG(5, "Particle velocity: Calculating absolute pressure components at the array:\n");
    switch(settings->output.arrayType){
        case ARRAY_TYPE__HORIZONTAL:
        case ARRAY_TYPE__VERTICAL:
        case ARRAY_TYPE__RECTANGULAR:
            for(j=0; j<settings->output.nArrayR; j++){
                rHyd = settings->output.arrayR[j];
                
                for(k=0; k<settings->output.nArrayZ; k++){
                    zHyd = settings->output.arrayZ[k],
                    
                    //TODO  get these values from a struct, instead of calculating them again?
                    //      (they where previously calculated in pressureStar.c)
                    
                    xp[0] = rHyd - dr;
                    xp[1] = rHyd;
                    xp[2] = rHyd + dr;
                    
                    intComplexBarycParab1D(xp, settings->output.pressure_H[j][k], rHyd, &junkComplex, &dP_dRi, &junkComplex);
                    
                    dP_dR2D[j][k] = -I*dP_dRi;
                    
                    xp[0] = zHyd - dz;
                    xp[1] = zHyd;
                    xp[2] = zHyd + dz;
                    
                    intComplexBarycParab1D(xp, settings->output.pressure_V[j][k], zHyd, &junkComplex, &dP_dZi, &junkComplex);
                    
                    dP_dZ2D[j][k] = I*dP_dZi;
                    
                    //show the pressure contribuitions:
                    /*
                    DEBUG(1, "(j,k)=(%u,%u)>> pL: %e,  pU, %e,  pR: %e,  pD: %e,  pC:%e\n",
                            (uint32_t)j, (uint32_t)k, cabs(settings->output.pressure_H[j][k][LEFT]),
                            cabs(settings->output.pressure_V[j][k][TOP]), cabs(settings->output.pressure_H[j][k][RIGHT]),
                            cabs(settings->output.pressure_V[j][k][BOTTOM]), cabs(settings->output.pressure_H[j][k][CENTER]));
                    */
                    DEBUG(7, "(j,k)=(%u,%u)>> dP_dR: %e, dP_dZ: %e\n",
                            (uint32_t)j, (uint32_t)k,
                            cabs(dP_dR2D[j][k]), cabs(dP_dZ2D[j][k]));
                }
            }
            break;
        case ARRAY_TYPE__LINEAR:
            for(j=0; j<settings->output.nArrayR; j++){
                rHyd = settings->output.arrayR[j];
                zHyd = settings->output.arrayZ[j],
                
                //TODO  get these values from a struct, instead of calculating them again?
                //      (they where previously calculated in pressureStar.c)
                
                xp[0] = rHyd - dr;
                xp[1] = rHyd;
                xp[2] = rHyd + dr;
                
                intComplexBarycParab1D(xp, settings->output.pressure_H[0][j], rHyd, &junkComplex, &dP_dRi, &junkComplex);
                
                dP_dR2D[0][j] = -I*dP_dRi;
                
                xp[0] = zHyd - dz;
                xp[1] = zHyd;
                xp[2] = zHyd + dz;
                
                intComplexBarycParab1D(xp, settings->output.pressure_V[0][j], zHyd, &junkComplex, &dP_dZi, &junkComplex);
                
                dP_dZ2D[0][j] = I*dP_dZi;
            }
            break;
    }

    /**
     *  Write the data to the output file:
     */
    switch( settings->output.arrayType){
        case ARRAY_TYPE__HORIZONTAL:
        case ARRAY_TYPE__RECTANGULAR:
        case ARRAY_TYPE__VERTICAL:
            //Note: the output for rectangular and horizontal cases has to be transposed.
            /// write the U-component to the mat-file:
            pu2D = mxCreateDoubleMatrix((MWSIZE)dimZ, (MWSIZE)dimR, mxCOMPLEX);
            if( pu2D == NULL){
                fatal("Memory alocation error.");
            }
            copyComplexToMxArray2D_transposed(dP_dR2D, pu2D, dimZ, dimR);
            matPutVariable(settings->options.matfile, "u", pu2D);
            mxDestroyArray(pu2D);
            
            /// write the W-component to the mat-file:
            pw2D = mxCreateDoubleMatrix((MWSIZE)dimZ, (MWSIZE)dimR, mxCOMPLEX);
            if( pw2D == NULL){
                fatal("Memory alocation error.");
            }
            copyComplexToMxArray2D_transposed(dP_dZ2D, pw2D, dimZ, dimR);
            matPutVariable(settings->options.matfile, "w", pw2D);
            mxDestroyArray(pw2D);
            break;
            
        
        case ARRAY_TYPE__LINEAR:
            DEBUG(3,"Writing pressure output of rectangular/vertical/horizontal array to file:\n");
            
            /// write the U-component to the mat-file:
            pu2D = mxCreateDoubleMatrix((MWSIZE)dimR, (MWSIZE)dimZ, mxCOMPLEX);
            if( pu2D == NULL){
                fatal("Memory alocation error.");
            }
            copyComplexToMxArray2D(dP_dR2D, pu2D, dimZ, dimR);
            matPutVariable(settings->options.matfile, "u", pu2D);
            mxDestroyArray(pu2D);
            
            /// write the W-component to the mat-file:
            pw2D = mxCreateDoubleMatrix((MWSIZE)dimR, (MWSIZE)dimZ, mxCOMPLEX);
            if( pw2D == NULL){
                fatal("Memory alocation error.");
            }
            copyComplexToMxArray2D(dP_dZ2D, pw2D, dimZ, dimR);
            matPutVariable(settings->options.matfile, "w", pw2D);
            mxDestroyArray(pw2D);
            break;
    }
    
    //free memory
    for(i=0; i<dimR; i++){
        free(settings->output.pressure_H[i]);
        free(settings->output.pressure_V[i]);
    }
    free(settings->output.pressure_H);
    free(settings->output.pressure_V);
    
    freeComplex2D(dP_dR2D, dimR);
    freeComplex2D(dP_dZ2D, dimR);
    DEBUG(1, "\tout\n");
}
