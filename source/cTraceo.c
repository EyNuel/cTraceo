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
#include "calcSSP.c"
#ifndef WINDOWS
    #include <sys/time.h>       //for struct timeval
    #include <sys/resource.h>   //for getrusage()
#endif
#include <string.h>
#include <stdbool.h>

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
"*          Copyright (C) 2011, 2012                                           *\n"
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
"*          $> ctraceo [options] <filename>                                    *\n" 
"*                                                                             *\n"
"*  Mandatory arguments:                                                       *\n"
"*          filename            :The input file's name, without it's extension.*\n"
"*                                                                             *\n"
"*  Options:                                                                   *\n"
"*          --nolog             :Do not write a log file.                      *\n"
"*          -h, --help          :show this text.                               *\n"
"*          -s <#>, --ssp <#>   :generate the interpolated sound speed profile *\n"
"*                               as used by the model and save it as 'ssp.mat'.*\n"
"*                               The number # is the number of SSP points to   *\n"
"*                               generate and should be a positive integer.    *\n"
"*          --stdin             :read input file from stdin instead of reading *\n"
"*                               it from disk.                                 *\n"
"*          -v                  :show version.                                 *\n"
"*                                                                             *\n"
"*  For more information check out the readme.txt, read the manual.pdf or      *\n"
"*  contact the authors.                                                       *\n"
"*                                                                             *\n"
"* =========================================================================== *\n\n");

    
}

