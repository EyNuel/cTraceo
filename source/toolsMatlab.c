/****************************************************************************************
 * toolsMatlab.c                                                                        *
 * Collection of functions to write c arrays (pointers) to mxArrays.                    *
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
 ****************************************************************************************/

#pragma once
#if USE_MATLAB == 1
    #include    <matrix.h>          //for matlab functions (used in copyComplexToPtr and copyComplexToPtr2D)
#else
    #include    "matOut/matOut.h"
#endif

#include    <stdint.h>
#include    <stdbool.h>


///Prototypes:
 
//void    copyDoubleToPtr(double*, double*, uintptr_t);
void    copyFloatToMxArray(float*, mxArray*, uintptr_t);
void    copyDoubleToMxArray(double*, mxArray*, uintptr_t);
void    copyUInt32ToMxArray(uint32_t*, mxArray*, uintptr_t);
void    copyBoolToMxArray(bool*, mxArray*, uintptr_t);
        
//void    copyfloatToPtr2D(float**, float*, uintptr_t, uintptr_t);
void    copyFloatToMxArray2D(float**, mxArray*, uintptr_t, uintptr_t);
void    copyFloatToMxArray2D_transposed(float**, mxArray*, uintptr_t, uintptr_t);
void    copyDoubleToMxArray2D(double**, mxArray*, uintptr_t, uintptr_t);
void    copyDoubleToMxArray2D_transposed(double**, mxArray*, uintptr_t, uintptr_t);
        
void    copyComplexFloatToMxArray(complex float*, mxArray*, uintptr_t);
void    copyComplexDoubleToMxArray(complex double*, mxArray*, uintptr_t);
void    copyComplexFloatToMxArray2D(complex float**, mxArray*, uintptr_t, uintptr_t);
void    copyComplexDoubleToMxArray2D(complex double**, mxArray*, uintptr_t, uintptr_t);
void    copyComplexFloatToMxArray2D_transposed(complex float**, mxArray*, uintptr_t, uintptr_t);
void    copyComplexDoubleToMxArray2D_transposed(complex double**, mxArray*, uintptr_t, uintptr_t);



///Functions:

/*
 * TODO: safe to remove?
void    copyDoubleToPtr(double* origin, double* dest, uintptr_t nItems){
    //TODO replace all uses of this function with copyToMxArray()
    uintptr_t   i;
    
    for( i=0; i<nItems; i++ ){
        dest[i] = origin[i];
    }
}
*/

void    copyFloatToMxArray(float* origin, mxArray* dest, uintptr_t nItems){
    uintptr_t   i;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);
    
    for( i=0; i<nItems; i++ ){
        destReal[i] = (double)origin[i];
    }
}

void    copyDoubleToMxArray(double* origin, mxArray* dest, uintptr_t nItems){
    uintptr_t   i;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);
    
    for( i=0; i<nItems; i++ ){
        destReal[i] = origin[i];
    }
}

void    copyUInt32ToMxArray(uint32_t* origin, mxArray* dest, uintptr_t nItems){
    uintptr_t   i;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);
    
    for( i=0; i<nItems; i++ ){
        destReal[i] = (float)origin[i];
    }
}

void    copyBoolToMxArray(bool* origin, mxArray* dest, uintptr_t nItems){
    uintptr_t   i;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);
    
    for( i=0; i<nItems; i++ ){
        destReal[i] = (float)origin[i];
    }
}

/*
 * TODO: safe to remove
void    copyDoubleToPtr2D(double** origin, double* dest, uintptr_t rowSize, uintptr_t colSize){
    //TODO replace all uses of this function with copyToMxArray2D()
    uintptr_t   i,j;

    for( j=0; j<colSize; j++ ){
        for(i=0; i<rowSize; i++){
            dest[i*colSize +j] = origin[j][i];
        }
    }
}
*/

void    copyFloatToMxArray2D(float** origin, mxArray* dest, uintptr_t rowSize, uintptr_t colSize){
    uintptr_t   i,j;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);

    for( j=0; j<colSize; j++ ){
        for(i=0; i<rowSize; i++){
            destReal[i*colSize +j] = (double)origin[j][i];
        }
    }
}

