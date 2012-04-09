/****************************************************************************************
 *  calcSSP.c                                                                           *
 *  Interpolates the sound speed with depth and over a certain number of points.        *
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
 *          settings:   Pointer to structure containing all input info.                 *
 *                                                                                      *
 *  Outputs:                                                                            *
 *          "ssp.mat":  A file containing the trajectories of all launched rays.        *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma  once
#if USE_MATLAB == 1
    #include <mat.h>
    #include "matrix.h"
#else
    #include    "matOut/matOut.h"
#endif
#include "tools.h"
#include <math.h>
#include "solveEikonalEq.c"

void    calcSSP(settings_t* settings, uintptr_t nPoints);

void    calcSSP(settings_t* settings, uintptr_t nPoints){
    DEBUG(1,"in\n");
    
    MATFile*        matfile     = NULL;
    mxArray*        mxZSSP      = NULL;
    mxArray*        mxCSSP      = NULL;
    double          *depths = NULL;
    double          *c = NULL;
    uintptr_t       i;
    
    double      cc, sigmaI, cri, czi, crri, czzi, crzi;
    vector_t    slowness;
    
    c = mallocDouble(nPoints);
    depths = mallocDouble(nPoints);
    linearSpaced(nPoints, settings->soundSpeed.z[0], settings->soundSpeed.z[settings->soundSpeed.nz-1], depths);
    
    
    /* Interpolate the sound speeds:  */
    for(i=0; i<nPoints; i++){
        csValues(   settings,
                    settings->source.rbox1,
                    depths[i],
                    &c[i],
                    &cc,
                    &sigmaI,
                    &cri,
                    &czi,
                    &slowness,
                    &crri,
                    &czzi,
                    &crzi);
    }
    
    //open matfile for output
    matfile     = matOpen("ssp.mat", "w");
    
    mxZSSP        = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)nPoints, mxREAL);
    if(matfile == NULL || mxZSSP == NULL)
        fatal("Memory alocation error.");
    //copy cArray to mxArray:
    copyDoubleToMxArray(depths, mxZSSP, nPoints);
    //move mxArray to file:
    matPutVariable(matfile, "zssp", mxZSSP);
    mxDestroyArray(mxZSSP);
    
    mxCSSP        = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)nPoints, mxREAL);
    if( mxCSSP == NULL)
        fatal("Memory alocation error.");
    //copy cArray to mxArray:
    copyDoubleToMxArray(c, mxCSSP, nPoints);
    //move mxArray to file:
    matPutVariable(matfile, "cssp", mxCSSP);
    mxDestroyArray(mxCSSP);

    
    /// Finish up
    matClose(matfile);
    free(depths);
    free(c);
    DEBUG(1,"out\n");
}