int main(int argc, char **argv){
    char*           inFileName  = mallocChar(256);
    char*           logFileName = mallocChar(256);
    FILE*           inFile      = NULL;
    settings_t*     settings    = mallocSettings();
    const char*     line = "-----------------------------------------------";
    FILE*           logFile = NULL;
    bool            saveSSP = false;
    bool            writeLogFile = true;
    uint32_t        nSSPPoints = 0;

    DEBUG(1,"Running cTraceo in verbose mode.\n\n");
    
    // check if a command line argument was passed:
    if (argc == 1){
        //complain and quit
        printHelp();
        fatal("No input file provided.\nAborting...");
        exit(EXIT_SUCCESS);
    
    }else{
        //process command line options:
        for (int i = 1; i < argc; i++){
            if(argv[i][0] == '-'){
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
                else if(strlen(argv[i]) == 2){
                    switch(argv[i][1]){
                        // '-h' for help
                        case 'h':
                            printHelp();
                            exit(EXIT_SUCCESS);
                            break;
                        
                        // '-s' for ssp [save the interpolated soundSpeedProfile to ssp.mat]
                        // NOTE: same as long option '--ssp'
                        case 's':
                            //the next item from command line options should be the number of points used for generating the soundSpeedProfile (ssp.mat)
                            nSSPPoints = atoi(argv[++i]);
                            saveSSP = true;
                            break;
                            
                        
                        // '-v' for version
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
                else if (strlen(argv[i]) > 2){
                    if (!strcmp(argv[i], "--stdin")){
                        /*
                         * Read input file from stdin instead of from a file on disk.
                         * This avoids the overhead of writing to disk; intended for inversion uses.
                         * Same as short option "-"
                         * TODO: this needs to be documented (manual and --help)
                         */
                        inFile = stdin;
                    }
                    
                    //print help file
                    else if(!strcmp(argv[i], "--help")){
                        printHelp();
                        exit(EXIT_SUCCESS);
                    }
                    
                    // '--ssp' [save the interpolated soundSpeedProfile to ssp.mat]
                    // NOTE: same as short option '-s'
                    else if(!strcmp(argv[i], "--ssp")){
                        //the next item from command line options should be the number of points used for generating the soundSpeedProfile (ssp.mat)
                        nSSPPoints = atoi(argv[++i]);
                        saveSSP = true;
                    }
                    
                    // '--nolog' don't write a log file
                    else if(!strcmp(argv[i], "--nolog")){
                        writeLogFile = false;
                    }
                }
            
            }else{
                /*
                 * only the infile argument is supposed to be passed without a '-'
                 * and it's also supposed to be the last argument.
                 */
                
                //try to use last command line argument as an input file name
                strcpy(inFileName, argv[i]);
                inFileName = strcat(inFileName, ".in");
                inFile = openFile(inFileName, "r");
                break;  //leave the for loop (options after the input file's name will be ignored)
            }
        }//for loop
    }//if (argc == 1)

    printf("\n");
    printf(HEADER);

    //Read the input file
    readIn(settings, inFile);
    
    //if user requested storing the interpolated sound speed profile, do so now:
    if(saveSSP){
        calcSSP(settings, nSSPPoints);
    }
    
    if (VERBOSE){
        DEBUG(2, "Calling printSettings()\n");
        printSettings(settings);
        DEBUG(2, "Returned from printSettings()\n");
    }

    if(writeLogFile){
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
    }
    switch(settings->output.calcType){
        case CALC_TYPE__RAY_COORDS:
            printf("Calculating ray coordinates.\n");
            if(writeLogFile){
                fprintf(logFile, "Ray coordinates\n");
            }
            calcRayCoords(settings);
            break;

        case CALC_TYPE__ALL_RAY_INFO:
            printf("Calculating all ray information.\n");
            if(writeLogFile){
                fprintf(logFile, "All ray information\n");
            }
            calcAllRayInfo(settings);
            break;
            
        case CALC_TYPE__EIGENRAYS_PROXIMITY:
            printf("Calculating eigenrays by Proximity Method.\n");
            if(writeLogFile){
                fprintf(logFile, "Eigenrays by Proximity Method.\n");
            }
            calcEigenrayPr(settings);
            break;
            
        case CALC_TYPE__EIGENRAYS_REG_FALSI:
            printf("Calculating eigenrays by Regula Falsi Method.\n");
            if(writeLogFile){
                fprintf(logFile, "Eigenrays by Regula Falsi Method.\n");
            }
            calcEigenrayRF(settings);
            break;
            
        case CALC_TYPE__AMP_DELAY_PROXIMITY:
            printf("Calculating amplitudes and delays by Proximity Method.\n");
            if(writeLogFile){
                fprintf(logFile, "Amplitudes and delays by Proximity Method.\n");
            }
            calcAmpDelPr(settings);
            break;
            
        case CALC_TYPE__AMP_DELAY_REG_FALSI:
            printf("Calculating amplitudes and delays by Regula Falsi Method.\n");
            if(writeLogFile){
                fprintf(logFile, "Amplitudes and delays by Regula Falsi Method.\n");
            }
            calcAmpDelRF(settings);
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS:
            printf("Calculating coherent acoustic pressure.\n");
            if(writeLogFile){
                fprintf(logFile, "Coherent acoustic pressure.\n");
            }
            calcCohAcoustPress(settings);
            break;
            
        case CALC_TYPE__COH_TRANS_LOSS:
            printf("Calculating coherent transmission loss.\n");
            if(writeLogFile){
                fprintf(logFile, "Coherent transmission loss.\n");
            }
            calcCohAcoustPress(settings);
            calcCohTransLoss(settings);
            break;
            
        case CALC_TYPE__PART_VEL:
            printf("Calculating particle velocity.\n");
            if(writeLogFile){
                fprintf(logFile, "Particle velocity.\n");
            }
            calcCohAcoustPress(settings);
            calcParticleVel(settings);
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
            printf("Calculating coherent acoustic pressure and particle velocity.\n");
            if(writeLogFile){
                fprintf(logFile, "Coherent acoustic pressure and particle velocity.\n");
            }
            calcCohAcoustPress(settings);
            calcParticleVel(settings);
            break;
            
        default:
            fatal("Unknown output option.\nAborting...");
            break;
    }
    
    if(writeLogFile){
        //finish up the log:
        fprintf(logFile, "%s\n", line);
        fprintf(logFile, "Done.\n");
    }
    
    printCpuTime(stdout);
    if(writeLogFile){
        printCpuTime(logFile);
    }
    
    //free memory
    freeSettings(settings);
    if(writeLogFile){
        fclose(logFile);
        free(logFileName);
    }
    free(inFileName);
    exit(EXIT_SUCCESS);
}
