/****************************************************************************************
 *  cTraceo.c                                                                           *
 *  (formerly "trace0.for")                                                             *
 *  The main cTraceo file. This is where the story begins.                              *
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
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "logOptions.c"
#if USE_MATLAB == 1
    #include <mat.h>
    #include "matrix.h"
#else
    #include    "matOut/matOut.h"
#endif

void    printHelp(void);
int     main(int, char**);

void    printHelp(void){
    /*
     * Print usage help
     */
    printf("\n"
"* =========================================================================== *\n"
"*          The cTraceo Acoustic Raytracing Model, Version "VERSION"*\n"                //NOTE: version string is defined in globals.h
"*                                                                             *\n"
"* --------------------------------------------------------------------------- *\n"
"* Website:                                                                    *\n"
"*          https://github.com/EyNuel/cTraceo/wiki                             *\n"
"*                                                                             *\n"
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
"* ----------------------------------------------------------------------------*\n");
printf(""
"*  Usage:                                                                     *\n"
"*          $> ctraceo [options] <filename>                                    *\n" 
"*                                                                             *\n"
"*  Mandatory arguments:                                                       *\n"
"*          filename            The input file's name, without it's extension. *\n"
"*                                                                             *\n"
"*  Options:                                                                   *\n"
"*          --noLog             Do not write a log file.                       *\n"
"*                                                                             *\n"
"*          -h, --help          Show this text.                                *\n"
"*                                                                             *\n"
"*          -s <#>, --ssp <#>   Generate the interpolated sound speed profile  *\n"
"*                              as used by the model and save it as 'ssp.mat'. *\n"
"*                              The number # is the number of SSP points to    *\n"
"*                              generate and should be a positive integer.     *\n"
"*                                                                             *\n"
"*          --stdin             Read input file from stdin instead of reading  *\n"
"*                              it from disk.                                  *\n"
"*                                                                             *\n"
"*          -v, --version       Show version.                                  *\n"
"*                                                                             *\n"
"*          --killBackscatteredRays                                            *\n"
"*                              terminates a ray's propagation as soon as it   *\n"
"*                              inverts it's  horizontal travel direction.     *\n"
"*                              The number of affected rays will be stored in  *\n" 
"*                              the resulting matfile as 'nBackscatteredRays'. *\n"
"*                              May significantly improve performance.         *\n"
"*                                                                             *\n"
"*          --noHeader          Disables the output of cTraceo's header. Can   *\n"
"*                              be used to keep logs smaller.                  *\n"
"*                                                                             *\n"
"*          --outputFileName <name>                                            *\n"
"*                              Specify a different file name for the output   *\n"
"*                              Generated by cTraceo.                          *\n"
"*                                                                             *\n"
"*  Note:   cTraceo's command line options are not case sensitive, i.e.,       *\n"
"*          passing '--noLog' or '--nolog' will have the same effect.          *\n"
"*                                                                             *\n"
"*  For more information visit our wiki, check out the readme.txt, read the    *\n"
"*  manual.pdf or contact the authors.                                         *\n"
"*                                                                             *\n"
"* =========================================================================== *\n\n");

    
}

