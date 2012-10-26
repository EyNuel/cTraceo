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
 *          "rco.mat":  A file containing the trajectories of all launched rays.        *
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

void    calcRayCoords(settings_t*);

void    calcRayCoords(settings_t* settings){
    DEBUG(1,"in\n");
    
    MATFile*        matfile     = NULL;
    mxArray*        mxTheta     = NULL;
    mxArray*        mxR         = NULL;
    mxArray*        mxZ         = NULL;
    
    mxArray*        mxThetas    = NULL;
    mxArray*        mxTitle     = NULL;
    mxArray*        mxRayStruct = NULL;
    const char*     fieldNames[]= { "theta",
                                    "r",
                                    "z"};
    float      thetai, ctheta;
    ray_t*      ray     = NULL;
    uintptr_t   i;
    
    
    //open matfile for output
    if(settings->options.outputFileName == NULL){
        matfile     = matOpen("rco.mat", "w");
    }else{
        matfile = matOpen(settings->options.outputFileName, "w");
    }
    
    
    //write launching angles to file
    mxThetas        = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
    if(matfile == NULL || mxThetas == NULL)
        fatal("Memory alocation error.");
    //copy cArray to mxArray:
    copyFloatToMxArray(    settings->source.thetas, mxThetas, settings->source.nThetas);
    //move mxArray to file:
    matPutVariable(matfile, "thetas", mxThetas);
    mxDestroyArray(mxThetas);
    
    
    //write title to matfile:
    mxTitle = mxCreateString("TRACEO: Ray COordinates");
    if(mxTitle == NULL)
        fatal("Memory alocation error.");
    matPutVariable(matfile, "caseTitle", mxTitle);
    mxDestroyArray(mxTitle);
    
    
    //create mxStructArray:
    mxRayStruct = mxCreateStructMatrix( (MWSIZE)settings->source.nThetas,   //number of rows
                                        (MWSIZE)1,      //number of columns
                                        3,              //number of fields in each element
                                        fieldNames);    //list of field names
    if( mxRayStruct == NULL ) {
        fatal("Memory Alocation error.");
    }
    
    
    //allocate the rays:
    ray = makeRay(settings->source.nThetas);
    
    
    /* Trace the rays:  */
    for(i=0; i<settings->source.nThetas; i++){
        thetai = -settings->source.thetas[i] * M_PI/180.0;
        ray[i].theta = thetai;
        DEBUG(2,"ray[%u].theta: %lf\n", (uint32_t)i, settings->source.thetas[i]);
        ctheta = fabs( cos(thetai));
        
        //Trace a ray as long as it is neither 90 or -90:
        if (ctheta > 1.0e-7){
            solveEikonalEq(settings, &ray[i]);
            
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
            
            //copy data to mxArrays:
            copyFloatToMxArray(&settings->source.thetas[i],mxTheta,1);
            copyFloatToMxArray(ray[i].r,                   mxR,    ray[i].nCoords);
            copyFloatToMxArray(ray[i].z,                   mxZ,    ray[i].nCoords);
            
            //copy mxArrays to mxRayStruct
            mxSetFieldByNumber( mxRayStruct,                        //pointer to the mxStruct
                                (MWINDEX)i,                         //index of the element (number of ray)
                                0,                                  //position of the field (in this case, field 0 is "theta"
                                mxTheta);                           //the mxArray we want to copy into the mxStruct
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 1, mxR);   // "r"
            mxSetFieldByNumber( mxRayStruct, (MWINDEX)i, 2, mxZ);   // "z"
            ///ray has been saved to mxStructArray
            
            if(KEEP_RAYS_IN_MEM == false){
                //free the ray's memory
                reallocRayMembers(&ray[i],0);
            }
        }//if (ctheta > 1.0e-7)
    }//for(i=0; i<settings->source.nThetas; i++)
    
    /// Write all ray information to matfile:
    matPutVariable(matfile, "rays", mxRayStruct);
    
    /// Finish up
    matClose(matfile);
    free(ray);
    DEBUG(1,"out\n");
}
