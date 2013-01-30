/****************************************************************************************
 *  calcRayCoords.c                                                                     *
 *  (formerly "calrco.for")                                                             *
 *  Uses solveEikonalEq for raytracing and writes ray coordinates to .mat file.         *
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
 *          "ari.mat":  A file containing the coordinates of all rays.                  *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#include <complex.h>
#include "globals.h"
#include "tools.h"
#include "solveDynamicEq.c"
#include "solveEikonalEq.c"
#if USE_MATLAB == 1
    #include <mat.h>
    #include "matrix.h"
#else
    #include    "matOut/matOut.h"
#endif

void    calcAllRayInfo(settings_t*);

void    calcAllRayInfo(settings_t* settings){
    DEBUG(1,"in\n");
    mxArray*            pThetas     = NULL;
    mxArray*            mxTheta     = NULL;
    mxArray*            mxR         = NULL;
    mxArray*            mxZ         = NULL;
    mxArray*            mxTau       = NULL;
    mxArray*            mxAmp       = NULL;
    mxArray*            iReturns    = NULL;
    mxArray*            nSurRefl    = NULL;
    mxArray*            nBotRefl    = NULL;
    mxArray*            nObjRefl    = NULL;
    mxArray*            nRefrac     = NULL;
    mxArray*            mxRefrac_r  = NULL;
    mxArray*            mxRefrac_z  = NULL;
    
    mxArray*            mxRayStruct = NULL;
    const char*         fieldNames[]= { "theta",
                                        "r",
                                        "z",
                                        "tau",
                                        "amp",
                                        "iReturns",
                                        "nSurRefl",
                                        "nBotRefl",
                                        "nObjRefl",
                                        "nRefrac",
                                        "refrac_r",
                                        "refrac_z"};        //the names of the fields contained in mxRayStruct
    double              thetai, ctheta;
    ray_t*              ray         = NULL;
    uintptr_t           i;
    
    
    //write launching angles to file
    pThetas     = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
    if(pThetas == NULL){
        fatal("Memory alocation error.");
    }
    //copy angles in cArray to mxArray:
    copyDoubleToMxArray(    settings->source.thetas, pThetas, settings->source.nThetas);
    //move mxArray to file and free memory:
    matPutVariable(settings->options.matfile, "thetas", pThetas);
    mxDestroyArray(pThetas);
    
    
    //create mxStructArray:
    mxRayStruct = mxCreateStructMatrix( (MWSIZE)settings->source.nThetas,   //number of rows
                                        (MWSIZE)1,      //number of columns
                                        12,             //number of fields in each element
                                        fieldNames);    //list of field names
    if( mxRayStruct == NULL ) {
        fatal("Memory Alocation error.");
    }
    
    //allocate memory for the rays:
    ray = makeRay(settings->source.nThetas);
    
    /** Trace the rays: */
    for(i=0; i<settings->source.nThetas; i++){
        thetai = -settings->source.thetas[i] * M_PI/180.0;
        ray[i].theta = thetai;
        DEBUG(2,"ray[%u].theta: %lf\n", (uint32_t)i, settings->source.thetas[i]);
        ctheta = fabs( cos(thetai));
        
        //Trace a ray as long as it is neither 90 or -90:
        if (ctheta > 1.0e-7){
            solveEikonalEq(settings, &ray[i]);
            solveDynamicEq(settings, &ray[i]);
            DEBUG(4, "Equations solved.\n");
            
            ///prepare to write ray to mxStructArray:
            /*NOTE: when writing a mxArray to a mxStructArray, the mxArray cannot simply be reused after it was
             *      copied to the mxStructArray, otherwise data corruption will occur.
             *      Because of this, the variables mxR, mxZ, mxTau, mxAmp have to be allocated again at each pass.
             *      Also note that these mxArrays shall not be deallocated, otherwise their content will be lost
             *      (even after they have been copied to the mxStruct).
             *      This may appear to be a memmory leak, but it is intentional.
             */
            
            //create mxArrays:
            mxTheta = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,              mxREAL);
            mxR     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)ray[i].nCoords, mxREAL);
            mxZ     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)ray[i].nCoords, mxREAL);
            mxTau   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)ray[i].nCoords, mxREAL);
            mxAmp   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)ray[i].nCoords, mxCOMPLEX);
            if( mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
                fatal("Memory alocation error.");
            }
            
            //copy data to mxArrays:
            copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
            copyDoubleToMxArray(ray[i].r,                   mxR,    ray[i].nCoords);
            copyDoubleToMxArray(ray[i].z,                   mxZ,    ray[i].nCoords);
            copyDoubleToMxArray(ray[i].tau,                 mxTau,  ray[i].nCoords);
            copyComplexToMxArray(ray[i].amp,                mxAmp,  ray[i].nCoords);
            
            //copy mxArrays to mxRayStruct
            mxSetFieldByNumber( mxRayStruct,        //pointer to the mxStruct
                                (MWINDEX)i,         //index of the element (number of ray)
                                0,                  //position of the field (in this case, field 0 is "r"
                                mxTheta);           //the mxArray we want to copy into the mxStruct
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 1, mxR);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 2, mxZ);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 3, mxTau);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 4, mxAmp);
            ///ray has been saved to mxStructArray
            
            ///now lets save some aditional ray information:
            iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
            nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
            nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
            nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
            nRefrac     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
            
            copyBoolToMxArray(      &ray[i].iReturn,    iReturns,   1);
            copyUInt32ToMxArray(    &ray[i].sRefl,      nSurRefl,   1);
            copyUInt32ToMxArray(    &ray[i].bRefl,      nBotRefl,   1);
            copyUInt32ToMxArray(    &ray[i].oRefl,      nObjRefl,   1);
            copyUInt32ToMxArray(    &ray[i].nRefrac,    nRefrac,    1);
            
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 5, iReturns);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 6, nSurRefl);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 7, nBotRefl);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 8, nObjRefl);
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 9, nRefrac);
            ///aditional information has been saved
            
            ///save refraction coordinates to structure:
            if (ray[i].nRefrac > 0){
                mxRefrac_r = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[i].nRefrac, mxREAL);
                mxRefrac_z = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[i].nRefrac, mxREAL);
                
                copyDoubleToMxArray(ray[i].rRefrac, mxRefrac_r, ray[i].nRefrac);
                copyDoubleToMxArray(ray[i].zRefrac, mxRefrac_z, ray[i].nRefrac);
                
                mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 10, mxRefrac_r);
                mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 11, mxRefrac_z);
            }
            if(KEEP_RAYS_IN_MEM == false){
                //free the ray's memory
                reallocRayMembers(&ray[i],0);
            }
        }
    }

    /// Write all ray information to matfile:
    matPutVariable(settings->options.matfile, "rays", mxRayStruct);
    
    /// Finish up
    mxDestroyArray(mxRayStruct);
    free(ray);
    DEBUG(1,"out\n");
}
