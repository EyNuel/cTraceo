/****************************************************************************************
 * readIn.c                                                                             *
 * (formerly "readin.for")                                                              *
 * read the waveguide input file.                                                       *
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
 * Inputs:                                                                              *
 *          settings:   Pointer to structure containing all input info.                 *
 *          inFile:     A file pointer to an open input file (may also be a redirected  *
 *                      standard input "stdin").                                        *
 *                                                                                      *
 * Outputs:                                                                             *
 *          None:       All values are stored in the "settings" structure.              *
 *                                                                                      *
 * Return Value:                                                                        *
 *          None                                                                        *
 *                                                                                      *
 ****************************************************************************************/

#include <stdio.h>
#include <inttypes.h>       //contains definitions of integer data types that are inequivocal.
#include "tools.h"          
#include "globals.h"        //Include global variables
#include <math.h>

//prototype:
void    readIn(settings_t* settings, FILE* inFile);

//actual function declaration:
void    readIn(settings_t* settings, FILE* inFile){

    uint32_t    i,j;
    double      dTheta;
    uint32_t    nThetas;            //used locally to make code more readable. Value is stored in settings.
    double      theta0;             //used locally to make code more readable. Value is stored in settings.
    double      thetaN;             //used locally to make code more readable. Value is stored in settings.
    uint32_t    numSurfaceCoords;   //used locally to make code more readable. Value is stored in settings.
    uint32_t    nr, nz;             //used locally to make code more readable. Value is stored in settings.
    char*       tempString;
    int32_t     tempInt;
    char*       junkChar;
    

    /************************************************************************
     *  Read the title:                                                     *
     ***********************************************************************/
    junkChar = fgets(settings->cTitle, MAX_LINE_LEN+1, inFile);
    (void)junkChar;


    /************************************************************************
     *  Read and validate the source info:                                  *
     ***********************************************************************/
     DEBUG(2, "Reading source info.\n");
     skipLine(inFile);
     settings->source.ds        = readDouble(inFile);
     settings->source.rx        = readDouble(inFile);
     settings->source.zx        = readDouble(inFile);
     settings->source.rbox1     = readDouble(inFile);
     settings->source.rbox2     = readDouble(inFile);
     settings->source.freqx     = readDouble(inFile);
     
     /*
      * Read the number of launching angles.
      * NOTE: There are 2 methods of encoding ray launching angles in the input file,
      *       both encode this information on two lines of the input text file.
      *         a) classic: the number of rays on the first line, followed on the 2nd line
      *            by a "start" and "end" angle. The launching angles will then be linealy spaced
      *            between these 2 angles.
      *         b) the number of rays on the first line, followed by a second line containing
      *            all the launching angles, as well as the "dTheta" parameter.
      *       Method b is identified by saving the number of launched rays as a negative value.
      */
     tempInt    = readInt(inFile);
     nThetas    = (uint32_t)abs(tempInt);   
     settings->source.nThetas   = nThetas;
    
    /*  Allocate memory for the launching angles    */
    settings->source.thetas = mallocDouble(nThetas);
    DEBUG(8,"number of launching angles in inFile: %u\n", nThetas);
    
    /*  Read the  thetas from the file, according to method a or b (see above)  */
    if (tempInt > 0){
        //method a:
        DEBUG(3, "reading launching angles according to method a.\n");
        theta0 = readDouble(inFile);
        thetaN = readDouble(inFile);
        if(settings->source.nThetas == 1){
            settings->source.thetas[0] = theta0;
            dTheta = 0;
        }else if(settings->source.nThetas == 2){
            settings->source.thetas[0] = theta0;
            settings->source.thetas[1] = thetaN;
            dTheta = thetaN - theta0;
        }else{
            settings->source.thetas[0] = theta0;
            settings->source.thetas[nThetas - 1] = thetaN;
            dTheta =    (thetaN - theta0 ) / ( (double)nThetas - 1 );
            
            for(i=1;i <= nThetas-2; i++){
                settings->source.thetas[i] = theta0 +dTheta *(double)(i);
            }
        }
    
    }else{
        //method b:
        DEBUG(3, "reading launching angles according to method a.\n");
        
        //read dTheta from the beginning of the 2nd line:
        dTheta = readDouble(inFile);
        
        //read all the launching angles from the file:
        for (i=0; i<nThetas; i++){
            settings->source.thetas[i] = readDouble(inFile);
        }
        
    }
    settings->source.dTheta = dTheta;
    
    
   
    DEBUG(10,"\n");
    
    /*  Source validation   */
    if(settings->source.ds == 0.0 ){
        settings->source.ds = fabs( settings->source.rbox2 - settings->source.rbox1)/1000;
    }
    if( (settings->source.rx < settings->source.rbox1) ||
        (settings->source.rx > settings->source.rbox2)){
        fatal(  "Input file: Source: initial range is outside the range box!\nAborting...");
    }
    
    
    /************************************************************************
     * Read and validate altimetry info:                                    *
     ***********************************************************************/
    DEBUG(2, "Reading altimetry info.\n");
    skipLine(inFile);

    /* surfaceType; formerly "atype"    */
    tempString = readStringN(inFile,5);
    if(strcmp(tempString,"'A'\n") == 0){
        settings->altimetry.surfaceType = SURFACE_TYPE__ABSORVENT;
        
    }else if(strcmp(tempString,"'E'\n") == 0){
        settings->altimetry.surfaceType = SURFACE_TYPE__ELASTIC;
        
    }else if(strcmp(tempString,"'R'\n") == 0){
        settings->altimetry.surfaceType = SURFACE_TYPE__RIGID;
        
    }else if(strcmp(tempString,"'V'\n") == 0){
        settings->altimetry.surfaceType = SURFACE_TYPE__VACUUM;
        
    }else{
        printf("Input file: batimetry: unknown surface type %s\n", tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* surfacePropertyType;     //formerly "aptype" */
    tempString = readStringN(inFile,5);
    if(strcmp(tempString,"'H'\n") == 0){
        settings->altimetry.surfacePropertyType = SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
        
    }else if(strcmp(tempString,"'N'\n") == 0){
        settings->altimetry.surfacePropertyType = SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
        
    }else{
        printf("Input file: altimetry: surface property type: '%s'\n",tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* surfaceInterpolation;    //formerly "aitype" */
    tempString = readStringN(inFile,6);
    if(strcmp(tempString,"'FL'\n") == 0){
        settings->altimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__FLAT;
        
    }else if(strcmp(tempString,"'SL'\n") == 0){
        settings->altimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__SLOPED;
        
    }else if(strcmp(tempString,"'2P'\n") == 0){
        settings->altimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__2P;
        
    }else if(strcmp(tempString,"'3P'\n") == 0){
        settings->altimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__3P;
        
    }else if(strcmp(tempString,"'4P'\n") == 0){
        settings->altimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__4P;
        
    }else{
        printf("Input file: altimetry: unknown surface interpolation type: '%s'\n",tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* surfaceAttenUnits;       //formerly "atiu"   */
    tempString = readStringN(inFile,5);
    if(strcmp(tempString,"'F'\n") == 0){
        settings->altimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperkHz;
        
    }else if(strcmp(tempString,"'M'\n") == 0){
        settings->altimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperMeter;
        
    }else if(strcmp(tempString,"'N'\n") == 0){
        settings->altimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperNeper;
        
    }else if(strcmp(tempString,"'Q'\n") == 0){
        settings->altimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__qFactor;
        
    }else if(strcmp(tempString,"'W'\n") == 0){
        settings->altimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperLambda;
        
    }else{
        printf("Input file: altimetry: unknown surface attenuation units: '%s'\n",tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* numSurfaceCoords;        //formerly "nati" */
    numSurfaceCoords = (uint32_t)readInt(inFile);
    settings->altimetry.numSurfaceCoords = numSurfaceCoords;

    //malloc interface coords
    settings->altimetry.r = mallocDouble(numSurfaceCoords);
    settings->altimetry.z = mallocDouble(numSurfaceCoords);
    
    //read the surface properties and coordinates
    switch(settings->altimetry.surfacePropertyType){
        case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
            //malloc and read only one set of interface properties:
            settings->altimetry.cp  = mallocDouble(1);
            settings->altimetry.cs  = mallocDouble(1);
            settings->altimetry.rho = mallocDouble(1);
            settings->altimetry.ap  = mallocDouble(1);
            settings->altimetry.as  = mallocDouble(1);
            
            settings->altimetry.cp[0]   = readDouble(inFile);
            settings->altimetry.cs[0]   = readDouble(inFile);
            settings->altimetry.rho[0]  = readDouble(inFile);
            settings->altimetry.ap[0]   = readDouble(inFile);
            settings->altimetry.as[0]   = readDouble(inFile);
            
            //read coordinates of interface points:
            for(i=0; i<numSurfaceCoords; i++){
                settings->altimetry.r[i] = readDouble(inFile);
                settings->altimetry.z[i] = readDouble(inFile);
            }
            break;
        
        case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
            //Read coordinates and interface properties for all interface points:
            settings->altimetry.cp  = mallocDouble(numSurfaceCoords);
            settings->altimetry.cs  = mallocDouble(numSurfaceCoords);
            settings->altimetry.rho = mallocDouble(numSurfaceCoords);
            settings->altimetry.ap  = mallocDouble(numSurfaceCoords);
            settings->altimetry.as  = mallocDouble(numSurfaceCoords);
                
            for(i=0; i<numSurfaceCoords; i++){
                settings->altimetry.r[i]    = readDouble(inFile);
                settings->altimetry.z[i]    = readDouble(inFile);
                settings->altimetry.cp[i]   = readDouble(inFile);
                settings->altimetry.cs[i]   = readDouble(inFile);
                settings->altimetry.rho[i]  = readDouble(inFile);
                settings->altimetry.ap[i]   = readDouble(inFile);
                settings->altimetry.as[i]   = readDouble(inFile);
            }
            break;
    }

    /************************************************************************
     * Read and validate sound speed info:                                  *
     ***********************************************************************/
    DEBUG(2, "Reading sound speed info.\n");
    /*  sound speed distribution    "cdist"     */
    skipLine(inFile);
    tempString = readStringN(inFile,10);
    if(strcmp(tempString,"'c(z,z)'\n") == 0){
        DEBUG(3, "c(z,z)\n");
        settings->soundSpeed.cDist  = C_DIST__PROFILE;
    }else if(strcmp(tempString,"'c(r,z)'\n") == 0){
        DEBUG(3, "c(r,z)\n");
        settings->soundSpeed.cDist  = C_DIST__FIELD;
    }else{
        printf("Input file: Sound Speed: unknown sound speed distribution type: '%s'\n",tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /*  sound speed class   "cclass"        */
    tempString = readStringN(inFile,7);
    if(strcmp(tempString,"'ISOV'") == 0){
        settings->soundSpeed.cClass = C_CLASS__ISOVELOCITY;
        
    }else if(strcmp(tempString,"'LINP'") == 0){
        settings->soundSpeed.cClass = C_CLASS__LINEAR;
        
    }else if(strcmp(tempString,"'PARP'") == 0){
        settings->soundSpeed.cClass = C_CLASS__PARABOLIC;
        
    }else if(strcmp(tempString,"'EXPP'") == 0){
        settings->soundSpeed.cClass = C_CLASS__EXPONENTIAL;
        
    }else if(strcmp(tempString,"'N2LP'") == 0){
        settings->soundSpeed.cClass = C_CLASS__N2_LINEAR;
        
    }else if(strcmp(tempString,"'ISQP'") == 0){
        settings->soundSpeed.cClass = C_CLASS__INV_SQUARE;
        
    }else if(strcmp(tempString,"'MUNK'") == 0){
        settings->soundSpeed.cClass = C_CLASS__MUNK;
        
    }else if(strcmp(tempString,"'TABL'") == 0){
        settings->soundSpeed.cClass = C_CLASS__TABULATED;
        DEBUG(3, "TABL\n");
    }else{
        printf("Input file: Sound Speed: unknown sound class type: '%s'\n",tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* number of points in range and depth, "nr0,nz0" */
    nr = (uint32_t)readInt(inFile);
    nz = (uint32_t)readInt(inFile);
    DEBUG(3, "nr0: %u, nz0: %u\n", nr, nz);
    settings->soundSpeed.nr = nr;
    settings->soundSpeed.nz = nz;

    //read actual values of soundspeed profile/field:
    switch(settings->soundSpeed.cDist){
        
        case C_DIST__PROFILE:
            DEBUG(3, "Reading sound speed profile\n");
            if(settings->soundSpeed.cClass != C_CLASS__TABULATED){
                //all cClasses execept for "TABULATED" only require ( z0(0),c0(0) ) and ( z0(1), c0(1) )
                //malloc z0 an c0:
                settings->soundSpeed.z = mallocDouble(2);
                settings->soundSpeed.c1D = mallocDouble(2);

                //read 4 values:
                settings->soundSpeed.z[0] = readDouble(inFile);
                settings->soundSpeed.c1D[0] = readDouble(inFile);
                settings->soundSpeed.z[1] = readDouble(inFile);
                settings->soundSpeed.c1D[1] = readDouble(inFile);

                // validate the values that were just read:
                if( (settings->soundSpeed.cClass != C_CLASS__ISOVELOCITY) &&
                    (settings->soundSpeed.cClass != C_CLASS__MUNK)){
                    
                    if(settings->soundSpeed.z[0] == settings->soundSpeed.z[1])
                        fatal("Input file: Analytical sound speed: z[1] == z[0] Only valid for Isovelocity and Munk Options!\nAborting...");

                    if(settings->soundSpeed.c1D[0] == settings->soundSpeed.c1D[1])
                        fatal("Input file: Analytical sound speed: c[1] == c[0] Only valid for Isovelocity option!\nAborting...");
                }
            }else if(settings->soundSpeed.cClass == C_CLASS__TABULATED){
                //malloc z0 an c0:
                settings->soundSpeed.z = mallocDouble(nz);
                settings->soundSpeed.c1D = mallocDouble(nz);

                //read pairs of z0 and c0
                for(i=0; i<settings->soundSpeed.nz; i++){
                    settings->soundSpeed.z[i]   = readDouble(inFile);
                    settings->soundSpeed.c1D[i]= readDouble(inFile);
                }
            }
            break;
        
        case C_DIST__FIELD:
            DEBUG(3, "Reading sound speed field\n");
            if(settings->soundSpeed.cClass != C_CLASS__TABULATED)
                fatal("Input file: Unknown sound speed field type.\nAborting...");
            
            //malloc ranges (vector)
            settings->soundSpeed.r = mallocDouble(nr);
            //read ranges
            for(i=0; i<nr; i++)
                settings->soundSpeed.r[i] = readDouble(inFile);

            //malloc depths (vector)
            settings->soundSpeed.z = mallocDouble(nz);
            //read depths
            for(i=0; i<nz; i++)
                settings->soundSpeed.z[i] = readDouble(inFile);

            //malloc sound speeds (2 dim matrix)
            settings->soundSpeed.c2D = mallocDouble2D(nz, nr);  //mallocDouble2D(numCols, numRows)
            //read actual sound speeds
            for(j=0; j<nz; j++){        //rows
                for(i=0; i<nr; i++){    //columns
                    settings->soundSpeed.c2D[j][i] = readDouble(inFile);
                }
            }
            break;
    }

    /************************************************************************
     * Read and validate object info:                                       *
     ***********************************************************************/
    DEBUG(2, "Reading object info.\n");
    skipLine(inFile);
    settings->objects.numObjects = (uint32_t)readInt(inFile);

    /* only attempt to read object info if at least one object exists:  */
    if(settings->objects.numObjects > 0){
        //malloc memory for the objects:
        settings->objects.object = malloc((uintptr_t)settings->objects.numObjects * sizeof(object_t));
        if (settings->objects.object == NULL)
            fatal("Memory allocation error.");

        /*  interpolation type  (formerly "oitype")     */
        tempString = readStringN(inFile,6);
        if(strcmp(tempString,"'2P'\n") == 0){
            settings->objects.surfaceInterpolation  = SURFACE_INTERPOLATION__2P;
            
        }else if(strcmp(tempString,"'3P'\n") == 0){
            settings->objects.surfaceInterpolation  = SURFACE_INTERPOLATION__3P;
            
        }else if(strcmp(tempString,"'4P'\n") == 0){
            settings->objects.surfaceInterpolation  = SURFACE_INTERPOLATION__4P;
            
        }else{
            printf("Input file: altimetry: unknown surface interpolation type: '%s'\n",tempString);
            fatal("Aborting...");
        }
        free(tempString);
        
        for(i=0; i<settings->objects.numObjects; i++){
            /*  surfaceType     */
            tempString = readStringN(inFile,5);
            if(strcmp(tempString,"'A'\n") == 0){
                settings->objects.object[i].surfaceType = SURFACE_TYPE__ABSORVENT;
                
            }else if(strcmp(tempString,"'E'\n") == 0){
                settings->objects.object[i].surfaceType = SURFACE_TYPE__ELASTIC;
                
            }else if(strcmp(tempString,"'R'\n") == 0){
                settings->objects.object[i].surfaceType = SURFACE_TYPE__RIGID;
                
            }else if(strcmp(tempString,"'V'\n") == 0){
                settings->objects.object[i].surfaceType = SURFACE_TYPE__VACUUM;
                
            }else{
                printf("Input file: Object %u: unknown surface type: %s\n", i, tempString);
                fatal("Aborting...");
            }
            free(tempString);
            
            /* surfaceAttenUnits    */
            tempString = readStringN(inFile,5);
            if(strcmp(tempString,"'F'\n") == 0){
                settings->objects.object[i].surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperkHz;
                
            }else if(strcmp(tempString,"'M'\n") == 0){
                settings->objects.object[i].surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperMeter;
                
            }else if(strcmp(tempString,"'N'\n") == 0){
                settings->objects.object[i].surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperNeper;
                
            }else if(strcmp(tempString,"'Q'\n") == 0){
                settings->objects.object[i].surfaceAttenUnits   = SURFACE_ATTEN_UNITS__qFactor;
                
            }else if(strcmp(tempString,"'W'\n") == 0){
                settings->objects.object[i].surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperLambda;
                
            }else{
                printf("Input file: Object %u: unknown surface attenuation units: %s\n", i, tempString);
                fatal("Aborting...");
            }
            free(tempString);
            
            settings->objects.object[i].nCoords = (uint32_t)readInt(inFile);
            settings->objects.object[i].cp      = readDouble(inFile);               //compressional speed
            settings->objects.object[i].cs      = readDouble(inFile);               //shear speed
            settings->objects.object[i].rho     = readDouble(inFile);               //density
            settings->objects.object[i].ap      = readDouble(inFile);               //compressional attenuation
            settings->objects.object[i].as      = readDouble(inFile);               //shear attenuation

            //malloc memory for the object coordinates:
            settings->objects.object[i].r       = mallocDouble((uintptr_t)settings->objects.object[i].nCoords);
            settings->objects.object[i].zDown   = mallocDouble((uintptr_t)settings->objects.object[i].nCoords);
            settings->objects.object[i].zUp     = mallocDouble((uintptr_t)settings->objects.object[i].nCoords);
            //verify succesfull allocation of memory:
            if( (settings->objects.object[i].r      ==  NULL)   ||
                (settings->objects.object[i].zDown  ==  NULL)   ||
                (settings->objects.object[i].zUp    ==  NULL))
                fatal("Memory allocation error.");
            
            //read the coords of the object:
            for(j=0; j<settings->objects.object[i].nCoords; j++){
                settings->objects.object[i].r[j]        = readDouble(inFile);
                settings->objects.object[i].zDown[j]    = readDouble(inFile);
                settings->objects.object[i].zUp[j]      = readDouble(inFile);
            }
        }
    }
    
    /************************************************************************
     * Read and validate batimetry info:                                    *
     ***********************************************************************/
    DEBUG(2, "Reading batimetry info.\n");
    skipLine(inFile);

    /* surfaceType; formerly "atype"    */
    tempString = readStringN(inFile,5);
    if(strcmp(tempString,"'A'\n") == 0){
        settings->batimetry.surfaceType = SURFACE_TYPE__ABSORVENT;
        
    }else if(strcmp(tempString,"'E'\n") == 0){
        settings->batimetry.surfaceType = SURFACE_TYPE__ELASTIC;
        
    }else if(strcmp(tempString,"'R'\n") == 0){
        settings->batimetry.surfaceType = SURFACE_TYPE__RIGID;
        
    }else if(strcmp(tempString,"'V'\n") == 0){
        settings->batimetry.surfaceType = SURFACE_TYPE__VACUUM;
        
    }else{
        printf("Input file: batimetry: unknown surface type: %s\n", tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* surfacePropertyType;     //formerly "aptype" */
    tempString = readStringN(inFile,5);
    if(strcmp(tempString,"'H'\n") == 0){
        settings->batimetry.surfacePropertyType = SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
        
    }else if(strcmp(tempString,"'N'\n") == 0){
        settings->batimetry.surfacePropertyType = SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
        
    }else{
        printf("Input file: batimetry: unknown surface property type: %s\n", tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* surfaceInterpolation;    //formerly "aitype" */
    tempString = readStringN(inFile,6);
    if(strcmp(tempString,"'FL'\n") == 0){
        settings->batimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__FLAT;
        
    }else if(strcmp(tempString,"'SL'\n") == 0){
        settings->batimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__SLOPED;
        
    }else if(strcmp(tempString,"'2P'\n") == 0){
        settings->batimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__2P;
        
    }else if(strcmp(tempString,"'3P'\n") == 0){
        settings->batimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__3P;
        
    }else if(strcmp(tempString,"'4P'\n") == 0){
        settings->batimetry.surfaceInterpolation    = SURFACE_INTERPOLATION__4P;
        
    }else{
        printf("Input file: batimetry: unknown surface interpolation type: %s\n", tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* surfaceAttenUnits;       //formerly "atiu"   */
    tempString = readStringN(inFile,5);
    if(strcmp(tempString,"'F'\n") == 0){
        settings->batimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperkHz;
        
    }else if(strcmp(tempString,"'M'\n") == 0){
        settings->batimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperMeter;
        
    }else if(strcmp(tempString,"'N'\n") == 0){
        settings->batimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperNeper;
        
    }else if(strcmp(tempString,"'Q'\n") == 0){
        settings->batimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__qFactor;
        
    }else if(strcmp(tempString,"'W'\n") == 0){
        settings->batimetry.surfaceAttenUnits   = SURFACE_ATTEN_UNITS__dBperLambda;
        
    }else{
        printf("Input file: batimetry: unknown surface attenuation units: %s\n", tempString);
        fatal("Aborting...");
    }
    free(tempString);

    /* numSurfaceCoords;        //formerly "nati" */
    numSurfaceCoords = (uint32_t)readInt(inFile);
    settings->batimetry.numSurfaceCoords = numSurfaceCoords;

    //malloc interface coords
    settings->batimetry.r = mallocDouble(numSurfaceCoords);
    settings->batimetry.z = mallocDouble(numSurfaceCoords);
    
    //read the surface properties and coordinates
    switch(settings->batimetry.surfacePropertyType){
        case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
            //malloc and read only one set of interface properties:
            settings->batimetry.cp  = mallocDouble(1);
            settings->batimetry.cs  = mallocDouble(1);
            settings->batimetry.rho = mallocDouble(1);
            settings->batimetry.ap  = mallocDouble(1);
            settings->batimetry.as  = mallocDouble(1);
                            
            settings->batimetry.cp[0]   = readDouble(inFile);
            settings->batimetry.cs[0]   = readDouble(inFile);
            settings->batimetry.rho[0]  = readDouble(inFile);
            settings->batimetry.ap[0]   = readDouble(inFile);
            settings->batimetry.as[0]   = readDouble(inFile);
            
            //read coordinates of interface points:
            for(i=0; i<numSurfaceCoords; i++){
                settings->batimetry.r[i] = readDouble(inFile);
                settings->batimetry.z[i] = readDouble(inFile);
            }
            break;
        
        case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
            //Read coordinates and interface properties for all interface points:
            settings->batimetry.cp  = mallocDouble(numSurfaceCoords);
            settings->batimetry.cs  = mallocDouble(numSurfaceCoords);
            settings->batimetry.rho = mallocDouble(numSurfaceCoords);
            settings->batimetry.ap  = mallocDouble(numSurfaceCoords);
            settings->batimetry.as  = mallocDouble(numSurfaceCoords);
                
            for(i=0; i<numSurfaceCoords; i++){
                settings->batimetry.r[i]    = readDouble(inFile);
                settings->batimetry.z[i]    = readDouble(inFile);
                settings->batimetry.cp[i]   = readDouble(inFile);
                settings->batimetry.cs[i]   = readDouble(inFile);
                settings->batimetry.rho[i]  = readDouble(inFile);
                settings->batimetry.ap[i]   = readDouble(inFile);
                settings->batimetry.as[i]   = readDouble(inFile);
            }
            break;
    }
    
    /************************************************************************
     * Read and validate hydrophone array info:                             *
     ***********************************************************************/
     DEBUG(2, "Reading array info.\n");
    skipLine(inFile);
    /*  output array type "artype"  */
    tempString = readStringN(inFile,6);
    if(strcmp(tempString,"'RRY'") == 0){
        settings->output.arrayType  = ARRAY_TYPE__RECTANGULAR;
        
    }else if(strcmp(tempString,"'HRY'") == 0){
        settings->output.arrayType  = ARRAY_TYPE__HORIZONTAL;
        
    }else if(strcmp(tempString,"'VRY'") == 0){
        settings->output.arrayType  = ARRAY_TYPE__VERTICAL;
        
    }else if(strcmp(tempString,"'LRY'") == 0){
        settings->output.arrayType  = ARRAY_TYPE__LINEAR;
        
    }else{
        fatal("Input file: output: unknown array type.\nAborting...");
    }
    free(tempString);
    
    /*  output array dimensions "nra, nrz"      */
    settings->output.nArrayR = (uint32_t)readInt(inFile);
    settings->output.nArrayZ = (uint32_t)readInt(inFile);

    /* validate the array dimensions            */
    switch(settings->output.arrayType){
        case ARRAY_TYPE__LINEAR:
            if (settings->output.nArrayR != settings->output.nArrayZ){
                fatal("Input file: Linear array: number of range and depth coordinates must match.\nAborting.");
            }
            break;
        case ARRAY_TYPE__HORIZONTAL:
            if(settings->output.nArrayZ != 1){
                fatal("Input file: Horizontal array: number of hydrophone elements in Z must be 1.\nAborting.");
            }
            break;
        case ARRAY_TYPE__VERTICAL:
            if(settings->output.nArrayR != 1){
                fatal("Input file: Vertical array: number of hydrophone elements in R must be 1.\nAborting.");
            }
            break;
    }

    settings->output.arrayR = mallocDouble(settings->output.nArrayR);
    settings->output.arrayZ = mallocDouble(settings->output.nArrayZ);

    //read the actual array values
    for(i=0; i<settings->output.nArrayR; i++){
        settings->output.arrayR[i] = readDouble(inFile);
    }
    for(i=0; i<settings->output.nArrayZ; i++){
        settings->output.arrayZ[i] = readDouble(inFile);
    }

    /************************************************************************
     * Read and validate output settings:
     ***********************************************************************/
    DEBUG(2, "Reading output settings.\n");
    skipLine(inFile);
    
    /*  output calculation type "catype"    */
    tempString = readStringN(inFile,6);
    if(strcmp(tempString,"'RCO'") == 0){
        settings->output.calcType   = CALC_TYPE__RAY_COORDS;
        
    }else if(strcmp(tempString,"'ARI'") == 0){
        settings->output.calcType   = CALC_TYPE__ALL_RAY_INFO;
        
    }else if(strcmp(tempString,"'ERF'") == 0){
        settings->output.calcType   = CALC_TYPE__EIGENRAYS_REG_FALSI;
        
    }else if(strcmp(tempString,"'EPR'") == 0){
        settings->output.calcType   = CALC_TYPE__EIGENRAYS_PROXIMITY;
        
    }else if(strcmp(tempString,"'ADR'") == 0){
        settings->output.calcType   = CALC_TYPE__AMP_DELAY_REG_FALSI;
        
    }else if(strcmp(tempString,"'ADP'") == 0){
        settings->output.calcType   = CALC_TYPE__AMP_DELAY_PROXIMITY;
        
    }else if(strcmp(tempString,"'CPR'") == 0){
        settings->output.calcType   = CALC_TYPE__COH_ACOUS_PRESS;
        
    }else if(strcmp(tempString,"'CTL'") == 0){
        settings->output.calcType   = CALC_TYPE__COH_TRANS_LOSS;
        
    }else if(strcmp(tempString,"'PVL'") == 0){
        settings->output.calcType   = CALC_TYPE__PART_VEL;
        
    }else if(strcmp(tempString,"'PAV'") == 0){
        settings->output.calcType   = CALC_TYPE__COH_ACOUS_PRESS_PART_VEL;
        
    }else{

        fatal("Input file: unknown output calculation type.\nAborting...");
    }
    free(tempString);

    /*  output calculation type "catype"    */
    settings->output.miss = readDouble(inFile);

    /* Check batimetry/altimetry    */
    if(settings->altimetry.r[0] > settings->source.rbox1)
        fatal("Minimum altimetry range > minimum rbox range.\nAborting...");
    if(settings->altimetry.r[settings->altimetry.numSurfaceCoords-1] < settings->source.rbox2)
        fatal("Maximum altimetry range < maximum rbox range.\nAborting...");
    if(settings->batimetry.r[0] > settings->source.rbox1)
        fatal("Minimum batimetry range > minimum rbox range.\nAborting...");
    if(settings->batimetry.r[settings->batimetry.numSurfaceCoords-1] < settings->source.rbox2)
        fatal("Maximum batimetry range < maximum rbox range.\nAborting...");
    DEBUG(1, "out\n");

    //close the input file
    fclose(inFile);
}


