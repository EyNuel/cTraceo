/****************************************************************************************
 *  calcAmpDelPr.c                                                                      *
 *  (formerly "caladp.for")                                                             *
 *  Calculates Amplitudes and Delays using proximity method.                            *
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
 *          "aad.mat":  File containing arrival information.                            *
 *                                                                                      *
 *  Return Value:                                                                       *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

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

void calcAmpDelPr(settings_t*);

void calcAmpDelPr(settings_t* settings){
    //NOTE: the code below is practically identical to calcEigenrayPr.c, the only difference being the file output
    DEBUG(1,"in\n");

    double          thetai, ctheta;
    double          junkDouble;
    double          maxNumArrivals=0;       //keeps track of the highest number of arrivals
    uintptr_t       i, j, jj, l;
    double          rHyd, zHyd, zRay, tauRay;
    complex double  junkComplex, ampRay;
    double          dz;
    uintptr_t       nRet, iHyd = 0;
    uintptr_t       iRet[51];
    ray_t*          ray                 = NULL;

    mxArray*        pThetas             = NULL;
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
    for (j=0; j<settings->output.nArrayR; j++){
        for (jj=0; jj<settings->output.nArrayZ; jj++){
            arrivals[j][jj].nArrivals = 0;
            arrivals[j][jj].mxArrivalStruct = mxCreateStructMatrix( (MWSIZE)settings->source.nThetas,       //number of rows
                                                                    (MWSIZE)1,                              //number of columns
                                                                    9,                                      //number of fields in each element
                                                                    arrivalFieldNames);                     //list of field names
            if( arrivals[j][jj].mxArrivalStruct == NULL ){
                fatal("Memory Alocation error.");
            }
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
    matPutVariable(settings->options.matfile, "arrayR", pHydArrayR);
    mxDestroyArray(pHydArrayR);


    //write hydrophone array depths to file:
    pHydArrayZ          = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)settings->output.nArrayZ, mxREAL);
    if(pHydArrayZ == NULL){
        fatal("Memory alocation error.");
    }
    copyDoubleToMxArray(    settings->output.arrayZ, pHydArrayZ, (uintptr_t)settings->output.nArrayZ);
    matPutVariable(settings->options.matfile, "arrayZ", pHydArrayZ);
    mxDestroyArray(pHydArrayZ);


    //write source dept to file:
    pSourceZ            = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
    if(pSourceZ == NULL){
        fatal("Memory alocation error.");
    }
    copyDoubleToMxArray(&settings->source.zx, pSourceZ, 1);
    matPutVariable(settings->options.matfile, "sourceZ", pSourceZ);
    mxDestroyArray(pSourceZ);


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

                                ///prepare to write arrival to matfile:
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
                                copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
                                copyDoubleToMxArray(&rHyd,                      mxR,    1);
                                copyDoubleToMxArray(&zRay,                      mxZ,    1);
                                copyDoubleToMxArray(&tauRay,                    mxTau,  1);
                                copyComplexToMxArray(&ampRay,                   mxAmp,  1);

                                //copy mxArrays to mxArrivalStruct
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct,    //pointer to the mxStruct
                                                    (MWINDEX)arrivals[j][jj].nArrivals, //index of the element
                                                    0,                                  //position of the field (in this case, field 0 is "theta"
                                                    mxTheta);                           //the mxArray we want to copy into the mxStruct
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 1, mxR);
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 2, mxZ);
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 3, mxTau);
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 4, mxAmp);
                                ///Arrival has been saved to mxAadStruct
                                
                                ///now lets save some aditional ray information:
                                iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                                copyBoolToMxArray(      &ray[i].iReturn,    iReturns,   1);
                                copyUInt32ToMxArray(    &ray[i].sRefl,  nSurRefl,   1);
                                copyUInt32ToMxArray(    &ray[i].bRefl,  nBotRefl,   1);
                                copyUInt32ToMxArray(    &ray[i].oRefl,  nObjRefl,   1);

                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 5, iReturns);
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 6, nSurRefl);
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 7, nBotRefl);
                                mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 8, nObjRefl);
                                ///aditional information has been saved
                                
                                arrivals[j][jj].nArrivals += 1;
                                maxNumArrivals = max(arrivals[j][jj].nArrivals, maxNumArrivals);
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

                                    ///prepare to write arrival to matfile:
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
                                    copyDoubleToMxArray(&settings->source.thetas[i],mxTheta,1);
                                    copyDoubleToMxArray(&rHyd,                      mxR,    1);
                                    copyDoubleToMxArray(&zRay,                      mxZ,    1);
                                    copyDoubleToMxArray(&tauRay,                    mxTau,  1);
                                    copyComplexToMxArray(&ampRay,                   mxAmp,  1);

                                    //copy mxArrays to mxArrivalStruct
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct,    //pointer to the mxStruct
                                                        (MWINDEX)arrivals[j][jj].nArrivals, //index of the element
                                                        0,                                  //position of the field (in this case, field 0 is "theta"
                                                        mxTheta);                           //the mxArray we want to copy into the mxStruct
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 1, mxR);
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 2, mxZ);
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 3, mxTau);
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 4, mxAmp);
                                    ///Arrival has been saved to mxAadStruct
                                    
                                     ///now lets save some aditional ray information:
                                    iReturns    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nSurRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nBotRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);
                                    nObjRefl    = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1, mxREAL);

                                    copyBoolToMxArray(      &ray[i].iReturn,    iReturns,   1);
                                    copyUInt32ToMxArray(    &ray[i].sRefl,  nSurRefl,   1);
                                    copyUInt32ToMxArray(    &ray[i].bRefl,  nBotRefl,   1);
                                    copyUInt32ToMxArray(    &ray[i].oRefl,  nObjRefl,   1);

                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 5, iReturns);
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 6, nSurRefl);
                                    mxSetFieldByNumber( arrivals[j][jj].mxArrivalStruct, (MWINDEX)arrivals[j][jj].nArrivals, 7, nBotRefl);
                                    ///aditional information has been saved

                                    arrivals[j][jj].nArrivals += 1;
                                    maxNumArrivals = max(arrivals[j][jj].nArrivals, maxNumArrivals);
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


    //write "maximum number of arrivals at any single hydrophone" to matfile:
    mxNumArrivals = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
    copyDoubleToMxArray(&maxNumArrivals, mxNumArrivals, 1);
    matPutVariable(settings->options.matfile, "maxNumArrivals", mxNumArrivals);


    //copy arrival data to mxAadStruct:
    mxAadStruct = mxCreateStructMatrix( (MWSIZE)settings->output.nArrayZ,   //number of rows
                                        (MWSIZE)settings->output.nArrayR,   //number of columns
                                        4,              //number of fields in each element
                                        aadFieldNames); //list of field names
    if( mxAadStruct == NULL ) {
        fatal("Memory Alocation error.");
    }
    for (j=0; j<settings->output.nArrayR; j++){
        for (jj=0; jj<settings->output.nArrayZ; jj++){
            mxNumArrivals   = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxRHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);
            mxZHyd          = mxCreateDoubleMatrix((MWSIZE)1,   (MWSIZE)1,  mxREAL);

            copyDoubleToMxArray(&arrivals[j][jj].nArrivals,     mxNumArrivals,1);
            copyDoubleToMxArray(&settings->output.arrayR[j],    mxRHyd,1);
            copyDoubleToMxArray(&settings->output.arrayZ[jj],   mxZHyd,1);

            idx[0] = (MWINDEX)jj;
            idx[1] = (MWINDEX)j;
            mxSetFieldByNumber( mxAadStruct,                                    //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                0,                                              //position of the field (in this case, field 0 is "nArrivals"
                                mxNumArrivals);                                 //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAadStruct,                        //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                1,                                          //position of the field (in this case, field 1 is "rHyd"
                                mxRHyd);                                    //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAadStruct,                        //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                2,                                          //position of the field (in this case, field 2 is "zHyd"
                                mxZHyd);                                    //the mxArray we want to copy into the mxStruct

            mxSetFieldByNumber( mxAadStruct,                                    //pointer to the mxStruct
                                mxCalcSingleSubscript(mxAadStruct,  2, idx),    //index of the element
                                3,                                              //position of the field (in this case, field 3 is "arrival"
                                arrivals[j][jj].mxArrivalStruct);               //the mxArray we want to copy into the mxStruct
        }
    }


    ///Write Eigenrays to matfile:
    matPutVariable(settings->options.matfile, "arrivals", mxAadStruct);
    
    //free memory
    mxDestroyArray(mxAadStruct);
    reallocRayMembers(ray, 0);
    free(ray);
    DEBUG(1,"out\n");
}