int main(int argc, char **argv){
    float           tEnd, tInit   = (double)clock()/CLOCKS_PER_SEC;   //get time
    settings_t*     settings    = mallocSettings();
    const char*     line = "-----------------------------------------------";

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
                     settings->options.inFile = stdin;
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
                            settings->options.nSSPPoints = atoi(argv[++i]);
                            settings->options.saveSSP = true;
                            break;
                            
                        
                        // '-v' for version (same as '--version')
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
                    if (!strcmp(stringToLower(argv[i]), "--stdin")){
                        /*
                         * Read input file from stdin instead of from a file on disk.
                         * This avoids the overhead of writing to disk; intended for inversion uses.
                         * Same as short option "-"
                         * TODO: this needs to be documented (manual and --help)
                         */
                        settings->options.inFile = stdin;
                        LOG("Option '--stdin' enabled; reading input file from stdin.\n");
                    }
                    
                    //print help file
                    else if(!strcmp(stringToLower(argv[i]), "--help")){
                        printHelp();
                        exit(EXIT_SUCCESS);
                    }
                    
                    // '--ssp' [save the interpolated soundSpeedProfile to ssp.mat]
                    // NOTE: same as short option '-s'
                    else if(!strcmp(stringToLower(argv[i]), "--ssp")){
                        //the next item from command line options should be the number of points used for generating the soundSpeedProfile (ssp.mat)
                        settings->options.nSSPPoints = atoi(argv[++i]);
                        settings->options.saveSSP = true;
                    }
                    
                    // '--nolog' don't write a log file
                    else if(!strcmp(stringToLower(argv[i]), "--nolog")){
                        settings->options.writeLogFile = false;
                    }
                    
                    // '--version' print out version string (same as -v)
                    else if(!strcmp(stringToLower(argv[i]), "--version")){
                        printf(HEADER);
                        exit(EXIT_SUCCESS);
                    }
                    
                    // '--killBackstatteredRays'
                    else if(!strcmp(stringToLower(argv[i]), "--killbackscatteredrays")){
                        settings->options.killBackscatteredRays = true;
                    }
                    
                    // '--noHeader'
                    else if(!strcmp(stringToLower(argv[i]), "--noheader")){
                        settings->options.writeHeader = false;
                    }
                    
                    // '--outputFileName'  
                    else if(!strcmp(stringToLower(argv[i]), "--outputfilename")){
                        i += 1;     //next argument should contain output file name.
                        settings->options.outputFileName = mallocChar(strlen(argv[i]+1));
                        strcpy( settings->options.outputFileName, argv[i]);
                    }
                    
                    // unknown options:
                    else{
                        printf("Ignoring unknown option %s.\n", argv[i]);
                    }
                }
            
            }else{
                /*
                 * only the infile argument is supposed to be passed without a '-'
                 * and it's also supposed to be the last argument.
                 */
                
                //try to use last command line argument as an input file name
                strcpy(settings->options.inFileName, argv[i]);
                settings->options.inFileName = strcat(  settings->options.inFileName, ".in");
                settings->options.inFile     = openFile(settings->options.inFileName, "r");
                strcpy(settings->options.logFileName, argv[i]);
                break;  //leave the for loop (options after the input file's name will be ignored)
            }
        }//for loop
    }//if (argc == 1)

    printf("\n");
    if (settings->options.writeHeader){
        printf(HEADER);
    }
    
    //Read the input file
    readIn(settings);
    
    //if user requested storing the interpolated sound speed profile, do so now:
    if(settings->options.saveSSP){
        calcSSP(settings);
    }
    
    if (VERBOSE){
        DEBUG(2, "Calling printSettings()\n");
        printSettings(settings);
        DEBUG(2, "Returned from printSettings()\n");
    }

    if(settings->options.writeLogFile){
        //open the log file and write the header:
        settings->options.logFile= openFile(strcat(settings->options.logFileName,".log"), "w");
        LOG(HEADER);
        LOG("\n");
        
        LOG("Input file: %s\n", settings->options.inFileName);
        LOG("Title: %s\n", settings->cTitle);
        LOG("%s\n", line);
        
        logOptions(settings);
        LOG("%s\n", line);
    }
    
    //run the computation
    switch(settings->output.calcType){
        case CALC_TYPE__RAY_COORDS:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "rco.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating ray coordinates [RCO].\n");
            LOG(    "Calculating ray coordinates [RCO]\n");
            
            calcRayCoords(settings);
            break;

        case CALC_TYPE__ALL_RAY_INFO:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "ari.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating all ray information [ARI].\n");
            LOG(    "Calculating all ray information [ARI]\n");
            
            calcAllRayInfo(settings);
            break;
            
        case CALC_TYPE__EIGENRAYS_PROXIMITY:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "eig.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating eigenrays by proximity method [EPR].\n");
            LOG(    "Calculating eigenrays by proximity method [EPR].\n");
            
            calcEigenrayPr(settings);
            break;
            
        case CALC_TYPE__EIGENRAYS_REG_FALSI:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "eig.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating eigenrays by Regula Falsi Method [ERF].\n");
            LOG(    "Calculating  eigenrays by Regula Falsi Method [ERF].\n");
            
            calcEigenrayRF(settings);
            break;
            
        case CALC_TYPE__AMP_DELAY_PROXIMITY:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "aad.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating amplitudes and delays by Proximity Method [ADP].\n");
            LOG(    "Calculating amplitudes and delays by Proximity Method [ADP].\n");
            
            calcAmpDelPr(settings);
            break;
            
        case CALC_TYPE__AMP_DELAY_REG_FALSI:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "aad.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating amplitudes and delays by Regula Falsi Method [ADR].\n");
            LOG(    "Calculating amplitudes and delays by Regula Falsi Method [ADR].\n");
            
            calcAmpDelRF(settings);
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "cpr.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating coherent acoustic pressure [CPR].\n");
            LOG(    "Calculating coherent acoustic pressure [CPR].\n");
            
            calcCohAcoustPress(settings);
            break;
            
        case CALC_TYPE__COH_TRANS_LOSS:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "ctl.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating coherent transmission loss [CTL].\n");
            LOG(    "Calculating coherent transmission loss [CTL].\n");
            
            calcCohAcoustPress(settings);
            calcCohTransLoss(settings);
            break;
            
        case CALC_TYPE__PART_VEL:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "pvl.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating particle velocity [PVL].\n");
            LOG(    "Calculating particle velocity [PVL].\n");
            
            calcCohAcoustPress(settings);
            calcParticleVel(settings);
            break;
            
        case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
            if(settings->options.outputFileName == NULL){
                settings->options.outputFileName = mallocChar(8);
                strcpy( settings->options.outputFileName, "pav.mat");
            }
            LOG(    "Output file name: %s\n", settings->options.outputFileName);
            printf( "Calculating coherent acoustic pressure and particle velocity [PAV].\n");
            LOG(    "Calculating coherent acoustic pressure and particle velocity [PAV].\n");
            
            calcCohAcoustPress(settings);
            calcParticleVel(settings);
            break;
            
        default:
            fatal("Unknown output option.\nAborting...");
            break;
    }
    
    #if 0
    /*
     * TODO:
     * this block results in corrupted matlab files when calculating CTL.
     * This may be resolved be moving the matlab opening from the individual calcXX functions up one level to this file.
     * See issues #
     */
    //write number of truncated rays to log and matfile:
    if (settings->options.killBackscatteredRays){
        LOG("Truncated %u backscattered rays.\n", settings->options.nBackscatteredRays);
        
        MATFile*        matfile                 = NULL;
        mxArray*        mxNBackscatteredRays    = NULL;
        //~ double          nBackscatteredRays_d    = 10;//(double)settings->options.nBackscatteredRays;
        
        DEBUG(0, "nBackscatteredRays_d: %lf\n", settings->options.nBackscatteredRays);
        DEBUG(0, "output file name: %s\n", settings->options.outputFileName);
        
        //open matfile for output
        //~ matfile     = matOpen("ctl.mat", "u");
        matfile = matOpen(settings->options.outputFileName, "u");
        
        //write launching angles to file
        mxNBackscatteredRays = mxCreateDoubleMatrix((MWSIZE)1, (MWSIZE)1, mxREAL);
        
        //copy cArray to mxArray:
        copyUInt32ToMxArray(&settings->options.nBackscatteredRays, mxNBackscatteredRays, 1);
        
        //move mxArray to file:
        matPutVariable(matfile, "nBackscatteredRays", mxNBackscatteredRays);
        mxDestroyArray(mxNBackscatteredRays);
        matClose(matfile);
    }
    #endif
    
    //finish up the log:
    LOG("%s\n", line);
    LOG("Done.\n");

    //get elapsed time:
    tEnd = (double)clock()/CLOCKS_PER_SEC;    
    fprintf(stderr,     "---------\n%f seconds total.\n", tEnd-tInit);
    LOG("---------\n%f seconds total.\n", tEnd-tInit);
    
    //free memory
    freeSettings(settings);
    if(settings->options.writeLogFile){
        fclose( settings->options.logFile);
        free(   settings->options.logFileName);
    }
    free(settings->options.inFileName);
    exit(EXIT_SUCCESS);
}
