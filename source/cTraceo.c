/****************************************************************************************
 *  cTraceo.c                                                                           *
 *  (formerly "trace0.for")                                                             *
 *  The main cTraceo file. This is where the story begins.                              *
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
 *  Command line arguments:                                                             *
 *          (TODO)
 ****************************************************************************************/

#include <stdio.h>
#include "globals.h"
#include "readIn.c"
#include "math.h"
#include "calcRayCoords.c"
#include "calcAllRayInfo.c"
#include "calcEigenrayPr.c"
#include "calcEigenrayRF.c"
#include "calcAmpDelPr.c"
#include "calcAmpDelRF.c"
#include "calcCohAcoustPress.c"
#include "calcCohTransLoss.c"
#include "calcParticleVel.c"
#include <sys/time.h>       //for struct timeval
#include <sys/resource.h>   //for getrusage()
#include <string.h>

void    printHelp(void);
int     main(int, char**);

void    printHelp(void){
    /*
     * Print usage help
     */
    printf("\n"
"* =========================================================================== *\n"
"*          The cTraceo Acoustic Raytracing Model, Version "VERSION"*\n"
"*                                                                             *\n"
"* --------------------------------------------------------------------------- *\n"
"* License: The cTraceo Acoustic Raytracing Model is released under the        *\n"
"*          Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported *\n"
"*          License ( http://creativecommons.org/licenses/by-nc-sa/3.0/ )      *\n"
"*                                                                             *\n"
"* NOTE:    cTraceo is research code under active development.                 *\n"
"*          The code may contain bugs and updates are possible in the future.  *\n"
"*                                                                             *\n"
"* Written for project SENSOCEAN by:                                           *\n"
"*          Emanuel Ey                                                         *\n"
"*          emanuel.ey@gmail.com                                               *\n"
"*          Copyright (C) 2011                                                 *\n"
"*          Signal Processing Laboratory                                       *\n"
"*          Universidade do Algarve                                            *\n"
"*                                                                             *\n"
"* cTraceo is the C port of the FORTRAN 77 TRACEO code written by:             *\n"
"*          Orlando Camargo Rodriguez:                                         *\n"
"*          Copyright (C) 2010                                                 *\n"
"*          Orlando Camargo Rodriguez                                          *\n"
"*          orodrig@ualg.pt                                                    *\n"
"*          Universidade do Algarve                                            *\n"
"*          Physics Department                                                 *\n"
"*          Signal Processing Laboratory                                       *\n"
"*                                                                             *\n"
"* ----------------------------------------------------------------------------*\n"
"*  Usage:                                                                     *\n"
"*          The only command line argument required by cTraceo is the name of  *\n"
"*          the input file, without it's extension. Something like:            *\n"
"*              $> ctraceo filename                                            *\n" 
"*                                                                             *\n"
"*          For more information check out the readme.txt, read the manual.pdf *\n"
"*          or contact the authors.                                            *\n"
"*                                                                             *\n"
"* =========================================================================== *\n");
    
}

