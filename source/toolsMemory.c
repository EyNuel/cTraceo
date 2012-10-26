/****************************************************************************************
 * toolsMemory.c                                                                        *
 * Collection of utility functions for  handling memory.                                *
 * NOTE: All memory allocation functions in this file check for allocation errors.      *
 * NOTE: TODO the memory reallocation functions contained in this file are mostly not   *
 *       in use due to random occurences of "bus error".                                *
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
 ****************************************************************************************/

#pragma     once
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdint.h>
#include    <stdbool.h>
#include    "globals.h"
#include    "toolsMisc.c"

/** 
 */


///Function Prototypes:
char*           mallocChar(uintptr_t);
bool*           mallocBool(uintptr_t numBools);
bool*           reallocBool(bool* old, uintptr_t numBools);
uint32_t*       mallocUint(uintptr_t);
uint32_t*       reallocUint(uint32_t*, uintptr_t);
int32_t*        mallocInt(uintptr_t);
int32_t*        reallocInt(int32_t*, uintptr_t);

float*          mallocFloat(uintptr_t);
float*          reallocFloat(float*, uintptr_t);
void            freeFloat(float*);
float**         mallocFloat2D(uintptr_t, uintptr_t);
void            freeFloat2D(float**, uintptr_t);

double*         mallocDouble(uintptr_t);
double*         reallocDouble(double*, uintptr_t);
void            freeDouble(double*);
double**        mallocDouble2D(uintptr_t, uintptr_t);
void            freeDouble2D(double**, uintptr_t);

complex float*  mallocComplex(uintptr_t);
complex float*  reallocComplex(complex float*, uintptr_t);
void            freeComplex(complex float*);
complex float** mallocComplex2D(uintptr_t, uintptr_t);
void            freeComplex2D(complex float**, uintptr_t);

settings_t*     mallocSettings(void);
void            freeInterface(interface_t*);
void            freeObject(object_t*);
void            freeSettings(settings_t*);
vector_t*       mallocVector(uintptr_t);
vector_t*       reallocVector(vector_t*, uintptr_t);
point_t*        mallocPoint(uintptr_t);
point_t*        reallocPoint(point_t*, uintptr_t);
void            printSettings(settings_t*);
ray_t*          makeRay(uintptr_t);
void            reallocRayMembers(ray_t*, uintptr_t);



///Functions:


