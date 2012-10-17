/****************************************************************************************
 *  calcAmpDelPr.c                                                                      *
 *  (formerly "caladp.for")                                                             *
 *  Calculates Amplitudes and arrivals using Regula Falsi method.                       *
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
 *          "aad.mat":  File containing arrival information.                            *
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

void calcAmpDelRF(settings_t*);

void calcAmpDelRF(settings_t* settings){
        DEBUG(1,"in\n");
    float          thetai, ctheta;
    uintptr_t       i, j, k, l, nRays, iHyd = 0;
    uintptr_t       nPossibleArrivals, nFoundArrivals = 0;
    float          zRay, zHyd, rHyd;
    float          junkfloat;
    float          maxNumArrivals=0;       //keeps track of the highest number of arrivals
    uint32_t        nTrial;
    float          theta0, f0;
    //used for root-finding in actual Regula-Falsi Method:
    float          fl, fr, prod;
    float*         thetaL              = NULL;
    float*         thetaR              = NULL;
    ray_t*          tempRay             = NULL;
    bool            success             = false;
    float*         thetas              = NULL;
    float**        depths              = NULL;
    ray_t*          ray                 = NULL;
    float*         dz                  = NULL;
    
    MATFile*        matfile             = NULL;
    mxArray*        pThetas             = NULL;
    mxArray*        pTitle              = NULL;
    mxArray*        pHydArrayR          = NULL;
    mxArray*        pHydArrayZ          = NULL;
    mxArray*        pSourceZ            = NULL;
    mxArray*        mxTheta             = NULL;
    mxArray*        mxR                 = NULL;
    mxArray*        mxZ                 = NULL;
    mxArray*        mxTau               = NULL;
    mxArray*        mxAmp               = NULL;
    mxArray*        iReturns            = NULL;
    mxArray*        nSurRefl            = NULL;
    mxArray*        nBotRefl            = NULL;
    mxArray*        nObjRefl            = NULL;
    mxArray*        mxAadStruct         = NULL;     //contains the arrivals at a single hydrophone
    mxArray*        mxNumArrivals       = NULL;
    mxArray*        mxRHyd              = NULL;
    mxArray*        mxZHyd              = NULL;
    const char*     aadFieldNames[]     = { "nArrivals", "rHyd", "zHyd", "arrival" };
    const char*     arrivalFieldNames[] = { "theta",
                                            "r",
                                            "z",
                                            "tau",
                                            "amp",
                                            "iReturns",
                                            "nSurRefl",
                                            "nBotRefl",
                                            "nObjRefl"};        //the names of the fields contained in mxArrivalStruct

    MWINDEX         idx[2];                     //used for accessing a specific element in the mxAadStruct
    
    arrivals_t      arrivals[settings->output.nArrayR][settings->output.nArrayZ];
    /*
     * arrivals[][] is an array with the dimensions of the hydrophone array and will contain the
     * actual arrival information before it is written to a matlab structure at the end of the file.
     */
    //initialize to 0
    for (i=0; i<settings->output.nArrayR; i++){
        for (j=0; j<settings->output.nArrayZ; j++){
            arrivals[i][j].nArrivals = 0;
            arrivals[i][j].mxArrivalStruct = mxCreateStructMatrix(  (MWSIZE)settings->source.nThetas,       //number of rows
                                                                        (MWSIZE)1,                              //number of columns
                                                                        9,                                      //number of fields in each element
                                                                        arrivalFieldNames);                     //list of field names
            if( arrivals[i][j].mxArrivalStruct == NULL ){
                fatal("Memory Alocation error.");
            }
        }
    }
    
    #if 1
    //Open matfile for output:
    matfile     = matOpen("aad.mat", "w");
    
    
    //write ray launching angles to matfile:
    pThetas     = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->source.nThetas, mxREAL);
    if(matfile == NULL || pThetas == NULL){
        fatal("Memory alocation error.");
    }
    //copy angles in cArray to mxArray:
    copyFloatToMxArray(    settings->source.thetas, pThetas , settings->source.nThetas);
    matPutVariable(matfile, "thetas", pThetas);
    mxDestroyArray(pThetas);
    
    
    //write title to matfile:
    pTitle = mxCreateString("TRACEO: EIGenrays (by Regula Falsi)");
    if(pTitle == NULL){
        fatal("Memory alocation error.");
    }
    matPutVariable(matfile, "caseTitle", pTitle);
    mxDestroyArray(pTitle);
    
    
    //write hydrophone array ranges to file:
    pHydArrayR = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayR, mxREAL);
    if(pHydArrayR == NULL){
        fatal("Memory alocation error.");
    }
    copyFloatToMxArray(    settings->output.arrayR, pHydArrayR, (uintptr_t)settings->output.nArrayR);
    matPutVariable(matfile, "arrayR", pHydArrayR);
    mxDestroyArray(pHydArrayR);
    
    
    //write hydrophone array depths to file:
    pHydArrayZ          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
    if(pHydArrayZ == NULL){
        fatal("Memory alocation error.");
    }
    copyFloatToMxArray(    settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
    matPutVariable(matfile, "arrayZ", pHydArrayZ);
    mxDestroyArray(pHydArrayZ);
    
    
    //write source depth to file:
    pSourceZ            = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
    if(pSourceZ == NULL){
        fatal("Memory alocation error.");
    }
    copyFloatToMxArray(&settings->source.zx, pSourceZ, 1);
    matPutVariable(matfile, "sourceZ", pSourceZ);
    mxDestroyArray(pSourceZ);
    
    
    //allocate memory for rays and auxiliary variables:
    ray = makeRay(settings->source.nThetas);
    #endif
    
    /**********************************************************************************************
     *  1)  Create a set of arrays (thetas[], depths[][]) that relate the launching angles of the 
     *      rays with their depth at each of the hydrophone array's depths:
     */
    thetas = mallocFloat(settings->source.nThetas);
    depths = mallocFloat2D(settings->source.nThetas, settings->output.nArrayR);
    DEBUG(2,"Calculting preliminary rays:\n");
    nRays = 0;
    
    for(i=0; i<settings->source.nThetas; i++){
        DEBUG(3, "--\n\t\tRay Launching angle: %lf\n", settings->source.thetas[i]);
        thetai = -settings->source.thetas[i]*M_PI/180.0;
        ray[i].theta = thetai;
        ctheta = fabs( cos( thetai ) );
        
        //  Trace a ray as long as it is neither 90 nor -90:
        if (ctheta > 1.0e-7){
            thetas[nRays] = thetai;
            DEBUG(3, "thetas[%u]: %e\n", (uint32_t)nRays, thetas[nRays]);
            solveEikonalEq(settings, &ray[i]);
            solveDynamicEq(settings, &ray[i]);
            
            if (ray[i].iReturn == true){
                printf("Eigenray search by Regula Falsi detected a returning ray at angle %lf.\n", thetas[i]);
                fatal("Returning eigenrays can only be determined by Proximity.\nAborting");
            }
            
            //Ray calculted; now fill the matrix of depths: 
            for(j=0; j<settings->output.nArrayR; j++){
                rHyd = settings->output.arrayR[j];
                
                //check if the hydrophone range coord is whithin range of the ray
                if ( (rHyd >= ray[i].rMin) && (rHyd <= ray[i].rMax)){
                    
                    //find bracketing coords:
                    bracket( ray[i].nCoords, ray[i].r, rHyd, &iHyd);
                    
                    //interpolate the ray depth at the range coord of hydrophone
                    intLinear1D(&ray[i].r[iHyd], &ray[i].z[iHyd], rHyd, &zRay, &junkfloat);
                    depths[nRays][j] = zRay;
                    DEBUG(3,"rHyd: %lf; rMin: %lf; rMax: %lf\n", rHyd, ray[i].rMin, ray[i].rMax);
                    DEBUG(3,"nCoords: %u, rHyd: %lf; iHyd: %u, zRay: %lf\n", (uint32_t)ray[i].nCoords, rHyd, (uint32_t)iHyd, zRay);
                }else{
                    depths[nRays][j] = NAN;
                }
            }
            reallocRayMembers(&ray[i],0);
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
    dz =        mallocFloat(nRays);
    thetaL =    mallocFloat(nRays);
    thetaR =    mallocFloat(nRays);

    //  iterate over....
    for (i=0; i<settings->output.nArrayR; i++){
        rHyd = settings->output.arrayR[i];
        //  ...the entire hydrophone array:
        for(j=0; j<settings->output.nArrayZ; j++){
            zHyd = settings->output.arrayZ[j];
            DEBUG(3, "i: %u; j: %u; rHyd:%lf, zHyd:%lf\n",(uint32_t)i, (uint32_t)j, rHyd, zHyd );
            
            //for each ray calculate the difference between the hydrophone and ray depths:
            for(k=0; k<nRays; k++){
                dz[k] = zHyd - depths[k][i];
                DEBUG(3,"dz[%u]= %lf\n", (uint32_t)k, dz[k]);
            }
            
            /** By looking at sign variations (or zero values) of dz[]:
             *      :: determine the number of possible arrivals 
             *      :: find the launching angles of adjacent rays that pass above and below (named L and R) a hydrophone
             *          (which implies that there may be an intermediate launching angle that corresponds to an eigenray.
             */
            nPossibleArrivals = 0;
            for(k=0; k<nRays-1; k++){
                fl = dz[k];
                fr = dz[k+1];
                prod = fl*fr;
                DEBUG(3, "k: %u; thetaL: %e; thetaR: %e\n", (uint32_t)k, thetaL[k], thetaR[k]);
                
                if( isnan_d(depths[k][i]) == false  &&
                    isnan_d(depths[k+1][i]) == false    ){
                    DEBUG(3, "Not a NAN\n");
                    
                    if( (fl == 0.0) && (fr != 0.0)){
                        thetaL[nPossibleArrivals] = thetas[k];
                        thetaR[nPossibleArrivals] = thetas[k+1];
                        nPossibleArrivals++;
                    
                    }else if(   (fr == 0.0) && (fl != 0.0)){
                        thetaL[nPossibleArrivals] = thetas[k];
                        thetaR[nPossibleArrivals] = thetas[k+1];
                        nPossibleArrivals++;
                    
                    }else if(prod < 0.0){
                        thetaL[nPossibleArrivals] = thetas[k];
                        thetaR[nPossibleArrivals] = thetas[k+1];
                        nPossibleArrivals++;
                    
                    }
                    DEBUG(3, "thetaL: %e, thetaR: %e\n", thetaL[nPossibleArrivals-1], thetaR[nPossibleArrivals-1]);
                }else{
                    DEBUG(4, "Its a NAN\n");
                }
                if (nPossibleArrivals > nRays){
                    //this should not be possible. TODO replace by assertion?
                    fatal("The impossible happened.\nNumber of possible eigenrays exceeds number of calculated rays.\nAborting.");
                }
            }
            
            //Time to find arrivals; either we are lucky or we need to apply regula falsi:
            /** We now know how many possible arrivals this hydrophone has (nPossibleArrivals),
             *  and for each of them we have the bracketing launching angles.
             *  It is now time to determine the "exact" launching angle of each eigenray.
             */
            DEBUG(3, "nPossibleArrivals: %u\n", (uint32_t)nPossibleArrivals);
            
            #if 0
            /*
            //create mxStructArray:
            mxRayStruct = mxCreateStructMatrix( (MWSIZE)nPossibleArrivals,  //number of rows
                                                (MWSIZE)1,                  //number of columns
                                                5,                          //number of fields in each element
                                                fieldNames);                //list of field names
            if( mxRayStruct == NULL ) {
                fatal("Memory Alocation error.");
            }
            */
            #endif
            
            tempRay = makeRay(1);
            nFoundArrivals = 0;
            for(l=0; l<nPossibleArrivals; l++){     //Note that if nPossibleArrivals = 0 this loop will not be executed:
                settings->source.rbox2 = rHyd + 1;
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
                    nFoundArrivals++;
                    success = true;
                
                }else if (fabs(fr) <= settings->output.miss){
                    DEBUG(3, "\"right\" is eigenray.\n");
                    theta0 = thetaR[l];
                    nFoundArrivals++;
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
                            //printf("(rHyd,zHyd)= %e, %e\n", rHyd, zHyd);
                            //printf("Eigenray search failure, skipping to next case...\n");
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
                        
                        //check if the new rays is close enough to the hydrophone to be considered and eigenray:
                        if (fabs(f0) < settings->output.miss){
                            DEBUG(3, "Found eigenray by applying Regula-Falsi.\n");
                            success = true;
                            nFoundArrivals++;
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
                    //DEBUG(3, "iFail: %u\n", iFail);
                }
                //DEBUG(3,"iFail: %u\n", (uint32_t)iFail);
                if (success == true){
                    
                    //finally: get arrivals from the coordinates and amplitudes of the eigenray
                    tempRay[0].theta = theta0;
                    solveEikonalEq(settings, tempRay);
                    solveDynamicEq(settings, tempRay);
                    
                    
                    ///prepare to save ray to mxStructArray:
                    //create mxArrays:
                    mxTheta = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
                    mxR     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
                    mxZ     = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
                    mxTau   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
                    mxAmp   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxCOMPLEX);
                    if( mxTheta == NULL || mxR == NULL || mxZ == NULL || mxTau == NULL || mxAmp == NULL){
                        fatal("Memory alocation error.");
                    }
                    
                    //copy data to mxArrays:
                    copyFloatToMxArray(&tempRay[0].theta,                      mxTheta,1);
                    copyFloatToMxArray(&tempRay->r[tempRay->nCoords - 1],      mxR,    1);
                    copyFloatToMxArray(&tempRay->z[tempRay->nCoords - 1],      mxZ,    1);
                    copyFloatToMxArray(&tempRay->tau[tempRay->nCoords - 1],    mxTau,  1);
                    copyComplexFloatToMxArray(&tempRay->amp[tempRay->nCoords - 2],   mxAmp,  1);     //TODO: correct this
                    
                    //copy mxArrays to mxRayStruct
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct,                 //pointer to the mxStruct
                                        (MWINDEX)arrivals[i][j].nArrivals,              //index of the element (number of ray)
                                        0,                                              //position of the field (in this case, field 0 is "r"
                                        mxTheta);                                       //the mxArray we want to copy into the mxStruct
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 1, mxR);
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 2, mxZ);
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 3, mxTau);
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 4, mxAmp);
                    ///arrival has been saved to mxStructArray
                    
                    ///now lets save some aditional ray information:
                    iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                    nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                    copyBoolToMxArray(      &tempRay->iReturn,  iReturns,   1);
                    copyUInt32ToMxArray(    &tempRay->sRefl,    nSurRefl,   1);
                    copyUInt32ToMxArray(    &tempRay->bRefl,    nBotRefl,   1);
                    copyUInt32ToMxArray(    &tempRay->oRefl,    nObjRefl,   1);

                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 5, iReturns);
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 6, nSurRefl);
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 7, nBotRefl);
                    mxSetFieldByNumber( arrivals[i][j].mxArrivalStruct, (MWINDEX)arrivals[i][j].nArrivals, 8, nObjRefl);
                    ///aditional information has been saved

                    arrivals[i][j].nArrivals += 1;
                    maxNumArrivals = max(arrivals[i][j].nArrivals, maxNumArrivals);
                }
            }
            DEBUG(3, "nFoundArrivals: %u\n", (uint32_t)nFoundArrivals);
        }
    }
    
    
    //write "maximum number of arrivals at any single hydrophone" to matfile:
    mxNumArrivals = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
    copyFloatToMxArray(&maxNumArrivals, mxNumArrivals, 1);
    matPutVariable(matfile, "maxNumArrivals", mxNumArrivals);
    
    
    //copy arrival data to mxAadStruct:
    mxAadStruct = mxCreateStructMatrix( (MWSIZE)settings->output.nArrayZ,   //number of rows
                                        (MWSIZE)settings->output.nArrayR,   //number of columns
                                        4,              //number of fields in each element
                                        aadFieldNames); //list of field names
    if( mxAadStruct == NULL ) {
        fatal("Memory Alocation error.");
    }
    for (i=0; i<settings->output.nArrayR; i++){
        for (j=0; j<settings->output.nArrayZ; j++){
            mxNumArrivals   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxRHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxZHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            
            copyFloatToMxArray(&arrivals[i][j].nArrivals,  mxNumArrivals,1);
            copyFloatToMxArray(&settings->output.arrayR[i],    mxRHyd,1);
            copyFloatToMxArray(&settings->output.arrayZ[j],    mxZHyd,1);
            
            idx[0] = (MWINDEX)j;
            idx[1] = (MWINDEX)i;
            mxSetFieldByNumber( mxAadStruct,                                    //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                0,                                              //position of the field (in this case, field 0 is "theta"
                                mxNumArrivals);                                 //the mxArray we want to copy into the mxStruct
            
            mxSetFieldByNumber( mxAadStruct,                                    //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                1,                                              //position of the field (in this case, field 0 is "theta"
                                mxRHyd);                                        //the mxArray we want to copy into the mxStruct
            
            mxSetFieldByNumber( mxAadStruct,                        //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                2,                                          //position of the field (in this case, field 0 is "theta"
                                mxZHyd);                                    //the mxArray we want to copy into the mxStruct
            
            mxSetFieldByNumber( mxAadStruct,                                    //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                3,                                                      //position of the field (in this case, field 0 is "theta"
                                arrivals[i][j].mxArrivalStruct);                        //the mxArray we want to copy into the mxStruct
        }
    }
    
    
    ///Write Eigenrays to matfile:
    matPutVariable(matfile, "arrivals", mxAadStruct);
    
    //Free memory
    matClose(matfile);
    mxDestroyArray(mxAadStruct);
    
    reallocRayMembers(tempRay, 0);
    free(tempRay);
    free(dz);
    DEBUG(1,"out\n");
}
