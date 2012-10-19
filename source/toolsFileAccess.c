/****************************************************************************************
 * toolsFileAccess.c                                                                    *
 * Collection of file access utility functions.                                         *
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

#pragma once
#include <string.h>
#include <stdint.h>
#include "globals.h"
#include "toolsMisc.c"
#include "toolsMemory.c"


///Prototypes:

FILE*           openFile(const char* , const char[4]);
double          readDouble(FILE*);
int32_t         readInt(FILE*);
char*           readStringN(FILE*, uint32_t);
void            skipLine(FILE*);



///Functions:

FILE*       openFile(const char *filename, const char mode[4]) {
    /* 
        Opens a file and returns a filepointer in case of success, exits with error code otherwise.
        Input values:
            filename    A string containing a full or relative path to the file.
            mode        A string containg the file acces mode.
        
        Return Value:
            A FILE pointer.
    */
    
    FILE *temp = NULL;
    if (VERBOSE)
        printf("Accessing file: %s... ", filename);
        
    temp=fopen(filename, mode);
    if(temp==NULL) {
        printf("Error while opening file '%s'.\n", filename);
        fatal("Aborting.\n");
        exit(EXIT_FAILURE);     //this is redundant but avoids "control may reach end of non-void function" warning
    } else {
        if (VERBOSE)
            printf("Ok.\n");
        return temp;
    }
}

double      readDouble(FILE* infile){
    /************************************************
     *  Reads a double from a file and returns it   *
     ***********************************************/
     
    char*   junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));;
    double  tempDouble;
    int32_t junkInt;
    
    junkInt = fscanf(infile, "%s\n", junkString);
    (void)junkInt;
    tempDouble = atof(junkString);
    free(junkString);
    
    return(tempDouble);
}

int32_t     readInt(FILE* infile){
    /************************************************
     *  Reads a int from a file and returns it      *
     ***********************************************/
     
    char*       junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
    int32_t     tempInt;
    int32_t     junkInt;
    
    junkInt = fscanf(infile, "%s\n", junkString);
    (void)junkInt;
    tempInt = (int32_t)atol(junkString);
    free(junkString);
    
    return(tempInt);
}

char*       readStringN(FILE* infile, uint32_t length){
    /********************************************************************
     *  Reads a <lenght> chars from a filestream.                       *
     *******************************************************************/
    char*       outputString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));;
    char*       junkChar;

    junkChar = fgets(outputString, (int32_t)length, infile);
    (void)junkChar;
    return(outputString);
}

void        skipLine(FILE* infile){
    /************************************************
     *  Reads a int from a file and returns it      *
     ***********************************************/
    char*       junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
    char*       junkChar;
    
    junkChar = fgets(junkString, MAX_LINE_LEN+1, infile);
    (void)junkChar;
    
    free(junkString);
}