int main(int argc, char **argv){
    char*           inFileName  = mallocChar(256);
    char*           logFileName = mallocChar(256);
    FILE*           inFile      = NULL;
    settings_t*     settings    = mallocSettings();
    double          omega;
    const char*     line = "-----------------------------------------------";
    FILE*           logFile = NULL;

    DEBUG(1,"Running cTraceo in verbose mode.\n\n");
    
    // check if a command line argument was passed:
    if(argc == 2){
        //check for command line options:
        if(argv[1][0] == '-'){
            //check if input file should be read from stdin:
            if(strlen(argv[1]) == 1){
                /*
                 * Read input file from stdin instead of from a file on disk.
                 * This avoids the overhead of writing to disk; intended for inversion uses.
                 * Same as long option "--stdin"
                 */
                 inFile = stdin;
            }
            
            //check for short options:
            else if(strlen(argv[1]) == 2){
                switch(argv[1][1]){
                    case 'h':
                        printHelp();
                        exit(EXIT_SUCCESS);
                        break;
                    case 'v':
                        printf(HEADER);
                        exit(EXIT_SUCCESS);
                        break;
                    default:
                        fatal("Unknown input option.\n");
                        break;
                }
            }
            
            //check for long options:
            else if (strlen(argv[1]) > 2){
                if (!strcmp(argv[1], "--stdin")){
                    /*
                     * Read input file from stdin instead of from a file on disk.
                     * This avoids the overhead of writing to disk; intended for inversion uses.
                     * Same as short option "-"
                     */
                    inFile = stdin;
                }
            }
        }
        
        //if no special options where passed, try to use command line argument it as an input file
        else{
            strcpy(inFileName, argv[1]);
            inFileName = strcat(inFileName, ".in");
            inFile = openFile(inFileName, "r");
        }
    
    //if no command line options where passed, complain and quit
    }else{
        printHelp();
        fatal("No input file provided.\nAborting...");
    }

    printf(HEADER);

    //Read the input file
    readIn(settings, inFile);

    if (VERBOSE){
        DEBUG(2, "Calling printSettings()\n");
        printSettings(settings);
        DEBUG(2, "Returned from printSettings()\n");
    }

    omega   = 2 * M_PI * settings->source.freqx;

    //open the log file and write the header:
    strcpy(logFileName, argv[1]);
    logFile= openFile(strcat(logFileName,".log"), "w");
    fprintf(logFile, "TRACEO ray tracing program.\n");
    fprintf(logFile, "TODO: write a nice header for the log file.\n");
    fprintf(logFile, "%s\n", line);

    fprintf(logFile, "INPUT:\n");
    fprintf(logFile, "%s\n", settings->cTitle);
    fprintf(logFile, "%s\n", line);

    fprintf(logFile, "OUTPUT:\n");
    switch(settings->output.calcType){
        case CALC_TYPE__RAY_COORDS:
            printf("Calculating ray coordinates.\n");
            fprintf(logFile, "Ray coordinates\n");
            calcRayCoords(settings);
            break;

        case CALC_TYPE__ALL_RAY_INFO:
            printf("Calculating all ray information.\n");
            fprintf(logFile, "All ray information\n");
            calcAllRayInfo(settings);
            break;
            
        case CALC_TYPE__EIGENRAYS_PROXIMITY:
            printf("Calculating eigenrays by Proximity Method.\n");
            fprintf(logFile, "Eigenrays by Proximity Method.\n");
            calcEigenrayPr(settings);
            break;
            
        case CALC_TYPE__EIGENRAYS_REG_FALSI:
            printf("Calculating eigenrays by Regula Falsi Method.\n");
            fprintf(logFile, "Eigenrays by Regula Falsi Method.\n");
            calcEigenrayRF(settings);
            break;
            
        case CALC_TYPE__AMP_DELAY_PROXIMITY:
            printf("Calculating amplitudes and delays by Proximity Method.\n");
            fprintf(logFile, "Amplitudes and delays by Proximity Method.\n");
            calcAmpDelPr(settings);
            break;
            
        case CALC_TYPE__AMP_DELAY_REG_FALSI:
            printf("Calculating amplitudes and delays by Regula Falsi Method.\n");
            fprintf(logFile, "Amplitudes and delays by Regula Falsi Method.\n");
            calcAmpDelRF(settings);
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS:
            printf("Calculating coherent acoustic pressure.\n");
            fprintf(logFile, "Coherent acoustic pressure.\n");
            calcCohAcoustPress(settings);
            break;
            
        case CALC_TYPE__COH_TRANS_LOSS:
            printf("Calculating coherent transmission loss.\n");
            fprintf(logFile, "Coherent transmission loss.\n");
            calcCohAcoustPress(settings);
            calcCohTransLoss(settings);
            break;
            
        case CALC_TYPE__PART_VEL:
            printf("Calculating particle velocity.\n");
            fprintf(logFile, "Particle velocity.\n");
            calcCohAcoustPress(settings);
            calcParticleVel(settings);
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
            printf("Calculating coherent acoustic pressure and particle velocity.\n");
            fprintf(logFile, "Coherent acoustic pressure and particle velocity.\n");
            calcCohAcoustPress(settings);
            calcParticleVel(settings);
            break;
            
        default:
            fatal("Unknown output option.\nAborting...");
            break;
    }

    //finish up the log:
    fprintf(logFile, "%s\n", line);
    fprintf(logFile, "Done.\n");
    
    printCpuTime(stdout);
    printCpuTime(logFile);

    //free memory
    freeSettings(settings);
    fclose(logFile);
    free(inFileName);
    free(logFileName);
    exit(EXIT_SUCCESS);
}