char*               mallocChar(uintptr_t numChars){
    /*
        Allocates a char string and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    
    char*   temp = NULL;    //temporary pointer
    temp = malloc((size_t)numChars*sizeof(char));
    if (temp == NULL){
        fatal("Memory allocation error.\n");
    }
    return temp;
}


bool*           mallocBool(uintptr_t numBools){
    /*
        Allocates a char string and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    bool*   temp = NULL;    //temporary pointer

    temp = malloc(numBools*sizeof(bool));
    return temp;
}

bool*           reallocBool(bool* old, uintptr_t numBools){
    /*
        Allocates an uint array and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    DEBUG(10,"reallocBool(),\t in\n");
    bool*   new = NULL;

    if(numBools == 0){
        free(old);
    }else{
        new = realloc(old, numBools*sizeof(bool));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    DEBUG(10,"reallocBool(),\t out\n");
    return new;
}

uint32_t*           mallocUint(uintptr_t numUints){
    /*
        Allocates a char string and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    uint32_t*   temp = NULL;    //temporary pointer

    temp = malloc(numUints*sizeof(uint32_t));
    return temp;
}

uint32_t*           reallocUint(uint32_t* old, uintptr_t numUints){
    /*
        Allocates an uint array and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    DEBUG(10,"reallocUint(),\t in\n");
    uint32_t*   new = NULL;

    if(numUints == 0){
        free(old);
    }else{
        new = realloc(old, numUints*sizeof(uint32_t));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    DEBUG(10,"reallocUint(),\t out\n");
    return new;
}

int32_t*            mallocInt(uintptr_t numInts){
    /*
        Allocates a char string and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    int32_t*    temp = NULL;    //temporary pointer

    temp = malloc(numInts*sizeof(int32_t));
    return temp;
}

int32_t*            reallocInt(int32_t* old, uintptr_t numInts){
    /*
        Allocates a char string and returns a pointer to it in case of success,
        exits with error code otherwise.
    */
    int32_t*    new = NULL;
    
    if(numInts == 0){
        free(old);
    }else{
        new = realloc(old, numInts*sizeof(int32_t));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    return new;
}

double*             mallocDouble(uintptr_t numDoubles){
    /*
        Allocates an array of doubles and returns a pointer to it in case of success,
        Exits with error code otherwise.
    */
    DEBUG(9,"mallocDouble(),\tin\n");
    double* temp = NULL;    //temporary pointer
    temp = malloc(numDoubles * sizeof(double));
    if(temp == NULL){
        fatal("Memory alocation error.\n");
    }
    DEBUG(9,"mallocDouble(),\tout\n");
    return temp;
}
    
double*             reallocDouble(double* old, uintptr_t numDoubles){
    DEBUG(10,"reallocDouble(),\tin\n");
    double*     new = NULL;
    
    if(numDoubles == 0){
        free(old);
    }else{
        new = realloc(old, numDoubles*sizeof(double));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    DEBUG(10,"reallocDouble(),\tout\n");
    return new;
}
    
void                freeDouble(double* greenMile){
    if(greenMile != NULL){
        free(greenMile);
    }
}
    
double**            mallocDouble2D(uintptr_t numRows, uintptr_t numCols){
    /*
     * Returns a pointer to an array of pointer do doubles.
     * Or:
     * Return a 2D Array.
     */

    uint32_t    i;
    double**    array = NULL;
    array = malloc(numRows * sizeof(uintptr_t*));   //malloc an array of pointers
    
    if(array == NULL)
        fatal("Memory allocation error.\n");

    for(i = 0; i < numRows; i++){
        array[i] = mallocDouble(numCols);   //Nota that mallocDouble() already checks for allocation errors
    }

    return array;
}

void                freeDouble2D(double** greenMile, uintptr_t items){
    /*
     * frees the memory allocated to a double pointer of type double.
     */
     uintptr_t  i;
     
    for(i=0; i<items; i++){
        if(greenMile[i] != NULL){
            free(greenMile[i]);
        }
    }
    free(greenMile);
}

float*             mallocFloat(uintptr_t numfloats){
    /*
        Allocates an array of floats and returns a pointer to it in case of success,
        Exits with error code otherwise.
    */
    DEBUG(9,"mallocFloat(),\tin\n");
    float* temp = NULL;    //temporary pointer
    temp = malloc(numfloats * sizeof(float));
    if(temp == NULL){
        fatal("Memory alocation error.\n");
    }
    DEBUG(9,"mallocFloat(),\tout\n");
    return temp;
}
    
float*             reallocFloat(float* old, uintptr_t numfloats){
    DEBUG(10,"reallocFloat(),\tin\n");
    float*     new = NULL;
    
    if(numfloats == 0){
        free(old);
    }else{
        new = realloc(old, numfloats*sizeof(float));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    DEBUG(10,"reallocFloat(),\tout\n");
    return new;
}
    
void                freeFloat(float* greenMile){
    if(greenMile != NULL){
        free(greenMile);
    }
}
    
float**            mallocFloat2D(uintptr_t numRows, uintptr_t numCols){
    /*
     * Returns a pointer to an array of pointer do floats.
     * Or:
     * Return a 2D Array.
     */

    uint32_t    i;
    float**    array = NULL;
    array = malloc(numRows * sizeof(uintptr_t*));   //malloc an array of pointers
    
    if(array == NULL)
        fatal("Memory allocation error.\n");

    for(i = 0; i < numRows; i++){
        array[i] = mallocFloat(numCols);   //Nota that mallocFloat() already checks for allocation errors
    }

    return array;
}

void                freeFloat2D(float** greenMile, uintptr_t items){
    /*
     * frees the memory allocated to a float pointer of type float.
     */
     uintptr_t  i;
     
    for(i=0; i<items; i++){
        if(greenMile[i] != NULL){
            free(greenMile[i]);
        }
    }
    free(greenMile);
}

complex float*     mallocComplex(uintptr_t numComplex){
    complex float* temp = NULL;
    uintptr_t       i;
    
    temp = malloc(numComplex * sizeof(complex float));
    if(temp == NULL)
        fatal("Memory alocation error.");
    //Initialize to zero:
    for (i=0; i<numComplex; i++){
        temp[i] = 0 + 0*I;
    }
    
    return temp;
}

complex float*     reallocComplex(complex float* old, uintptr_t numComplex){
    complex float* new = NULL;

    if(numComplex == 0){
        free(old);
    }else{
        new = realloc(old, numComplex*sizeof(complex float));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    return new;
}

void                freeComplex(complex float* greenMile){
    if(greenMile != NULL){
        free(greenMile);
    }
}

complex float**    mallocComplex2D(uintptr_t numRows, uintptr_t numCols){
    /*
     * Returns a pointer to an array of pointer do floats.
     * Or:
     * Return a 2D Array.
     */

    uint32_t    i;
    complex float**    array = NULL;
    array = malloc(numRows * sizeof(uintptr_t*));   //malloc an array of pointers
    
    if(array == NULL)
        fatal("Memory allocation error.\n");

    for(i = 0; i < numRows; i++){
        array[i] = mallocComplex(numCols);  //Nota that mallocFloat() already checks for allocation errors
    }

    return array;
}

void                freeComplex2D(complex float** greenMile, uintptr_t items){
    /*
     * frees the memory allocated to a float pointer of type complex float.
     */
     uintptr_t  i;
     
    for(i=0; i<items; i++){
        if(greenMile[i] != NULL){
            free(greenMile[i]);
        }
    }
    free(greenMile);
}



settings_t*         mallocSettings(void){
    /*
        Allocate memory for a settings structure.
        Return pointer in case o success, exit with error code otherwise.
    */
    settings_t* settings = NULL;
    settings = malloc(sizeof(settings_t));
    if(settings == NULL){
        fatal("Memory allocation error.\n");
    }
    
    settings->cTitle = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
    settings->source.thetas = NULL; //memory will be properly allocated in "readin.c"
    
    settings->altimetry.r = NULL;
    settings->altimetry.z = NULL;
    //settings->altimetry.surfaceProperties = NULL;
    
    settings->batimetry.r = NULL;
    settings->batimetry.z = NULL;
    //settings->batimetry.surfaceProperties = NULL;
    
    settings->output.arrayR = NULL;
    settings->output.arrayZ = NULL;
    
    //default values for options:
    settings->options.inFileName            = mallocChar(256);
    settings->options.killBackscatteredRays = false;
    settings->options.writeLogFile          = true;
    settings->options.logFile               = NULL;
    settings->options.logFileName           = mallocChar(256);
    settings->options.saveSSP               = false;
    
    return(settings);
}

void                freeInterface(interface_t* interface){
    if(interface != NULL){
        //Note that reallcing to size 0, corresponds to deallocing the memory
        reallocFloat(interface->r, 0);
        reallocFloat(interface->z, 0);
        reallocFloat(interface->cp, 0);
        reallocFloat(interface->cs, 0);
        reallocFloat(interface->rho, 0);
        reallocFloat(interface->ap, 0);
        reallocFloat(interface->as, 0);
        //free(interface);
    }
}

void                freeSettings(settings_t* settings){
    /*
     * Go through all items in a settings struct and free the alocated memory.
     */

    uintptr_t       i;

    if(settings != NULL){
        
        //free title:
        if(settings->cTitle != NULL){
            free(settings->cTitle);
        }
        
        //free source:
        if(&settings->source != NULL){
            reallocFloat(settings->source.thetas, 0);
        }
        
        //free altimetry:
        freeInterface(&settings->altimetry);
        
        //free soundSpeed:
        if(&settings->soundSpeed != NULL){
            freeFloat(settings->soundSpeed.z);
            //note that the range coordinates of the soundspeed are only allocated for cDist = C_DIST__FIELD
            
            switch (settings->soundSpeed.cDist){
                case C_DIST__PROFILE:
                    freeFloat(settings->soundSpeed.c1D);
                    break;
                    
                case C_DIST__FIELD:
                    freeFloat(settings->soundSpeed.r);
                    freeFloat2D(settings->soundSpeed.c2D, settings->soundSpeed.nz);
                    break;
                    
                default:
                    //this should not be possible, as the values are verified when reading the input file
                    //if this occurs, some kind of memmory corruption must have taken place
                    fatal("freeSettings(): Unknown Sound distribution type (neither profile nor field)");
                    break;
            }
        }
        //free objects:
        if(&settings->objects != NULL){
            if(settings->objects.numObjects > 0){
                for (i=0; i<settings->objects.numObjects; i++){
                    if (settings->objects.object[i].nCoords > 0){
                        freeFloat(settings->objects.object[i].r);
                        freeFloat(settings->objects.object[i].zDown);
                        freeFloat(settings->objects.object[i].zUp);
                    }
                }
                free(settings->objects.object);
            }
        }
        
        //free batimetry:
        freeInterface(&settings->batimetry);
        
        //free output (array configuration and acoustic pressure -if calculated):
        if(&settings->output != NULL){
            if(settings->output.nArrayR > 0){
                freeFloat(settings->output.arrayR);
            }
            if (settings->output.nArrayZ > 0){
                freeFloat(settings->output.arrayZ);
            }

            //TODO this is no longer corrrect => adapt to new layout of pressure2D
            //Acoustic pressure is only calculated for some types of output
            if( settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS ||
                settings->output.calcType == CALC_TYPE__COH_TRANS_LOSS  ||
                settings->output.calcType == CALC_TYPE__PART_VEL        ||
                settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL){
                    
                if (settings->output.arrayType == ARRAY_TYPE__RECTANGULAR){
                    if(settings->output.pressure2D != NULL){
                        freeComplex2D(settings->output.pressure2D, settings->output.nArrayR);
                    }
                }else{
                    //freeComplex(settings->output.pressure1D);
                }
            }
        }

        //free the actual settings struct:
        free(settings);
    }
}

vector_t*           mallocVector(uintptr_t  numVectors){
    vector_t*   temp = NULL;

    temp = malloc(numVectors * sizeof(vector_t));
    if(temp == NULL)
        fatal("Memory alocation error.");
    return temp;
}

vector_t*           reallocVector(vector_t* old, uintptr_t  numVectors){
    vector_t*   new = NULL;

    if(numVectors == 0){
        free(old);
    }else{
        new = realloc(old, numVectors * sizeof(vector_t));
        if (new == NULL){
            fatal("Memory allocation error.\n");
        }
    }
    return new;
}

point_t*            mallocPoint(uintptr_t   numPoints){
    point_t*    temp = NULL;

    temp = malloc(numPoints * sizeof(point_t));
    if(temp == NULL)
        fatal("Memory alocation error.");
    return temp;
}

point_t*            reallocPoint(point_t* old, uintptr_t    numPoints){
    point_t*    new = NULL;

    if(numPoints == 0){
        free(old);
    }else{
        new = realloc(old, numPoints * sizeof(point_t));
        if (new == NULL){
            //NOTE when freeing memory (setting it to size 0) a null pointer is not an error.
            fatal("reallocPoint(): Memory allocation error.\n");
        }
    }
    return new;
}

void                printSettings(settings_t*   settings){
    /************************************************
     *  Outputs a settings structure to stdout.     *
     ***********************************************/
    DEBUG(1, "in\n");
    uint32_t    i;
    
    printf("cTitle: \t\t\t%s", settings->cTitle);   //assuming a \n at the end of cTitle
    printf("\nsource.ds: \t\t\t%12.5lf\t[m]\n",     settings->source.ds);
    printf("source.rx: \t\t\t%12.5lf\t[m]\n",       settings->source.rx);
    printf("source.zx: \t\t\t%12.5lf\t[m]\n",       settings->source.zx);
    printf("source.rbox1: \t\t\t%12.5lf\t[m]\n",    settings->source.rbox1);
    printf("source.rbox2: \t\t\t%12.5lf\t[m]\n",    settings->source.rbox2);
    printf("source.freqx: \t\t\t%12.5lf\t[m]\n",    settings->source.freqx);
    printf("source.nThetas: \t\t%6.0u\n", (uint32_t)settings->source.nThetas);

    /* uncoment the following block to output all launching angles */
    /*
    for(i=0; i<settings->source.nThetas; i++){
        printf("source.thetas[%ld\t]: \t%lf\n", i, settings->source.thetas[i]);
    }
    */
    printf("\naltimetry.surfaceType: \t\t");
    switch(settings->altimetry.surfaceType){
        case SURFACE_TYPE__ABSORVENT:
            printf("Absorvent\n");
            break;
        case SURFACE_TYPE__ELASTIC:
            printf("Elastic\n");
            break;
        case SURFACE_TYPE__RIGID:
            printf("Rigid\n");
            break;
        case SURFACE_TYPE__VACUUM:
            printf("Vacuum\n");
            break;
    }

    printf("altimetry.surfacePropertyType:\t");
    switch(settings->altimetry.surfacePropertyType){
        case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
            printf("Homogeneous\n");
            break;
        case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
            printf("Non-Homogeneous\n");
            break;
    }

    printf("altimetry.surfaceInterpolation: ");
    switch(settings->altimetry.surfaceInterpolation){
        case SURFACE_INTERPOLATION__FLAT:
            printf("Flat\n");
            break;
        case SURFACE_INTERPOLATION__SLOPED:
            printf("Sloped\n");
            break;
        case SURFACE_INTERPOLATION__2P:
            printf("2P -Piecewise Linear Interpolation\n");
            break;
        case SURFACE_INTERPOLATION__3P:
            printf("3P -Piecewise Parabolic Interpolation\n");
            break;
        case SURFACE_INTERPOLATION__4P:
            printf("4P -Piecewise Cubic Interpolation\n");
            break;
    }

    printf("altimetry.surfaceAttenUnits: \t");
    switch(settings->altimetry.surfaceAttenUnits){
        case SURFACE_ATTEN_UNITS__dBperkHz:
            printf("dB/kHz\n");
            break;
        case SURFACE_ATTEN_UNITS__dBperMeter:
            printf("dB/meter\n");
            break;
        case SURFACE_ATTEN_UNITS__dBperNeper:
            printf("dB/neper\n");
            break;
        case SURFACE_ATTEN_UNITS__qFactor:
            printf("Q factor\n");
            break;
        case SURFACE_ATTEN_UNITS__dBperLambda:
            printf("dB/<lambda>\n");
            break;
    }

    printf("altimetry.numSurfaceCoords:\t%lu\n", (long unsigned int)settings->altimetry.numSurfaceCoords);

    printf("altimetry.surfaceProperties:    ");
    switch(settings->altimetry.surfacePropertyType){
        case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
            printf("cp:%lf\n ", settings->altimetry.cp[0]);
            printf("\t\t\t\tcs:%lf\n ", settings->altimetry.cs[0]);
            printf("\t\t\t\trho:%lf\n ",    settings->altimetry.rho[0]);
            printf("\t\t\t\tap:%lf\n ", settings->altimetry.ap[0]);
            printf("\t\t\t\tas:%lf\n",  settings->altimetry.as[0]);
            break;
            
        case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
            for(i=0; i<settings->altimetry.numSurfaceCoords; i++){
                printf("cp:%lf; ",  settings->altimetry.cp[i]);
                printf("cs:%lf; ",  settings->altimetry.cs[i]);
                printf("rho:%lf; ", settings->altimetry.rho[i]);
                printf("ap:%lf; ",  settings->altimetry.ap[i]);
                printf("as:%lf;\n", settings->altimetry.as[i]);
            }
            break;
    }
    printf("altimetry.r[0]:\t\t\t%lf\n", settings->altimetry.r[0]);
    printf("altimetry.r[N]:\t\t\t%lf\n", settings->altimetry.r[settings->altimetry.numSurfaceCoords-1]);
    

    /* sound speed block */
    
    /*  object block    */
    printf("\nobjects.numObjects:\t\t%u\n",settings->objects.numObjects);
    if(settings->objects.numObjects > 0){
        printf("objects.surfaceInterpolation: ");
        switch(settings->objects.surfaceInterpolation){
            case SURFACE_INTERPOLATION__2P:
                printf("2P\n");
                break;
            case SURFACE_INTERPOLATION__3P:
                printf("3P\n");
                break;
            case SURFACE_INTERPOLATION__4P:
                printf("4P\n");
                break;
            default:
                fatal("Unknown object surface interpolation type.\nAborting");
                break;
        }
        for(i=0; i<settings->objects.numObjects; i++){
            printf("object[%u].surfaceType: \t", i);
            switch(settings->objects.object[i].surfaceType){
                case SURFACE_TYPE__ABSORVENT:
                    printf("Absorvent\n");
                    break;
                case SURFACE_TYPE__ELASTIC:
                    printf("Elastic\n");
                    break;
                case SURFACE_TYPE__RIGID:
                    printf("Rigid\n");
                    break;
                case SURFACE_TYPE__VACUUM:
                    printf("Vacuum\n");
                    break;
            }
            printf("object[%u].surfaceAttenUnits: \t", i);
            switch(settings->objects.object[i].surfaceAttenUnits){
                case SURFACE_ATTEN_UNITS__dBperkHz:
                    printf("dB/kHz\n");
                    break;
                case SURFACE_ATTEN_UNITS__dBperMeter:
                    printf("dB/meter\n");
                    break;
                case SURFACE_ATTEN_UNITS__dBperNeper:
                    printf("dB/neper\n");
                    break;
                case SURFACE_ATTEN_UNITS__qFactor:
                    printf("Q factor\n");
                    break;
                case SURFACE_ATTEN_UNITS__dBperLambda:
                    printf("dB/<lambda>\n");
                    break;
                default:
                    fatal("Unknown object surface attenuation units.\nAborting.");
                    break;
            }
        }
    }
    
    /* batimetry block  */
    printf("\nbatimetry.surfaceType: \t");
    switch(settings->batimetry.surfaceType){
        case SURFACE_TYPE__ABSORVENT:
            printf("Absorvent\n");
            break;
        case SURFACE_TYPE__ELASTIC:
            printf("Elastic\n");
            break;
        case SURFACE_TYPE__RIGID:
            printf("Rigid\n");
            break;
        case SURFACE_TYPE__VACUUM:
            printf("Vacuum\n");
            break;
    }

    printf("batimetry.surfacePropertyType:\t");
    switch(settings->batimetry.surfacePropertyType){
        case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
            printf("Homogeneous\n");
            break;
        case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
            printf("Non-Homogeneous\n");
            break;
    }

    printf("batimetry.surfaceInterpolation:");
    switch(settings->batimetry.surfaceInterpolation){
        case SURFACE_INTERPOLATION__FLAT:
            printf("Flat\n");
            break;
        case SURFACE_INTERPOLATION__SLOPED:
            printf("Sloped\n");
            break;
        case SURFACE_INTERPOLATION__2P:
            printf("2P -Piecewise Linear Interpolation\n");
            break;
        case SURFACE_INTERPOLATION__3P:
            printf("3P -Piecewise Parabolic Interpolation\n");
            break;
        case SURFACE_INTERPOLATION__4P:
            printf("4P -Piecewise Cubic Interpolation\n");
            break;
    }

    printf("batimetry.surfaceAttenUnits: \t");
    switch(settings->batimetry.surfaceAttenUnits){
        case SURFACE_ATTEN_UNITS__dBperkHz:
            printf("dB/kHz\n");
            break;
        case SURFACE_ATTEN_UNITS__dBperMeter:
            printf("dB/meter\n");
            break;
        case SURFACE_ATTEN_UNITS__dBperNeper:
            printf("dB/neper\n");
            break;
        case SURFACE_ATTEN_UNITS__qFactor:
            printf("Q factor\n");
            break;
        case SURFACE_ATTEN_UNITS__dBperLambda:
            printf("dB/<lambda>\n");
            break;
    }

    printf("batimetry.numSurfaceCoords:\t%lu\n", (long unsigned int)settings->batimetry.numSurfaceCoords);

    printf("batimetry.surfaceProperties:    ");
    switch(settings->batimetry.surfacePropertyType){
        case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
            printf("cp:%lf\n",  settings->batimetry.cp[0]);
            printf("\t\t\t\tcs:%lf\n",  settings->batimetry.cs[0]);
            printf("\t\t\t\trho:%lf\n", settings->batimetry.rho[0]);
            printf("\t\t\t\tap:%lf\n",  settings->batimetry.ap[0]);
            printf("\t\t\t\tas:%lf\n",  settings->batimetry.as[0]);
            break;
        case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
            for(i=0; i<settings->batimetry.numSurfaceCoords; i++){
                printf("cp:%lf; ",  settings->batimetry.cp[i]);
                printf("cs:%lf; ",  settings->batimetry.cs[i]);
                printf("rho:%lf; ", settings->batimetry.rho[i]);
                printf("ap:%lf; ",  settings->batimetry.ap[i]);
                printf("as:%lf;\n", settings->batimetry.as[i]);
            }
            break;
    }

    printf("\noutput.arrayType: \t\t");
    switch(settings->output.arrayType){
        case ARRAY_TYPE__RECTANGULAR:
            printf("Rectangular\n");
            break;
        case ARRAY_TYPE__HORIZONTAL:
            printf("Horizontal\n");
            break;
        case ARRAY_TYPE__VERTICAL:
            printf("Vertical\n");
            break;
        case ARRAY_TYPE__LINEAR:
            printf("Linear\n");
            break;
    }
    printf("output.nArrayR: \t\t%lu\n",(long unsigned int)settings->output.nArrayR);
    printf("output.nArrayZ: \t\t%lu\n",(long unsigned int)settings->output.nArrayZ);

    printf("output.calcType: \t\t");
    switch(settings->output.calcType){
        case CALC_TYPE__RAY_COORDS:
            printf("Ray Coordinates\n");
            break;
        case CALC_TYPE__ALL_RAY_INFO:
            printf("All ray information\n");
            break;
        case CALC_TYPE__EIGENRAYS_REG_FALSI:
            printf("Eigenrays using Regula Falsi\n");
            break;
        case CALC_TYPE__EIGENRAYS_PROXIMITY:
            printf("Eigenrays using Proximity method\n");
            break;
        case CALC_TYPE__AMP_DELAY_REG_FALSI:
            printf("Amplitudes and Delays using Regula Falsi\n");
            break;
        case CALC_TYPE__AMP_DELAY_PROXIMITY:
            printf("Amplitudes and Delays using Proximity method\n");
            break;
        case CALC_TYPE__COH_ACOUS_PRESS:
            printf("Coherent Acoustic Pressure\n");
            break;
        case CALC_TYPE__COH_TRANS_LOSS:
            printf("Coherent Transmission loss\n");
            break;
        case CALC_TYPE__PART_VEL:
            printf("Coherent Particle Velocity\n");
            break;
        case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
            printf("Coherent Acoustic Pressure and Particle Velocity\n");
            break;
    }
    printf("output.miss: \t\t\t%12.5lf\n",settings->output.miss);

    fflush(stdout); // Will now print everything in the stdout buffer

    DEBUG(1, "out\n");
}

ray_t*              makeRay(uintptr_t numRays){
    /*
     * allocates rays and initializes its members
     */
    ray_t*      tempRay = NULL;
    uintptr_t   i;
     
    tempRay = malloc(numRays * sizeof(ray_t));
    if(tempRay == NULL){
        fatal("Memory alocation error.");
    }

    for(i=0; i<numRays; i++){
        tempRay[i].r            = NULL;
        tempRay[i].z            = NULL;
        tempRay[i].c            = NULL;
        tempRay[i].iRefl        = NULL;
        tempRay[i].decay        = NULL;
        tempRay[i].phase        = NULL;
        tempRay[i].tau          = NULL;
        tempRay[i].s            = NULL;
        tempRay[i].ic           = NULL;
        tempRay[i].boundaryTg   = NULL;
        tempRay[i].boundaryJ    = NULL;
        tempRay[i].rRefrac      = NULL;
        tempRay[i].zRefrac      = NULL;
        tempRay[i].p            = NULL;
        tempRay[i].q            = NULL;
        tempRay[i].caustc       = NULL;
        tempRay[i].amp          = NULL;
    }
    return tempRay;
}

void                reallocRayMembers(ray_t* ray, uintptr_t numRayCoords){
    /*
     * resizes all ray members.
     */
    DEBUG(5,"reallocRayMembers(%u),\t in\n", (uint32_t)numRayCoords);
    ray->nCoords    = numRayCoords;
    ray->r          = reallocFloat(    ray->r,         numRayCoords);
    ray->z          = reallocFloat(    ray->z,         numRayCoords);
    ray->c          = reallocFloat(    ray->c,         numRayCoords);
    ray->iRefl      = reallocBool(      ray->iRefl,     numRayCoords);
    ray->decay      = reallocComplex(   ray->decay,     numRayCoords);
    ray->phase      = reallocFloat(    ray->phase,     numRayCoords);
    ray->tau        = reallocFloat(    ray->tau,       numRayCoords);
    ray->s          = reallocFloat(    ray->s,         numRayCoords);
    ray->ic         = reallocFloat(    ray->ic,        numRayCoords);
    ray->boundaryTg = reallocVector(    ray->boundaryTg,numRayCoords);
    ray->boundaryJ  = reallocInt(       ray->boundaryJ, numRayCoords);
    ray->nRefrac    = (uint32_t)numRayCoords;
    ray->rRefrac    = reallocFloat(    ray->rRefrac,   numRayCoords);
    ray->zRefrac    = reallocFloat(    ray->zRefrac,   numRayCoords);
    ray->p          = reallocFloat(    ray->p,         numRayCoords);
    ray->q          = reallocFloat(    ray->q,         numRayCoords);
    ray->caustc     = reallocFloat(    ray->caustc,    numRayCoords);
    ray->amp        = reallocComplex(   ray->amp,       numRayCoords);
    DEBUG(5,"reallocRayMembers(), \t out\n");
}



