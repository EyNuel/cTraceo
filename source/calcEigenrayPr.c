/****************************************************************************************
 *  calcEigenrayPr.c                                                                    *
 *  (formerly "calepr.for")                                                             *
 *  Calculates eigenrays using proximity method.                                        *
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
 *          "eig.mat":  File containing eigenray information.                           *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#pragma  once
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
#include "interpolation.h"
#include "bracket.c"
#include "eBracket.c"

void    calcEigenrayPr(settings_t*);

void    calcEigenrayPr(settings_t* settings){
    DEBUG(1,"in\n");
    float          thetai, ctheta;
    float          junkFloat;
    ray_t*          ray         = NULL;
    uintptr_t       iRay, iArrayR, iArrayZ, l;
    float           rHyd, zHyd, zRay, tauRay;
    complex float   junkComplex, ampRay;
    float           dz;
    uintptr_t       nRet, iHyd = 0;
    uintptr_t       iRet[51];
    uint32_t        maxNumEigenrays = 0;

    mxArray*        pThetas             = NULL;
    mxArray*        pHydArrayR          = NULL;
    mxArray*        pHydArrayZ          = NULL;
    mxArray*        mxMaxNumEigenrays   = NULL;
    mxArray*        mxTheta             = NULL;
    mxArray*        mxR                 = NULL;
    mxArray*        mxZ                 = NULL;
    mxArray*        mxTau               = NULL;
    mxArray*        mxAmp               = NULL;
    mxArray*        iReturns            = NULL;
    mxArray*        nSurRefl            = NULL;
    mxArray*        nBotRefl            = NULL;
    mxArray*        nObjRefl            = NULL;
    mxArray*        nRefrac             = NULL;
    mxArray*        mxRefrac_r          = NULL;
    mxArray*        mxRefrac_z          = NULL;
    mxArray*        mxEigenrayStruct    = NULL;     //contains the eigenrays at a single hydrophone
    mxArray*        mxAllEigenraysStruct= NULL;     //contains all the eigenrays at all hydrophones
    mxArray*        mxNumEigenrays      = NULL;
    mxArray*        mxRHyd              = NULL;
    mxArray*        mxZHyd              = NULL;
    const char*     eigenrayFieldNames[]= { "nEigenrays", "rHyd", "zHyd", "eigenray" };
    const char*     arrivalFieldNames[] = { "theta",
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
    MWINDEX         idx[2];                         //used for accessing a specific element in the mxAadStruct

    eigenrays_t     eigenrays[settings->output.nArrayR][settings->output.nArrayZ];
    /*
     * eigenrays[][] is an array with the dimensions of the hydrophone array and will contain the
     * actual arrival information before it is written to a matlab structure at the end of the file.
     */
    //initialize to 0
    for (iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
        for (iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
            DEBUG(1, "initializing eigenray structure (j, jj) = (%lu, %lu)...", iArrayR, iArrayZ);
            eigenrays[iArrayR][iArrayZ].nEigenrays = 0;
            eigenrays[iArrayR][iArrayZ].mxEigenrayStruct = mxCreateStructMatrix(   (MWSIZE)settings->source.nThetas,       //number of rows
                                                                        (MWSIZE)1,                              //number of columns
                                                                        12,                                     //number of fields in each element
                                                                        arrivalFieldNames);                     //list of field names
            if( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct == NULL ){
                fatal("Memory Alocation error.");
            }
            DEBUG(1, "Done.\n");
        }
    }
    
    #if 1
    //write launching angles to file
    pThetas     = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
    if(pThetas == NULL){
        fatal("Memory alocation error.");
    }
    //copy angles in cArray to mxArray:
    copyFloatToMxArray(settings->source.thetas, pThetas, settings->source.nThetas);
    //move mxArray to file and free memory:
    matPutVariable(settings->options.matfile, "thetas", pThetas);
    mxDestroyArray(pThetas);


    //write hydrophone array ranges to file:
    pHydArrayR          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
    if(pHydArrayR == NULL){
        fatal("Memory alocation error.");
    }
    copyFloatToMxArray(    settings->output.arrayR, pHydArrayR, (uintptr_t)settings->output.nArrayR);
    matPutVariable(settings->options.matfile, "rarray", pHydArrayR);
    mxDestroyArray(pHydArrayR);


    //write hydrophone array depths to file:
    pHydArrayZ          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
    if(pHydArrayZ == NULL){
        fatal("Memory alocation error.");
    }
    copyFloatToMxArray(    settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
    matPutVariable(settings->options.matfile, "zarray", pHydArrayZ);
    mxDestroyArray(pHydArrayZ);


    //allocate memory for the rays:
    ray = makeRay(settings->source.nThetas);
    #endif

    /** Trace the rays:  */
    for(iRay=0; iRay<settings->source.nThetas; iRay++){
        thetai = -settings->source.thetas[iRay] * M_PI/180.0;
        ray[iRay].theta = thetai;
        ctheta = fabs( cos(thetai));

        //Trace a ray as long as it is neither at 90 nor -90:
        if (ctheta > 1.0e-7){
            solveEikonalEq(settings, &ray[iRay]);
            solveDynamicEq(settings, &ray[iRay]);

            //test for proximity of ray to each hydrophone
            //(yes, this is slow, can you figure out a better way to do it?)
            for(iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
                rHyd = settings->output.arrayR[iArrayR];

                if ( (rHyd >= ray[iRay].rMin) && (rHyd <= ray[iRay].rMax)){

                    //  Check if the ray is returning back or not;
                    //  if not:     we can bracket it without problems,
                    //  otherwise:  we need to know how many times it passed by the given array range
                    if (ray[iRay].iReturn == false){

                        /*
                        //when debugging, save the coordinates of the last ray to a separate matfile before exiting.
                        #if VERBOSE
                        if((int)i==(int)4){
                            DEBUG(1,"i:%u, nCoords: %u\n", (uint32_t)i, (uint32_t)ray[iRay].nCoords);
                            DEBUG(1,"rMin: %e, rMax: %e\n",ray[iRay].rMin, ray[iRay].rMax);

                            mxArray*    pDyingRay   = NULL;
                            MATFile*    matfile2    = NULL;
                            float**    dyingRay    = malloc(2*sizeof(uintptr_t));
                            char*       string2     = mallocChar(10);

                            dyingRay[0] = ray[iRay].r;
                            dyingRay[1] = ray[iRay].z;
                            matfile2    = matOpen("dyingRay.mat", "w");
                            pDyingRay   = mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)ray[iRay].nCoords, mxREAL);
                            if(pDyingRay == NULL || matfile2 == NULL)
                                fatal("Memory alocation error.");
                            copyFloatToPtr2D(dyingRay, mxGetPr(pDyingRay), ray[iRay].nCoords,2);
                            sprintf(string2, "dyingRay");
                            matPutVariable(matfile2, (const char*)string2, pDyingRay);

                            mxDestroyArray(pDyingRay);
                            matClose(matfile2);
                            DEBUG(1,"Dying ray written to file.\n");
                        }
                        #endif
                        */

                        //get the index of the lower bracketing element:
                        bracket(ray[iRay].nCoords, ray[iRay].r, rHyd, &iHyd);
                        DEBUG(3,"non-returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[iRay].nCoords, (uint32_t)iHyd);

                        //from index interpolate the rays' depth:
                        intLinear1D(&ray[iRay].r[iHyd], &ray[iRay].z[iHyd], rHyd, &zRay,    &junkFloat);

                        //for every hydrophone check distance to ray
                        for(iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
                            zHyd = settings->output.arrayZ[iArrayZ];
                            dz = fabs(zRay-zHyd);
                            DEBUG(4, "dz: %e\n", dz);

                            if (dz < settings->output.miss){
                                DEBUG(3, "Eigenray found\n");

                                //from index interpolate the rays' travel time and amplitude:
                                intLinear1D(        &ray[iRay].r[iHyd], &ray[iRay].tau[iHyd], rHyd, &tauRay,  &junkFloat);
                                intComplexLinear1D( &ray[iRay].r[iHyd], &ray[iRay].amp[iHyd], rHyd, &ampRay,  &junkComplex);

                                //adjust the ray's last set of coordinates so that it matches up with the hydrophone
                                ray[iRay].r[iHyd+1]    = rHyd;
                                ray[iRay].z[iHyd+1]    = zRay;
                                ray[iRay].tau[iHyd+1]  = tauRay;
                                ray[iRay].amp[iHyd+1]  = ampRay;

                                ///prepare to write eigenray to matfile:
                                //create mxArrays:
                                mxTheta = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,          mxREAL);
                                mxR     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iHyd+2),   mxREAL);
                                mxZ     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iHyd+2),   mxREAL);
                                mxTau   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iHyd+2),   mxREAL);
                                mxAmp   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iHyd+2),   mxCOMPLEX);
                                if( mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
                                    fatal("Memory alocation error.");
                                }

                                //copy data to mxArrays:
                                copyFloatToMxArray(&settings->source.thetas[iRay],mxTheta,1);
                                copyFloatToMxArray(ray[iRay].r,                   mxR,    iHyd+2);
                                copyFloatToMxArray(ray[iRay].z,                   mxZ,    iHyd+2);
                                copyFloatToMxArray(ray[iRay].tau,                 mxTau,  iHyd+2);
                                copyComplexFloatToMxArray(ray[iRay].amp,          mxAmp,  iHyd+2);

                                //copy mxArrays to mxEigenrayStruct
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct,  //pointer to the mxStruct
                                                    (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays,   //index of the element
                                                    0,                                  //position of the field (in this case, field 0 is "theta"
                                                    mxTheta);                           //the mxArray we want to copy into the mxStruct
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 1, mxR);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 2, mxZ);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 3, mxTau);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 4, mxAmp);
                                ///Eigenray has been saved to mxAadStruct

                                ///now lets save some aditional ray information:
                                iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nRefrac     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                                copyBoolToMxArray(      &ray[iRay].iReturn,    iReturns,   1);
                                copyUInt32ToMxArray(    &ray[iRay].sRefl,      nSurRefl,   1);
                                copyUInt32ToMxArray(    &ray[iRay].bRefl,      nBotRefl,   1);
                                copyUInt32ToMxArray(    &ray[iRay].oRefl,      nObjRefl,   1);
                                copyUInt32ToMxArray(    &ray[iRay].nRefrac,    nRefrac,    1);

                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 5, iReturns);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 6, nSurRefl);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 7, nBotRefl);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 8, nObjRefl);
                                mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 9, nRefrac);
                                ///aditional information has been saved

                                ///save refraction coordinates to structure:
                                if (ray[iRay].nRefrac > 0){
                                    mxRefrac_r = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[iRay].nRefrac, mxREAL);
                                    mxRefrac_z = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[iRay].nRefrac, mxREAL);

                                    copyFloatToMxArray(ray[iRay].rRefrac, mxRefrac_r, ray[iRay].nRefrac);
                                    copyFloatToMxArray(ray[iRay].zRefrac, mxRefrac_z, ray[iRay].nRefrac);

                                    //~ mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 10, mxRefrac_r);
                                    //~ mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 11, mxRefrac_z);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 10, mxRefrac_r);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 11, mxRefrac_z);
                                }

                                eigenrays[iArrayR][iArrayZ].nEigenrays += 1;
                                maxNumEigenrays = max(eigenrays[iArrayR][iArrayZ].nEigenrays, maxNumEigenrays);
                            }// if (dz settings->output.miss)
                        }// for(jj=1; jj<=settings->output.nArrayZ; jj++)

                    }else{// if (ray[iRay].iReturn == false)

                        DEBUG(3,"returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[iRay].nCoords, (uint32_t)iHyd);
                        //get the indexes of the bracketing points.
                        eBracket(ray[iRay].nCoords, ray[iRay].r, rHyd, &nRet, iRet);

                        //for each index where the ray passes at the hydrophone, interpolate the rays' depth:
                        for(l=0; l<nRet; l++){
                            DEBUG(4, "nRet=%u, iRet[%u]= %u\n", (uint32_t)nRet, (uint32_t)l, (uint32_t)iRet[l]);
                            intLinear1D(&ray[iRay].r[iRet[l]], &ray[iRay].z[iRet[l]], rHyd, &zRay, &junkFloat);

                            //for every hydrophone check if the ray is close enough to be considered an eigenray:
                            for(iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
                                zHyd = settings->output.arrayZ[iArrayZ];
                                dz = fabs( zRay - zHyd );

                                if (dz < settings->output.miss){

                                    //interpolate the ray's travel time and amplitude:
                                    intLinear1D(        &ray[iRay].r[iRet[l]], &ray[iRay].tau[iRet[l]], rHyd,                &tauRay,  &junkFloat);
                                    intComplexLinear1D( &ray[iRay].r[iRet[l]], &ray[iRay].amp[iRet[l]], (complex float)rHyd, &ampRay,  &junkComplex);

                                    DEBUG(1, "i: %u, iHyd: %u, nCoords: %u\n", (uint32_t)iRay, (uint32_t)iHyd,(uint32_t)ray[iRay].nCoords);
                                    //adjust the ray's last set of coordinates so that it matches up with the hydrophone
                                    ray[iRay].r[iRet[l]+1] = rHyd;
                                    ray[iRay].z[iRet[l]+1] = zRay;
                                    ray[iRay].tau[iRet[l]+1]   = tauRay;
                                    ray[iRay].amp[iRet[l]+1]   = ampRay;

                                    ///prepare to write eigenray to matfile:
                                    //create mxArrays:
                                    mxTheta = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,          mxREAL);
                                    mxR     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iRet[l]+2),    mxREAL);
                                    mxZ     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iRet[l]+2),    mxREAL);
                                    mxTau   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iRet[l]+2),    mxREAL);
                                    mxAmp   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(iRet[l]+2),    mxCOMPLEX);
                                    if( mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
                                        fatal("Memory alocation error.");
                                    }

                                    //copy data to mxArrays:
                                    copyFloatToMxArray(&settings->source.thetas[iRay],mxTheta,1);
                                    copyFloatToMxArray(ray[iRay].r,                   mxR,    iRet[l]+2);
                                    copyFloatToMxArray(ray[iRay].z,                   mxZ,    iRet[l]+2);
                                    copyFloatToMxArray(ray[iRay].tau,                 mxTau,  iRet[l]+2);
                                    copyComplexFloatToMxArray(ray[iRay].amp,          mxAmp,  iRet[l]+2);

                                    //copy mxArrays to mxEigenrayStruct
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct,  //pointer to the mxStruct
                                                        (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays,   //index of the element
                                                        0,                                  //position of the field (in this case, field 0 is "theta"
                                                        mxTheta);                           //the mxArray we want to copy into the mxStruct
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 1, mxR);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 2, mxZ);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 3, mxTau);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 4, mxAmp);
                                    ///Eigenray has been saved to mxAadStruct

                                    ///now lets save some aditional ray information:
                                    iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nRefrac     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                                    copyBoolToMxArray(      &ray[iRay].iReturn, iReturns,   1);
                                    copyUInt32ToMxArray(    &ray[iRay].sRefl,   nSurRefl,   1);
                                    copyUInt32ToMxArray(    &ray[iRay].bRefl,   nBotRefl,   1);
                                    copyUInt32ToMxArray(    &ray[iRay].oRefl,   nObjRefl,   1);
                                    copyUInt32ToMxArray(    &ray[iRay].nRefrac, nRefrac,    1);

                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 5, iReturns);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 6, nSurRefl);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 7, nBotRefl);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 8, nObjRefl);
                                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 9, nRefrac);
                                    ///aditional information has been saved

                                    ///save refraction coordinates to structure:
                                    if (ray[iRay].nRefrac > 0){
                                        mxRefrac_r = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[iRay].nRefrac, mxREAL);
                                        mxRefrac_z = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[iRay].nRefrac, mxREAL);

                                        copyFloatToMxArray(ray[iRay].rRefrac, mxRefrac_r, ray[iRay].nRefrac);
                                        copyFloatToMxArray(ray[iRay].zRefrac, mxRefrac_z, ray[iRay].nRefrac);

                                        mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 10, mxRefrac_r);
                                        mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)iRay, 11, mxRefrac_z);
                                    }

                                    eigenrays[iArrayR][iArrayZ].nEigenrays += 1;
                                    maxNumEigenrays = max(eigenrays[iArrayR][iArrayZ].nEigenrays, maxNumEigenrays);
                                }
                            }
                        }
                    }// if (ray[iRay].iReturn == false)
                }//if ( (rHyd >= ray[iRay].rMin) && (rHyd < ray[iRay].rMax))
            }//for(j=0; j<settings->output.nArrayR; j++){
            if(KEEP_RAYS_IN_MEM == false){
                //free the ray's memory
                reallocRayMembers(&ray[iRay],0);
            }
        }//if (ctheta > 1.0e-7)
    }//for(iRay=0; iRay<settings->source.nThetas; iRay++)
    
    //write "maximum number of eigenrays at any of the hydrophones
    mxMaxNumEigenrays = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
    copyUInt32ToMxArray(&maxNumEigenrays, mxMaxNumEigenrays, 1);
    matPutVariable(settings->options.matfile, "maxNumEigenrays", mxMaxNumEigenrays);
    
    //copy arrival data to mxAllEigenraysStruct:
    mxAllEigenraysStruct = mxCreateStructMatrix((MWSIZE)settings->output.nArrayZ,   //number of rows
                                                (MWSIZE)settings->output.nArrayR,   //number of columns
                                                4,                                  //number of fields in each element
                                                eigenrayFieldNames);                //list of field names
    if( mxAllEigenraysStruct == NULL ) {
        fatal("Memory Alocation error.");
    }

    for (iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
        for (iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
            mxNumEigenrays  = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxRHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxZHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);

            copyFloatToMxArray(&eigenrays[iArrayR][iArrayZ].nEigenrays, mxNumEigenrays,1);
            copyFloatToMxArray(&settings->output.arrayR[iArrayR],       mxRHyd,1);
            copyFloatToMxArray(&settings->output.arrayZ[iArrayZ],       mxZHyd,1);

            idx[0] = (MWINDEX)iArrayZ;
            idx[1] = (MWINDEX)iArrayR;
            
            mxSetFieldByNumber( mxAllEigenraysStruct,                       //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                0,                                          //position of the field (in this case, field 0 is "theta"
                                mxNumEigenrays);                            //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAllEigenraysStruct,                       //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                1,                                          //position of the field (in this case, field 0 is "theta"
                                mxRHyd);                                    //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAllEigenraysStruct,                       //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                2,                                          //position of the field (in this case, field 0 is "theta"
                                mxZHyd);                                    //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAllEigenraysStruct,                       //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct,  2, idx),   //index of the element
                                3,                                          //position of the field (in this case, field 0 is "theta"
                                eigenrays[iArrayR][iArrayZ].mxEigenrayStruct);         //the mxArray we want to copy into the mxStruct
        }
    }

    ///Write Eigenrays to matfile:
    matPutVariable(settings->options.matfile, "eigenrays", mxAllEigenraysStruct);
    
    //free memory
    mxDestroyArray(mxEigenrayStruct);
    reallocRayMembers(ray, 0);
    free(ray);
    DEBUG(1,"out\n");
}
