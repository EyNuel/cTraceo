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
    double          thetai, ctheta;
    double          junkDouble;
    ray_t*          ray         = NULL;
    uintptr_t       i, j, jj, l;
    double          rHyd, zHyd, zRay, tauRay;
    complex double  junkComplex, ampRay;
    double          dz;
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
    for (j=0; j<settings->output.nArrayR; j++){
        for (jj=0; jj<settings->output.nArrayZ; jj++){
            DEBUG(1, "initializing eigenray structure (j, jj) = (%lu, %lu)...", j, jj);
            eigenrays[j][jj].nEigenrays = 0;
            eigenrays[j][jj].mxEigenrayStruct = mxCreateStructMatrix(   (MWSIZE)settings->source.nThetas,       //number of rows
                                                                        (MWSIZE)1,                              //number of columns
                                                                        12,                                     //number of fields in each element
                                                                        arrivalFieldNames);                     //list of field names
            if( eigenrays[j][jj].mxEigenrayStruct == NULL ){
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
    copyDoubleToMxArray(settings->source.thetas, pThetas, settings->source.nThetas);
    //move mxArray to file and free memory:
    matPutVariable(settings->options.matfile, "thetas", pThetas);
    mxDestroyArray(pThetas);


    //write hydrophone array ranges to file:
    pHydArrayR          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
    if(pHydArrayR == NULL){
        fatal("Memory alocation error.");
    }
    copyDoubleToMxArray(    settings->output.arrayR, pHydArrayR, (uintptr_t)settings->output.nArrayR);
    matPutVariable(settings->options.matfile, "rarray", pHydArrayR);
    mxDestroyArray(pHydArrayR);


    //write hydrophone array depths to file:
    pHydArrayZ          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
    if(pHydArrayZ == NULL){
        fatal("Memory alocation error.");
    }
    copyDoubleToMxArray(    settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
    matPutVariable(settings->options.matfile, "zarray", pHydArrayZ);
    mxDestroyArray(pHydArrayZ);


    //allocate memory for the rays:
    ray = makeRay(settings->source.nThetas);
    #endif

    /** Trace the rays:  */
    for(i=0; i<settings->source.nThetas; i++){
        thetai = -settings->source.thetas[i] * M_PI/180.0;
        ray[i].theta = thetai;
        ctheta = fabs( cos(thetai));

        //Trace a ray as long as it is neither at 90 nor -90:
        if (ctheta > 1.0e-7){
            solveEikonalEq(settings, &ray[i]);
            solveDynamicEq(settings, &ray[i]);

            //test for proximity of ray to each hydrophone
            //(yes, this is slow, can you figure out a better way to do it?)
            for(j=0; j<settings->output.nArrayR; j++){
                rHyd = settings->output.arrayR[j];

                if ( (rHyd >= ray[i].rMin) && (rHyd <= ray[i].rMax)){

                    //  Check if the ray is returning back or not;
                    //  if not:     we can bracket it without problems,
                    //  otherwise:  we need to know how many times it passed by the given array range
                    if (ray[i].iReturn == false){

                        /*
                        //when debugging, save the coordinates of the last ray to a separate matfile before exiting.
                        #if VERBOSE
                        if((int)i==(int)4){
                            DEBUG(1,"i:%u, nCoords: %u\n", (uint32_t)i, (uint32_t)ray[i].nCoords);
                            DEBUG(1,"rMin: %e, rMax: %e\n",ray[i].rMin, ray[i].rMax);

                            mxArray*    pDyingRay   = NULL;
                            MATFile*    matfile2    = NULL;
                            double**    dyingRay    = malloc(2*sizeof(uintptr_t));
                            char*       string2     = mallocChar(10);

                            dyingRay[0] = ray[i].r;
                            dyingRay[1] = ray[i].z;
                            matfile2    = matOpen("dyingRay.mat", "w");
                            pDyingRay   = mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)ray[i].nCoords, mxREAL);
                            if(pDyingRay == NULL || matfile2 == NULL)
                                fatal("Memory alocation error.");
                            copyDoubleToPtr2D(dyingRay, mxGetPr(pDyingRay), ray[i].nCoords,2);
                            sprintf(string2, "dyingRay");
                            matPutVariable(matfile2, (const char*)string2, pDyingRay);

                            mxDestroyArray(pDyingRay);
                            matClose(matfile2);
                            DEBUG(1,"Dying ray written to file.\n");
                        }
                        #endif
                        */

                        //get the index of the lower bracketing element:
                        bracket(ray[i].nCoords, ray[i].r, rHyd, &iHyd);
                        DEBUG(3,"non-returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[i].nCoords, (uint32_t)iHyd);

                        //from index interpolate the rays' depth:
                        intLinear1D(        &ray[i].r[iHyd], &ray[i].z[iHyd],   rHyd, &zRay,    &junkDouble);

                        //for every hydrophone check distance to ray
                        for(jj=0; jj<settings->output.nArrayZ; jj++){
                            zHyd = settings->output.arrayZ[jj];
                            dz = fabs(zRay-zHyd);
                            DEBUG(4, "dz: %e\n", dz);

                            if (dz < settings->output.miss){
                                DEBUG(3, "Eigenray found\n");

                                //from index interpolate the rays' travel time and amplitude:
                                intLinear1D(        &ray[i].r[iHyd], &ray[i].tau[iHyd], rHyd, &tauRay,  &junkDouble);
                                intComplexLinear1D( &ray[i].r[iHyd], &ray[i].amp[iHyd], rHyd, &ampRay,  &junkComplex);

                                //adjust the ray's last set of coordinates so that it matches up with the hydrophone
                                ray[i].r[iHyd+1]    = rHyd;
                                ray[i].z[iHyd+1]    = zRay;
                                ray[i].tau[iHyd+1]  = tauRay;
                                ray[i].amp[iHyd+1]  = ampRay;

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
                                copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
                                copyDoubleToMxArray(ray[i].r,                   mxR,    iHyd+2);
                                copyDoubleToMxArray(ray[i].z,                   mxZ,    iHyd+2);
                                copyDoubleToMxArray(ray[i].tau,                 mxTau,  iHyd+2);
                                copyComplexToMxArray(ray[i].amp,                mxAmp,  iHyd+2);

                                //copy mxArrays to mxEigenrayStruct
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct,  //pointer to the mxStruct
                                                    (MWINDEX)eigenrays[j][jj].nEigenrays,   //index of the element
                                                    0,                                  //position of the field (in this case, field 0 is "theta"
                                                    mxTheta);                           //the mxArray we want to copy into the mxStruct
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 1, mxR);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 2, mxZ);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 3, mxTau);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 4, mxAmp);
                                ///Eigenray has been saved to mxAadStruct

                                ///now lets save some aditional ray information:
                                iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nRefrac     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                                copyBoolToMxArray(      &ray[i].iReturn,    iReturns,   1);
                                copyUInt32ToMxArray(    &ray[i].sRefl,  nSurRefl,   1);
                                copyUInt32ToMxArray(    &ray[i].bRefl,  nBotRefl,   1);
                                copyUInt32ToMxArray(    &ray[i].oRefl,  nObjRefl,   1);
                                copyUInt32ToMxArray(    &ray[i].nRefrac,    nRefrac,    1);

                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 5, iReturns);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 6, nSurRefl);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 7, nBotRefl);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 8, nObjRefl);
                                mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 9, nRefrac);
                                ///aditional information has been saved

                                ///save refraction coordinates to structure:
                                if (ray[i].nRefrac > 0){
                                    mxRefrac_r = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[i].nRefrac, mxREAL);
                                    mxRefrac_z = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[i].nRefrac, mxREAL);

                                    copyDoubleToMxArray(ray[i].rRefrac, mxRefrac_r, ray[i].nRefrac);
                                    copyDoubleToMxArray(ray[i].zRefrac, mxRefrac_z, ray[i].nRefrac);

                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 10, mxRefrac_r);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 11, mxRefrac_z);
                                }

                                eigenrays[j][jj].nEigenrays += 1;
                                maxNumEigenrays = max(eigenrays[j][jj].nEigenrays, maxNumEigenrays);
                            }// if (dz settings->output.miss)
                        }// for(jj=1; jj<=settings->output.nArrayZ; jj++)

                    }else{// if (ray[i].iReturn == false)

                        DEBUG(3,"returning ray: nCoords: %u, iHyd:%u\n", (uint32_t)ray[i].nCoords, (uint32_t)iHyd);
                        //get the indexes of the bracketing points.
                        eBracket(ray[i].nCoords, ray[i].r, rHyd, &nRet, iRet);

                        //for each index where the ray passes at the hydrophone, interpolate the rays' depth:
                        for(l=0; l<nRet; l++){
                            DEBUG(4, "nRet=%u, iRet[%u]= %u\n", (uint32_t)nRet, (uint32_t)l, (uint32_t)iRet[l]);
                            intLinear1D(        &ray[i].r[iRet[l]], &ray[i].z[iRet[l]],     rHyd, &zRay,    &junkDouble);

                            //for every hydrophone check if the ray is close enough to be considered an eigenray:
                            for(jj=0;jj<settings->output.nArrayZ; jj++){
                                zHyd = settings->output.arrayZ[jj];
                                dz = fabs( zRay - zHyd );

                                if (dz < settings->output.miss){

                                    //interpolate the ray's travel time and amplitude:
                                    intLinear1D(        &ray[i].r[iRet[l]], &ray[i].tau[iRet[l]],   rHyd, &tauRay,  &junkDouble);
                                    intComplexLinear1D( &ray[i].r[iRet[l]], &ray[i].amp[iRet[l]],   (complex double)rHyd, &ampRay,  &junkComplex);

                                    DEBUG(1, "i: %u, iHyd: %u, nCoords: %u\n", (uint32_t)i, (uint32_t)iHyd,(uint32_t)ray[i].nCoords);
                                    //adjust the ray's last set of coordinates so that it matches up with the hydrophone
                                    ray[i].r[iRet[l]+1] = rHyd;
                                    ray[i].z[iRet[l]+1] = zRay;
                                    ray[i].tau[iRet[l]+1]   = tauRay;
                                    ray[i].amp[iRet[l]+1]   = ampRay;

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
                                    copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
                                    copyDoubleToMxArray(ray[i].r,                   mxR,    iRet[l]+2);
                                    copyDoubleToMxArray(ray[i].z,                   mxZ,    iRet[l]+2);
                                    copyDoubleToMxArray(ray[i].tau,                 mxTau,  iRet[l]+2);
                                    copyComplexToMxArray(ray[i].amp,                mxAmp,  iRet[l]+2);

                                    //copy mxArrays to mxEigenrayStruct
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct,  //pointer to the mxStruct
                                                        (MWINDEX)eigenrays[j][jj].nEigenrays,   //index of the element
                                                        0,                                  //position of the field (in this case, field 0 is "theta"
                                                        mxTheta);                           //the mxArray we want to copy into the mxStruct
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 1, mxR);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 2, mxZ);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 3, mxTau);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)eigenrays[j][jj].nEigenrays, 4, mxAmp);
                                    ///Eigenray has been saved to mxAadStruct

                                    ///now lets save some aditional ray information:
                                    iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nRefrac     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                                    copyBoolToMxArray(      &ray[i].iReturn,    iReturns,   1);
                                    copyUInt32ToMxArray(    &ray[i].sRefl,  nSurRefl,   1);
                                    copyUInt32ToMxArray(    &ray[i].bRefl,  nBotRefl,   1);
                                    copyUInt32ToMxArray(    &ray[i].oRefl,  nObjRefl,   1);
                                    copyUInt32ToMxArray(    &ray[i].nRefrac,    nRefrac,    1);

                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 5, iReturns);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 6, nSurRefl);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 7, nBotRefl);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 8, nObjRefl);
                                    mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 9, nRefrac);
                                    ///aditional information has been saved

                                    ///save refraction coordinates to structure:
                                    if (ray[i].nRefrac > 0){
                                        mxRefrac_r = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[i].nRefrac, mxREAL);
                                        mxRefrac_z = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)ray[i].nRefrac, mxREAL);

                                        copyDoubleToMxArray(ray[i].rRefrac, mxRefrac_r, ray[i].nRefrac);
                                        copyDoubleToMxArray(ray[i].zRefrac, mxRefrac_z, ray[i].nRefrac);

                                        mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 10, mxRefrac_r);
                                        mxSetFieldByNumber( eigenrays[j][jj].mxEigenrayStruct, (MWINDEX)i, 11, mxRefrac_z);
                                    }

                                    eigenrays[j][jj].nEigenrays += 1;
                                    maxNumEigenrays = max(eigenrays[j][jj].nEigenrays, maxNumEigenrays);
                                }
                            }
                        }
                    }// if (ray[i].iReturn == false)
                }//if ( (rHyd >= ray[i].rMin) && (rHyd < ray[i].rMax))
            }//for(j=0; j<settings->output.nArrayR; j++){
            if(KEEP_RAYS_IN_MEM == false){
                //free the ray's memory
                reallocRayMembers(&ray[i],0);
            }
        }//if (ctheta > 1.0e-7)
    }//for(i=0; i<settings->source.nThetas; i++)
    
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

    for (j=0; j<settings->output.nArrayR; j++){
        for (jj=0; jj<settings->output.nArrayZ; jj++){
            mxNumEigenrays  = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxRHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxZHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);

            copyDoubleToMxArray(&eigenrays[j][jj].nEigenrays,   mxNumEigenrays,1);
            copyDoubleToMxArray(&settings->output.arrayR[j],    mxRHyd,1);
            copyDoubleToMxArray(&settings->output.arrayZ[jj],   mxZHyd,1);

            idx[0] = (MWINDEX)jj;
            idx[1] = (MWINDEX)j;
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
                                eigenrays[j][jj].mxEigenrayStruct);         //the mxArray we want to copy into the mxStruct
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
