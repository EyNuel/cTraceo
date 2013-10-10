/****************************************************************************************
 *  calcCohTransLoss.c                                                                  *
 *  (formerly "calctl.for")                                                             *
 *  Calculates Coherent Transmission Loss.                                              *
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
 *          settings:   Pointer to structure containing all input info.                 *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          "ctl.mat":  File containing Coherent Transmission Loss.                     *
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
#if USE_MATLAB == 1
    #include <mat.h>
    #include "matrix.h"
#else
    #include    "matOut/matOut.h"
#endif
#include <math.h>
#include <complex.h>

void calcCohTransLoss(settings_t*);

void calcCohTransLoss(settings_t* settings){
    uint32_t    i, j, dim;
    double*     tl      = NULL;
    double**    tl2D    = NULL;
    mxArray*    ptl     = NULL;
    mxArray*    ptl2D   = NULL;
    

    switch(settings->output.arrayType){
        case ARRAY_TYPE__RECTANGULAR:
        case ARRAY_TYPE__HORIZONTAL:
        case ARRAY_TYPE__VERTICAL:
            /*
             * horizontal and vertical arrays are special cases of Rectangular hydrophone arrays,
             * so all of them can be handled by the same code.
             */
            tl2D = mallocDouble2D(settings->output.nArrayR, settings->output.nArrayZ);

            for(i=0; i<settings->output.nArrayR; i++){
                for(j=0; j<settings->output.nArrayZ; j++){
                    tl2D[i][j] = -20.0*log10( cabs( settings->output.pressure2D[i][j] ) );
                }
            }

            ptl2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
            if(ptl2D == NULL){
                fatal("Memory alocation error.");
            }
            copyDoubleToPtr2D_transposed(tl2D, ptl2D, settings->output.nArrayZ, settings->output.nArrayR);
            matPutVariable(settings->options.matfile,"tl",ptl2D);
            mxDestroyArray(ptl2D);

            freeDouble2D(tl2D, settings->output.nArrayR);
            break;

        case ARRAY_TYPE__LINEAR:
            dim = (uint32_t)max((double)settings->output.nArrayR, (double)settings->output.nArrayZ);
            tl = mallocDouble(dim);

            for(j=0; j<dim; j++){
                tl[j] = -20.0*log10( cabs( settings->output.pressure2D[0][j] ) );
                DEBUG(8, "|p|: %lf, tl: %lf\n", cabs( settings->output.pressure2D[0][j] ), tl[j]);
            }

            ptl = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)dim, mxREAL);
            if(ptl == NULL){
                fatal("Memory alocation error.");
            }
            copyDoubleToPtr(tl, mxGetPr(ptl), dim);
            matPutVariable(settings->options.matfile,"tl",ptl);
            mxDestroyArray(ptl);

            free(tl);
            break;
    }
}
