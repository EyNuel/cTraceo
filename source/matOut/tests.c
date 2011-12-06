/*
 *  Copyright 2011 Emanuel Ey <emanuel.ey@gmail.com>
 * 
 *  This file is part of matOut.
 *
 *  MatOut is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MatOut is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with matOut.  If not, see <http://www.gnu.org/licenses/>.
 */
 
/*
 * This file contains testing/example code for for matOut.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "matOut.h"
#include "toolsMatlab.c"
#include <complex.h>
#include "toolsMisc.c"


/*
 * Choose which test to run:
 *  testNumber   Description
 *       1       Create several different types os simple arrays and write them to "test1.mat"
 *       2       Create a struct containing 4 simple arrays and write it to "test2.mat"
 *       3       Create a struct containing 3 simple arrays and a child structure and write to "test3.mat"
 *       4       Like test 3, but structure members are 2D arrays instead of scalars. written to "test4.mat"
 *       5       an array of structures, with each one containing data similar to test4. written to "test5.mat"
 *       6       Testing a single string
 */

#define ALL 0
//#define runTest 1
#define runTest ALL



int main(void){
    MATFile*            matfile = NULL; 
    mxArray*            mxA     = NULL;
    mxArray*            mxB     = NULL;
    mxArray*            mxC     = NULL;
    mxArray*            mxD     = NULL;
    mxArray*            mxString= NULL;
    double              a[5]    = {1.1, 2.2, 3.3, 4.4, 5.5};
    double**            b       = NULL;
    double**            c       = NULL;
    complex double**    d       = NULL;
    
    mxArray*            mxStruct= NULL;
    mxArray*            mxSubStruct= NULL;
    const char*         fieldNames0[]= {"w",
                                        "y",
                                        "z",
                                        "zz"};
    const char*         fieldNames1[]= {"w",
                                        "y",
                                        "z",
                                        "subStruct"};
    const char*         fieldNames2[]= {"w",
                                        "y"};
    mxArray*            mxW = NULL;
    mxArray*            mxY = NULL;
    mxArray*            mxZ = NULL;
    mxArray*            mxZZ= NULL;
    double              temp = 0.6;
    
#if runTest == 1 || runTest == ALL
    /* *********************************************************
     * Create several different types os simple arrays and write
     * them to a matfile.
     */
    /*
    Should generate the same matfile as the following matlab code:
    a = [1.1,2.2,3.3,4.4,5.5];
    b = [1.1,2.2,3.3,4.4,5.5;-1.1,-2.2,-3.3,-4.4,-5.5];
    c = [1.1,-1.1; 2.2,-2.2; 3.3,-3.3; 4.4,-4.4; 5.5,-5.5];
    d = [ 1.1+1.1i, -1.1-1.1i; 2.2+2.2i, -2.2-2.2i; 3.3+3.3i, -3.3-3.3i; 4.4+4.4i, -4.4-4.4i; 5.5+5.5i, -5.5-5.5i];
    aString = 'Some Long fancy, textually relevant sequence of ASCII, commonly known as string.';
    save('test1_.mat', 'aString', 'a', 'b', 'c', 'd', '-v6');
    */
    b = mallocDouble2D(2,5);
    for (int i=0; i<5; i++){
        b[0][i] = 1.1*(i+1);
        b[1][i] = 1.1*(-i-1);
    }
    
    c = mallocDouble2D(5,2);
    for (int i=0; i<5; i++){
        c[i][0] = 1.1*(i+1);
        c[i][1] = 1.1*(-i-1);
    }
    
    d = mallocComplex2D(5,2);
    for (int i=0; i<5; i++){
        d[i][0] = 1.1*(i+1) +1.1*((i+1)*I);
        d[i][1] = 1.1*(-i-1) -1.1*((i+1)*I);
    }
    
    
    matfile = matOpen("test1.mat", "w");
    
    mxString= mxCreateString("Some Long fancy, textually relevant sequence of ASCII, commonly known as string.");
    matPutVariable(matfile, "aString", mxString);
    mxDestroyArray(mxString);
    
    mxA     = mxCreateDoubleMatrix(1, 5, mxREAL);
    mxB     = mxCreateDoubleMatrix(2, 5, mxREAL);
    mxC     = mxCreateDoubleMatrix(5, 2, mxREAL);
    mxD     = mxCreateDoubleMatrix(5, 2, mxCOMPLEX);
    
    copyDoubleToMxArray(a, mxA, 5);
    matPutVariable(     matfile, "a", mxA);
    
    copyDoubleToMxArray2D(b, mxB, 5, 2);
    matPutVariable(     matfile, "b", mxB);
    
    copyDoubleToMxArray2D(c, mxC, 2, 5);
    matPutVariable(     matfile, "c", mxC);
    
    copyComplexToMxArray2D(d, mxD, 2, 5);
    matPutVariable(     matfile, "d", mxD);
    
    mxDestroyArray(     mxA);
    mxDestroyArray(     mxB);
    mxDestroyArray(     mxC);
    mxDestroyArray(     mxD);
    matClose(matfile);
    freeDouble2D(b, 2);
    freeDouble2D(c, 5);
    freeComplex2D(d, 5);
#endif



#if runTest == 2 || runTest == ALL
    /* *********************************************************
     * Create a struct containing 4 simple arrays and write it
     * to a matfile.
     */
    matfile = matOpen("test2.mat", "w");
    mxStruct = mxCreateStructMatrix(    1,  //number of rows
                                        1,  //number of columns
                                        4,  //number of fields in each element
                                        fieldNames0);   //list of field names
    
    mxW = mxCreateDoubleMatrix(1 ,1, mxREAL);
    mxY = mxCreateDoubleMatrix(1 ,1, mxREAL);
    mxZ = mxCreateDoubleMatrix(1 ,1, mxREAL);
    mxZZ= mxCreateDoubleMatrix(1 ,1, mxREAL);
    
    temp = 1.1;
    copyDoubleToMxArray(&temp, mxW, 1);
    //temp = (double)(*(mxGetPr(mxW)));
    //printf("w: %lf\n", temp);
    
    temp = 2.2;
    copyDoubleToMxArray(&temp, mxY, 1);
    
    temp = 3.3;
    copyDoubleToMxArray(&temp, mxZ, 1);
    temp = 4.4;
    copyDoubleToMxArray(&temp, mxZZ, 1);
    
    mxSetFieldByNumber( mxStruct,   //pointer to the mxStruct
                        0,          //index of the element (linear)
                        0,          //position of the field (in this case, field 0 is "w"
                        mxW);       //the mxArray we want to add to the mxStruct
    
    //printf("w: %lf\n", (*(double*)mxStruct[0].field[0].pr_double));
    mxSetFieldByNumber( mxStruct,   0,  1,  mxY);
    mxSetFieldByNumber( mxStruct,   0,  2,  mxZ);
    mxSetFieldByNumber( mxStruct,   0,  3,  mxZZ);
    
    matPutVariable(     matfile, "aVariableName", mxStruct);
    
    mxDestroyArray(mxStruct);
#endif



#if runTest == 3 || runTest == ALL
    /* *********************************************************
     * Create a struct containing 3 simple arrays and a child
     * structure and write to a matfile.
     */
    matfile = matOpen("test3.mat", "w");
    mxStruct = mxCreateStructMatrix(    1,  //number of rows
                                        1,  //number of columns
                                        4,  //number of fields in each element
                                        fieldNames1);   //list of field names
    
    mxW = mxCreateDoubleMatrix(1 ,1, mxREAL);
    mxY = mxCreateDoubleMatrix(1 ,1, mxREAL);
    mxZ = mxCreateDoubleMatrix(1 ,1, mxREAL);
    mxSubStruct = mxCreateStructMatrix( 1,  //number of rows
                                        1,  //number of columns
                                        2,  //number of fields in each element
                                        fieldNames2);   //list of field names
    
    
    temp = 1.1;
    copyDoubleToMxArray(&temp, mxW, 1);
    //temp = (double)(*(mxGetPr(mxW)));
    //printf("w: %lf\n", temp);
    
    temp = 2.2;
    copyDoubleToMxArray(&temp, mxY, 1);
    
    temp = 3.3;
    copyDoubleToMxArray(&temp, mxZ, 1);
    
    mxSetFieldByNumber( mxStruct,   //pointer to the mxStruct
                        0,          //index of the element (linear)
                        0,          //position of the field (in this case, field 0 is "w"
                        mxW);       //the mxArray we want to add to the mxStruct
    
    //printf("w: %lf\n", (*(double*)mxStruct[0].field[0].pr_double));
    mxSetFieldByNumber( mxStruct,   0,  1,  mxY);
    mxSetFieldByNumber( mxStruct,   0,  2,  mxZ);
    
    mxSetFieldByNumber( mxSubStruct,    0,  0,  mxY);
    mxSetFieldByNumber( mxSubStruct,    0,  1,  mxZ);
    
    mxSetFieldByNumber( mxStruct,   0,  3,  mxSubStruct);
    
    matPutVariable(     matfile, "X", mxStruct);
    
    mxDestroyArray(mxStruct);
    mxDestroyArray(mxSubStruct);
#endif



#if runTest == 4 || runTest == ALL
    /* *********************************************************
     * Create a struct containing 3 2D arrays and a child
     * structure also containing 2D arrays and write to a matfile.
     */
    b = mallocDouble2D(2,5);
    for (int i=0; i<5; i++){
        b[0][i] = i+1;
        b[1][i] = -i-1;
    }
    
    c = mallocDouble2D(5,2);
    for (int i=0; i<5; i++){
        c[i][0] = i+1;
        c[i][1] = -i-1;
    }
    
    d = mallocComplex2D(5,2);
    for (int i=0; i<5; i++){
        d[i][0] = i+1 +(i+1)*I;
        d[i][1] = -i-1 -(i+1)*I;
    }
    matfile = matOpen("test4.mat", "w");
    mxStruct = mxCreateStructMatrix(    1,  //number of rows
                                        1,  //number of columns
                                        4,  //number of fields in each element
                                        fieldNames1);   //list of field names
    
    mxB     = mxCreateDoubleMatrix(2, 5, mxREAL);
    mxC     = mxCreateDoubleMatrix(5, 2, mxREAL);
    mxD     = mxCreateDoubleMatrix(5, 2, mxCOMPLEX);
    
    mxSubStruct = mxCreateStructMatrix( 1,  //number of rows
                                        1,  //number of columns
                                        2,  //number of fields in each element
                                        fieldNames2);   //list of field names
    
    copyDoubleToMxArray2D(b, mxB, 5, 2);
    copyDoubleToMxArray2D(c, mxC, 2, 5);
    copyComplexToMxArray2D(d, mxD, 2, 5);
    
    mxSetFieldByNumber( mxStruct,   //pointer to the mxStruct
                        0,          //index of the element (linear)
                        0,          //position of the field (in this case, field 0 is "b"
                        mxB);       //the mxArray we want to add to the mxStruct
    
    //printf("w: %lf\n", (*(double*)mxStruct[0].field[0].pr_double));
    mxSetFieldByNumber( mxStruct,   0,  1,  mxC);
    mxSetFieldByNumber( mxStruct,   0,  2,  mxD);
    
    mxSetFieldByNumber( mxSubStruct,    0,  0,  mxC);
    mxSetFieldByNumber( mxSubStruct,    0,  1,  mxD);
    
    mxSetFieldByNumber( mxStruct,   0,  3,  mxSubStruct);
    
    matPutVariable(     matfile, "X", mxStruct);
    
    mxDestroyArray(mxStruct);
    mxDestroyArray(mxSubStruct);
#endif



#if runTest == 5 || runTest == ALL
    /* *********************************************************
     * Create a struct containing 3 2D arrays and a child
     * structure also containing 2D arrays and write to a matfile.
     */
     /*
    Should generate the same matfile as the following matlab code:
    b = [1.1,2.2,3.3,4.4,5.5;-1.1,-2.2,-3.3,-4.4,-5.5];
    c = [1.1,-1.1; 2.2,-2.2; 3.3,-3.3; 4.4,-4.4; 5.5,-5.5];
    d = [ 1.1+1.1i, -1.1-1.1i; 2.2+2.2i, -2.2-2.2i; 3.3+3.3i, -3.3-3.3i; 4.4+4.4i, -4.4-4.4i; 5.5+5.5i, -5.5-5.5i];
    aString = 'Some Long fancy, textually relevant sequence of ASCII, commonly known as string.';
    X = struct;
    X(1).w = b;
    X(1).y = aString;
    X(1).z = d;
    X(1).subStruct = struct;
    X(1).subStruct(1).w = c;
    X(1).subStruct(1).y = d;
     
    X(2).w = aString;
    X(2).y = c;
    X(2).z = d;
    X(2).subStruct = struct;
    X(2).subStruct(1).w = c;
    X(2).subStruct(1).y = d;
    
    X(3).w = b;
    X(3).y = c;
    X(3).z = aString;
    X(3).subStruct = struct;
    X(3).subStruct(1).w = c;
    X(3).subStruct(1).y = d;
     
    save('test5_.mat', 'X', '-v6');
    */
    b = mallocDouble2D(2,5);
    for (int i=0; i<5; i++){
        b[0][i] = 1.1*(i+1);
        b[1][i] = 1.1*(-i-1);
    }
    
    c = mallocDouble2D(5,2);
    for (int i=0; i<5; i++){
        c[i][0] = 1.1*(i+1);
        c[i][1] = 1.1*(-i-1);
    }
    
    d = mallocComplex2D(5,2);
    for (int i=0; i<5; i++){
        d[i][0] = 1.1*(i+1) +1.1*((i+1)*I);
        d[i][1] = 1.1*(-i-1) -1.1*((i+1)*I);
    }
    matfile = matOpen("test5.mat", "w");
    
    mxB     = mxCreateDoubleMatrix(2, 5, mxREAL);
    mxC     = mxCreateDoubleMatrix(5, 2, mxREAL);
    mxD     = mxCreateDoubleMatrix(5, 2, mxCOMPLEX);
    
    copyDoubleToMxArray2D(b, mxB, 5, 2);
    copyDoubleToMxArray2D(c, mxC, 2, 5);
    copyComplexToMxArray2D(d, mxD, 2, 5);
    
    mxString= mxCreateString("Some Long fancy, textually relevant sequence of ASCII, commonly known as string.");
    
    mxStruct = mxCreateStructMatrix(    3,  //number of rows
                                        1,  //number of columns
                                        4,  //number of fields in each element
                                        fieldNames1);   //list of field names
    
    
    mxSubStruct = mxCreateStructMatrix( 1,  //number of rows
                                        1,  //number of columns
                                        2,  //number of fields in each element
                                        fieldNames2);   //list of field names
    
    //row 0
    mxSetFieldByNumber( mxStruct,   //pointer to the mxStruct
                        0,          //index of the element (linear)
                        0,          //position of the field (in this case, field 0 is "b"
                        mxB);       //the mxArray we want to add to the mxStruct
    mxSetFieldByNumber( mxStruct,   0,  1,  mxC);
    mxSetFieldByNumber( mxStruct,   0,  2,  mxString);
    
    mxSetFieldByNumber( mxSubStruct,    0,  0,  mxC);
    mxSetFieldByNumber( mxSubStruct,    0,  1,  mxD);
    
    mxSetFieldByNumber( mxStruct,   0,  3,  mxSubStruct);
    
    //row 1
    mxSetFieldByNumber( mxStruct,   //pointer to the mxStruct
                        1,          //index of the element (linear)
                        0,          //position of the field (in this case, field 0 is "b"
                        mxB);       //the mxArray we want to add to the mxStruct
    mxSetFieldByNumber( mxStruct,   1,  1,  mxString);
    mxSetFieldByNumber( mxStruct,   1,  2,  mxD);
    
    mxSetFieldByNumber( mxStruct,   1,  3,  mxSubStruct);
    
    //row 2
    mxSetFieldByNumber( mxStruct,   //pointer to the mxStruct
                        2,          //index of the element (linear)
                        0,          //position of the field (in this case, field 0 is "b"
                        mxString);      //the mxArray we want to add to the mxStruct
    mxSetFieldByNumber( mxStruct,   2,  1,  mxC);
    mxSetFieldByNumber( mxStruct,   2,  2,  mxD);
    
    mxSetFieldByNumber( mxStruct,   2,  3,  mxSubStruct);
    
    matPutVariable(     matfile, "X", mxStruct);
    
    //print the addresses of the mxStruct members:
    /*
    for (uintptr_t iStruct=0; iStruct<mxStruct->dims[0]*mxStruct->dims[1]; iStruct++){
        printf("&mxStruct[0]: %lu\n", &mxStruct[iStruct]);
    }
    */
    
    mxDestroyArray(mxStruct);
    mxDestroyArray(mxSubStruct);
#endif



#if runTest == 6 || runTest == ALL
    /*
     * write a string to a matfile.
     */
    /*
    Should generate the same matfile as the following matlab code:
    caseTitle = 'TRACEO: All Ray Information';
    save('test6_.mat', 'caseTitle', '-v6');
    */
    matfile = matOpen("test6.mat", "w");
    //printf("strlen(): %d\n", strlen("TRACEO: All Ray Information"));
    mxString = mxCreateString("MatOut: a single string written to a matfile.");
    if(mxString == NULL){
        fatal("Memory alocation error.");
    }
    matPutVariable(matfile, "caseTitle", mxString);
    mxDestroyArray(mxString);
    matClose(matfile);
#endif


    exit(EXIT_SUCCESS);
}
