/****************************************************************************************
 *  calcEigenrayRF.c                                                                    *
 *  (formerly "calerf.for")                                                             *
 *  Calculates eigenrays using Regula Falsi method.                                     *
 *  Note that this will only work with rays traveling from left to right and            *
 *  for cases where there are no returning rays.                                        *
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

void    calcEigenrayRF(settings_t*);

void    calcEigenrayRF(settings_t* settings){
    DEBUG(1,"in\n");
    double          thetai, ctheta;
    uintptr_t       iRay, iArrayR, iArrayZ, l, iHyd = 0;     //iterator variables
    uintptr_t       nRays;   //a counter for the number of found eigenrays
    uintptr_t       nPossibleEigenRays, nFoundEigenRays = 0;
    double          zRay, zHyd, rHyd;
    double          junkDouble;
    uint32_t        nTrial;
    double          theta0, f0;
    //used for root-finding in actual Regula-Falsi Method:
    double          fl, fr, prod;
    double*         thetaL              = NULL;
    double*         thetaR              = NULL;
    ray_t*          tempRay             = NULL;
    bool            success             = false;
    double*         thetas              = NULL;
    double**        depths              = NULL;
    ray_t*          ray                 = NULL;
    double*         dz                  = NULL;
    uint32_t        maxNumEigenrays     = 0;

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
    mxArray*        mxAllEigenraysStruct= NULL;     //contains all the eigenrays at all hydrophones
    mxArray*        mxNumEigenrays      = NULL;
    mxArray*        mxRHyd              = NULL;
    mxArray*        mxZHyd              = NULL;
    const char*     AllEigenrayFieldNames[]= {  "nEigenrays", "rHyd", "zHyd", "eigenray" };
    const char*     eigenrayFieldNames[]= { "theta",
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
    MWINDEX         idx[2];         //used for accessing a specific element in the mxAllEigenrayStruct

    eigenrays_t     eigenrays[settings->output.nArrayR][settings->output.nArrayZ];
    /*
     * arrivals[][] is an array with the dimensions of the hydrophone array and will contain the
     * actual arrival information before it is written to a matlab structure at the end of the file.
     */
    //initialize to 0
    for (iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
        for (iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
            eigenrays[iArrayR][iArrayZ].nEigenrays = 0;
            eigenrays[iArrayR][iArrayZ].mxEigenrayStruct = mxCreateStructMatrix((MWSIZE)settings->source.nThetas,       //number of rows
                                                                                (MWSIZE)1,                              //number of columns
                                                                                12,                                     //number of fields in each element
                                                                                eigenrayFieldNames);                        //list of field names
            if( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct == NULL ){
                fatal("Memory Alocation error.");
            }
        }
    }

    #if 1
    //write ray launching angles to matfile:
    pThetas     = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
    if(pThetas == NULL){
        fatal("Memory alocation error.");
    }
    //copy angles in cArray to mxArray:
    copyDoubleToMxArray(    settings->source.thetas, pThetas , settings->source.nThetas);
    matPutVariable(settings->options.matfile, "thetas", pThetas);
    mxDestroyArray(pThetas);


    //write hydrophone array ranges to file:
    pHydArrayR = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
    if(pHydArrayR == NULL){
        fatal("Memory alocation error.");
    }
    copyDoubleToMxArray( settings->output.arrayR, pHydArrayR, (uintptr_t)settings->output.nArrayR);
    matPutVariable(settings->options.matfile, "rarray", pHydArrayR);
    mxDestroyArray(pHydArrayR);


    //write hydrophone array depths to file:
    pHydArrayZ          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
    if(pHydArrayZ == NULL){
        fatal("Memory alocation error.");
    }
    copyDoubleToMxArray( settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
    matPutVariable(settings->options.matfile, "zarray", pHydArrayZ);
    mxDestroyArray(pHydArrayZ);


    //allocate memory for rays and auxiliary variables:
    ray = makeRay(settings->source.nThetas);
    #endif

    /**********************************************************************************************
     *  1)  Create a set of arrays (thetas[], depths[][]) that relate the launching angles of the
     *      rays with their depth at each of the hydrophone array's depths:
     */
    thetas = mallocDouble(settings->source.nThetas);
    depths = mallocDouble2D(settings->source.nThetas, settings->output.nArrayR);
    DEBUG(2,"Calculting preliminary rays:\n");
    nRays = 0;      //a counter for the number of found eigenrays

    for(iRay=0; iRay<settings->source.nThetas; iRay++){
        DEBUG(3, "--\n\t\tRay Launching angle: %lf\n", settings->source.thetas[i]);
        thetai = -settings->source.thetas[iRay]*M_PI/180.0;
        ray[iRay].theta = thetai;
        ctheta = fabs( cos( thetai ) );

        //  Trace a ray as long as it is neither 90 nor -90:
        if (ctheta > 1.0e-7){
            thetas[nRays] = thetai;
            DEBUG(3, "thetas[%u]: %e\n", (uint32_t)nRays, thetas[nRays]);
            solveEikonalEq(settings, &ray[iRay]);
            solveDynamicEq(settings, &ray[iRay]);

            if (ray[iRay].iReturn == true){
                printf("Eigenray search by Regula Falsi detected a returning ray at angle %lf.\n", thetas[iRay]);
                fatal("Returning eigenrays can only be determined by Proximity.\nAborting");
            }

            //Ray calculted; now fill the matrix of depths:
            for(iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
                rHyd = settings->output.arrayR[iArrayR];

                //check if the hydrophone range coord is whithin range of the ray
                if ( (rHyd >= ray[iRay].rMin) && (rHyd <= ray[iRay].rMax)){

                    //find bracketing coords:
                    bracket( ray[iRay].nCoords, ray[iRay].r, rHyd, &iHyd);

                    //interpolate the ray depth at the range coord of hydrophone
                    intLinear1D(&ray[iRay].r[iHyd], &ray[iRay].z[iHyd], rHyd, &zRay, &junkDouble);
                    depths[nRays][iArrayR] = zRay;
                    DEBUG(3,"rHyd: %lf; rMin: %lf; rMax: %lf\n", rHyd, ray[iRay].rMin, ray[iRay].rMax);
                    DEBUG(3,"nCoords: %u, rHyd: %lf; iHyd: %u, zRay: %lf\n", (uint32_t)ray[iRay].nCoords, rHyd, (uint32_t)iHyd, zRay);
                }else{
                    depths[nRays][iArrayR] = NAN;
                }
            }
            reallocRayMembers(&ray[iRay],0);
            nRays++;
        }
    }
    free(ray);
    /** 1)  Done.
     */
    DEBUG(3, "Preliminary rays calculated.\n");

    /********************************************************************************
     *  2)  Proceed to searching for possible eigenrays at each point of the array:
     */

    //allocate memory for some temporary variables
    dz =        mallocDouble(nRays);
    thetaL =    mallocDouble(nRays);
    thetaR =    mallocDouble(nRays);

    //  iterate over....
    for (iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
        rHyd = settings->output.arrayR[iArrayR];
        //  ...the entire hydrophone array:
        for(iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
            zHyd = settings->output.arrayZ[iArrayZ];
            DEBUG(3, "iArrayR: %u; iArrayZ: %u; rHyd:%lf, zHyd:%lf\n",(uint32_t)iArrayR, (uint32_t)iArrayZ, rHyd, zHyd );

            //for each ray calculate the difference between the hydrophone and ray depths:
            for(iRay=0; iRay<nRays; iRay++){
                dz[iRay] = zHyd - depths[iRay][iArrayR];
                DEBUG(3,"dz[%u]= %lf\n", (uint32_t)iRay, dz[iRay]);
            }

            /** By looking at sign variations (or zero values) of dz[]:
             *      :: determine the number of possible eigenrays
             *      :: find the launching angles of adjacent rays that pass above and below (named L and R) a hydrophone
             *          (which implies that there may be an intermediate launching angle that corresponds to an eigenray.
             */
            nPossibleEigenRays = 0;
            for(iRay=0; iRay<nRays-1; iRay++){
                fl = dz[iRay];
                fr = dz[iRay+1];
                prod = fl*fr;
                DEBUG(3, "k: %u; thetaL: %e; thetaR: %e\n", (uint32_t)iRay, thetaL[iRay], thetaR[iRay]);

                if( isnan_d(depths[iRay][iArrayR]) == false  &&
                    isnan_d(depths[iRay+1][iArrayR]) == false    ){
                    DEBUG(3, "Not a NAN\n");

                    if( (fl == 0.0) && (fr != 0.0)){
                        thetaL[nPossibleEigenRays] = thetas[iRay];
                        thetaR[nPossibleEigenRays] = thetas[iRay+1];
                        nPossibleEigenRays++;

                    }else if(   (fr == 0.0) && (fl != 0.0)){
                        thetaL[nPossibleEigenRays] = thetas[iRay];
                        thetaR[nPossibleEigenRays] = thetas[iRay+1];
                        nPossibleEigenRays++;

                    }else if(prod < 0.0){
                        thetaL[nPossibleEigenRays] = thetas[iRay];
                        thetaR[nPossibleEigenRays] = thetas[iRay+1];
                        nPossibleEigenRays++;

                    }
                    DEBUG(3, "thetaL: %e, thetaR: %e\n", thetaL[nPossibleEigenRays-1], thetaR[nPossibleEigenRays-1]);
                }else{
                    DEBUG(4, "Its a NAN\n");
                }
                if (nPossibleEigenRays > nRays){
                    //this should not be possible. TODO replace by assertion?
                    fatal("The impossible happened.\nNumber of possible eigenrays exceeds number of calculated rays.\nAborting.");
                }
            }

            //Time to find eigenrays; either we are lucky or we need to apply regula falsi:
            /** We now know how many possible eigenrays this hydrophone has (nPossibleEigenRays),
             *  and for each of them we have the bracketing launching angles.
             *  It is now time to determine the "exact" launching angle of each eigenray.
             */
            DEBUG(3, "nPossibleEigenRays: %u\n", (uint32_t)nPossibleEigenRays);

            #if 0
            /*
            //create mxStructArray:
            mxRayStruct = mxCreateStructMatrix( (MWSIZE)nPossibleEigenRays, //number of rows
                                                (MWSIZE)1,                  //number of columns
                                                5,                          //number of fields in each element
                                                fieldNames);                //list of field names
            if( mxRayStruct == NULL ) {
                fatal("Memory Alocation error.");
            }
            */
            #endif

            tempRay = makeRay(1);
            nFoundEigenRays = 0;
            for(l=0; l<nPossibleEigenRays; l++){        //Note that if nPossibleEigenRays = 0 this loop will not be executed:
                settings->source.rbox2 = rHyd + 1.1*settings->source.ds;  //TODO: change this to "rbox2 = rHyd + ds" and verify results.
                DEBUG(3,"l: %u\n", (uint32_t)l);

                //Determine "left" ray's depth at rHyd:
                tempRay[0].theta = thetaL[l];
                solveEikonalEq(settings, tempRay);
                fl = tempRay[0].z[tempRay[0].nCoords-1] - zHyd;
                //reset the ray members to zero:
                reallocRayMembers(tempRay, 0);

                //Determine "right" ray's depth at rHyd:
                tempRay[0].theta = thetaR[l];
                solveEikonalEq(settings, tempRay);
                fr = tempRay[0].z[tempRay[0].nCoords-1] - zHyd;
                //reset the ray members to zero:
                reallocRayMembers(tempRay, 0);

                //check if either the "left" or "right" ray pass at a distance within the defined threshold
                if (fabs(fl) <= settings->output.miss){
                    DEBUG(3, "\"left\" is eigenray.\n");
                    theta0 = thetaL[l];
                    nFoundEigenRays++;
                    success = true;

                }else if (fabs(fr) <= settings->output.miss){
                    DEBUG(3, "\"right\" is eigenray.\n");
                    theta0 = thetaR[l];
                    nFoundEigenRays++;
                    success = true;

                //if not, try to find the "exact" launching angle
                }else{
                    DEBUG(3, "Neither \"left\" nor \"right\" ray are close enough to be eigenrays.\nApplying Regula-Falsi...\n");
                    nTrial = 0;
                    success = false;

                    //here comes the actual Regula-Falsi loop:
                    while(success == false){
                        nTrial++;

                        if (nTrial > 21){
                            DEBUG(3, "(rHyd,zHyd)= %e, %e : Eigenray search failure, skipping to next case...\n" rHyd, zHyd);
                            break;
                        }

                        theta0 = thetaR[l] - fr*( thetaL[l] - thetaR[l] )/( fl - fr );
                        DEBUG(3, "l: %u; thetaR[l]: %e; thetaL[l]: %e; theta0: %e; fl: %e; fr: %e;\n",
                                (uint32_t)l, thetaR[l], thetaL[l],          theta0,     fl,     fr);

                        //find the distance between the new ray and the hydrophone:
                        tempRay[0].theta = theta0;
                        solveEikonalEq(settings, tempRay);
                        f0 = tempRay[0].z[tempRay[0].nCoords-1] - zHyd;
                        //reset the ray members to zero:
                        DEBUG(3, "nCoords: %u\n", (uint32_t)tempRay[0].nCoords);
                        reallocRayMembers(tempRay, 0);
                        DEBUG(3, "zHyd: %e; miss: %e, nTrial: %u, f0: %e\n", zHyd, settings->output.miss, (uint32_t)nTrial, f0);

                        //check if the new ray is close enough to the hydrophone to be considered and eigenray:
                        if (fabs(f0) < settings->output.miss){
                            DEBUG(3, "Found eigenray by applying Regula-Falsi.\n");
                            success = true;
                            nFoundEigenRays++;
                            break;

                        //if the root wasn't found, do another Regula-Falsi iterarion:
                        }else{
                            DEBUG(3, "ASD\n");
                            prod = fl*f0;

                            if ( prod < 0.0 ){
                                DEBUG(3, "ASD1\n");
                                thetaR[l] = theta0;
                                fr = f0;
                            }else{
                                DEBUG(3, "ASD2\n");
                                thetaL[l] = theta0;
                                fl = f0;
                            }
                        }
                    }//while()
                }
                if (success == true){

                    //finally: get the coordinates and amplitudes of the eigenray
                    tempRay[0].theta = theta0;
                    solveEikonalEq(settings, tempRay);
                    solveDynamicEq(settings, tempRay);


                    ///prepare to save ray to mxStructArray:
                    //create mxArrays:
                    //NOTE: on line 319 we resize the rangeBox to make sure we compute valid ray amplitude for the last ray coordinate (which means integrating the ray to a point beyond the hydrophone, so here we copy on e coordinate less than has actually been computed)
                    mxTheta = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,                    mxREAL);
                    mxR     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(tempRay->nCoords-1), mxREAL);
                    mxZ     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(tempRay->nCoords-1), mxREAL);
                    mxTau   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(tempRay->nCoords-1), mxREAL);
                    mxAmp   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)(tempRay->nCoords-1), mxCOMPLEX);
                    if( mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
                        fatal("Memory alocation error.");
                    }

                    //copy data to mxArrays:
                    copyDoubleToMxArray(&tempRay[0].theta,  mxTheta,1);
                    copyDoubleToMxArray(tempRay->r,         mxR,    tempRay->nCoords-1);
                    copyDoubleToMxArray(tempRay->z,         mxZ,    tempRay->nCoords-1);
                    copyDoubleToMxArray(tempRay->tau,       mxTau,  tempRay->nCoords-1);
                    copyComplexToMxArray(tempRay->amp,      mxAmp,  tempRay->nCoords-1);

                    //copy mxArrays to mxRayStruct
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct,               //pointer to the mxStruct
                                        (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays,            //index of the element (number of ray)
                                        0,                                              //position of the field (in this case, field 0 is "r"
                                        mxTheta);                                       //the mxArray we want to copy into the mxStruct
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 1, mxR);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 2, mxZ);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 3, mxTau);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 4, mxAmp);
                    ///ray has been saved to mxStructArray

                    ///now lets save some aditional ray information:
                    iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nRefrac     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                    copyBoolToMxArray(      &tempRay->iReturn,  iReturns,   1);
                    copyUInt32ToMxArray(    &tempRay->sRefl,    nSurRefl,   1);
                    copyUInt32ToMxArray(    &tempRay->bRefl,    nBotRefl,   1);
                    copyUInt32ToMxArray(    &tempRay->oRefl,    nObjRefl,   1);
                    copyUInt32ToMxArray(    &tempRay->nRefrac,  nRefrac,    1);

                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 5, iReturns);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 6, nSurRefl);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 7, nBotRefl);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 8, nObjRefl);
                    mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 9, nRefrac);
                    ///aditional information has been saved

                    ///save refraction coordinates to structure:
                    if (tempRay->nRefrac > 0){
                        mxRefrac_r = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)tempRay->nRefrac, mxREAL);
                        mxRefrac_z = mxCreateDoubleMatrix((MWSIZE)1,    (MWSIZE)tempRay->nRefrac, mxREAL);

                        copyDoubleToMxArray(tempRay->rRefrac,   mxRefrac_r, tempRay->nRefrac);
                        copyDoubleToMxArray(tempRay->zRefrac,   mxRefrac_z, tempRay->nRefrac);

                        mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 10, mxRefrac_r);
                        mxSetFieldByNumber( eigenrays[iArrayR][iArrayZ].mxEigenrayStruct, (MWINDEX)eigenrays[iArrayR][iArrayZ].nEigenrays, 11, mxRefrac_z);
                    }
                    eigenrays[iArrayR][iArrayZ].nEigenrays += 1;
                    maxNumEigenrays = max(eigenrays[iArrayR][iArrayZ].nEigenrays, maxNumEigenrays);
                }
            }
            DEBUG(3, "nFoundEigenRays: %u\n", (uint32_t)nFoundEigenRays);
        }
    }
    
    

    //write "maximum number of eigenrays at any of the hydrophones
    mxMaxNumEigenrays = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
    copyUInt32ToMxArray(&maxNumEigenrays, mxMaxNumEigenrays, 1);
    matPutVariable(settings->options.matfile, "maxNumEigenrays", mxMaxNumEigenrays);
    mxDestroyArray(mxMaxNumEigenrays);
    
    ///Write Eigenrays to matfile:
    //copy arrival data to mxAllEigenraysStruct:
    mxAllEigenraysStruct = mxCreateStructMatrix((MWSIZE)settings->output.nArrayZ,   //number of rows
                                                (MWSIZE)settings->output.nArrayR,   //number of columns
                                                4,                                  //number of fields in each element
                                                AllEigenrayFieldNames);             //list of field names
    if( mxAllEigenraysStruct == NULL ){
        fatal("Memory Alocation error.");
    }
    for (iArrayR=0; iArrayR<settings->output.nArrayR; iArrayR++){
        for (iArrayZ=0; iArrayZ<settings->output.nArrayZ; iArrayZ++){
            mxNumEigenrays  = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxRHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxZHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);

            copyDoubleToMxArray(&eigenrays[iArrayR][iArrayZ].nEigenrays,mxNumEigenrays,1);
            copyDoubleToMxArray(&settings->output.arrayR[iArrayR],      mxRHyd,1);
            copyDoubleToMxArray(&settings->output.arrayZ[iArrayZ],      mxZHyd,1);

            idx[0] = (MWINDEX)iArrayZ;
            idx[1] = (MWINDEX)iArrayR;
            mxSetFieldByNumber( mxAllEigenraysStruct,                                   //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                0,                                                      //position of the field (in this case, field 0 is "theta"
                                mxNumEigenrays);                                        //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAllEigenraysStruct,                                   //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                1,                                                      //position of the field (in this case, field 0 is "theta"
                                mxRHyd);                                                //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAllEigenraysStruct,                                   //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                2,                                                      //position of the field (in this case, field 0 is "theta"
                                mxZHyd);                                                //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAllEigenraysStruct,                                   //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAllEigenraysStruct, 2, idx),    //index of the element
                                3,                                                      //position of the field (in this case, field 0 is "theta"
                                eigenrays[iArrayR][iArrayZ].mxEigenrayStruct);                //the mxArray we want to copy into the mxStruct
        }
    }

    ///Write Eigenrays to matfile:
    matPutVariable(settings->options.matfile, "eigenrays", mxAllEigenraysStruct);
    

    
    //Free memory
    mxDestroyArray(mxAllEigenraysStruct);
    free(dz);
    DEBUG(1,"out\n");
}