void    copyDoubleToMxArray2D(double** origin, mxArray* dest, uintptr_t rowSize, uintptr_t colSize){
    uintptr_t   i,j;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);

    for( j=0; j<colSize; j++ ){
        for(i=0; i<rowSize; i++){
            destReal[i*colSize +j] = origin[j][i];
        }
    }
}


void    copyFloatToMxArray2D_transposed(float** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
    uintptr_t   i,j;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);
    
    for( j=0; j<dimR; j++ ){
        for(i=0; i<dimZ; i++){
            destReal[j*dimZ + i] = (double)origin[j][i];
        }
    }
}

void    copyDoubleToMxArray2D_transposed(double** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
    uintptr_t   i,j;
    double* destReal = NULL;
    
    destReal = mxGetData(dest);
    
    for( j=0; j<dimR; j++ ){
        for(i=0; i<dimZ; i++){
            destReal[j*dimZ + i] = origin[j][i];
        }
    }
}


void    copyComplexFloatToMxArray(complex float* origin, mxArray* dest, uintptr_t nItems){
    uintptr_t   i;
    double* destImag = NULL;
    double* destReal = NULL;
    
    //get a pointer to the real and imaginary parts of the destination:
    destReal = mxGetData(dest);
    destImag = mxGetImagData(dest);
    
    for( i=0; i<nItems; i++ ){
        destReal[i] = (double)creal(origin[i]);
        destImag[i] = (double)cimag(origin[i]);
    }
}

void    copyComplexDoubleToMxArray(complex double* origin, mxArray* dest, uintptr_t nItems){
    uintptr_t   i;
    double* destImag = NULL;
    double* destReal = NULL;
    
    //get a pointer to the real and imaginary parts of the destination:
    destReal = mxGetData(dest);
    destImag = mxGetImagData(dest);
    
    for( i=0; i<nItems; i++ ){
        destReal[i] = creal(origin[i]);
        destImag[i] = cimag(origin[i]);
    }
}

void    copyComplexFloatToMxArray2D(complex float** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
    uintptr_t   i,j;
    double* destImag = NULL;
    double* destReal = NULL;
    
    //get a pointer to the real and imaginary parts of the destination:
    destReal = mxGetData(dest);
    destImag = mxGetImagData(dest);
    
    for( j=0; j<dimR; j++ ){
        for(i=0; i<dimZ; i++){
            destReal[j + i*dimR] = (double)creal(origin[j][i]);
            destImag[j + i*dimR] = (double)cimag(origin[j][i]);
        }
    }
}

void    copyComplexDoubleToMxArray2D(complex double** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
    uintptr_t   i,j;
    double* destImag = NULL;
    double* destReal = NULL;
    
    //get a pointer to the real and imaginary parts of the destination:
    destReal = mxGetData(dest);
    destImag = mxGetImagData(dest);
    
    for( j=0; j<dimR; j++ ){
        for(i=0; i<dimZ; i++){
            destReal[j + i*dimR] = creal(origin[j][i]);
            destImag[j + i*dimR] = cimag(origin[j][i]);
        }
    }
}

void    copyComplexFloatToMxArray2D_transposed(complex float** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
    uintptr_t   i,j;
    double*     destImag = NULL;
    double*     destReal = NULL;
    
    //get a pointer to the real and imaginary parts of the destination:
    destReal = mxGetData(dest);
    destImag = mxGetImagData(dest);
    
    for( j=0; j<dimR; j++ ){
        for(i=0; i<dimZ; i++){
            destReal[j*dimZ + i] = (double)creal(origin[j][i]);
            destImag[j*dimZ + i] = (double)cimag(origin[j][i]);
        }
    }
}

void    copyComplexDoubleToMxArray2D_transposed(complex double** origin, mxArray* dest, uintptr_t dimZ, uintptr_t dimR){
    uintptr_t   i,j;
    double*     destImag = NULL;
    double*     destReal = NULL;
    
    //get a pointer to the real and imaginary parts of the destination:
    destReal = mxGetData(dest);
    destImag = mxGetImagData(dest);
    
    for( j=0; j<dimR; j++ ){
        for(i=0; i<dimZ; i++){
            destReal[j*dimZ + i] = creal(origin[j][i]);
            destImag[j*dimZ + i] = cimag(origin[j][i]);
        }
    }
}
