/************************************************************************************
 *	cTraceo.c		 																*
 * 	(formerly "trace0.for")															*
 * 	The main cTraceo file. This is where the story begins.							*
 * 																					*
 *	originally written in FORTRAN by:												*
 *  						Orlando Camargo Rodriguez:								*
 *							Copyright (C) 2010										*
 * 							Orlando Camargo Rodriguez								*
 *							orodrig@ualg.pt											*
 *							Universidade do Algarve									*
 *							Physics Department										*
 *							Signal Processing Laboratory							*
 *																					*
 *	Ported to C by:			Emanuel Ey												*
 *							emanuel.ey@gmail.com									*
 *							Signal Processing Laboratory							*
 *							Universidade do Algarve									*
 *																					*
 *	Inputs:																			*
 * 				None																*
 * 	Outputs:																		*
 * 				None:																*
 ***********************************************************************************/

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
#include <sys/time.h>		//for struct timeval
#include <sys/resource.h>	//for getrusage()
#include <string.h>

void	printUsage(void);
int 	main(int, char**);

void	printUsage(void){
	printf("Line 1\n"
	"Line 2\n"
	"Line %d\n",
	3);
	
}

int main(int argc, char **argv){
	char*		inFileName = mallocChar(256);
	char*		logFileName = mallocChar(256);
	settings_t*		settings = mallocSettings();
	double			omega;
	const char*		line = "-----------------------------------------------";
	FILE*			logFile = NULL;

	DEBUG(1,"Running cTraceo in verbose mode.\n\n");
	
	// check if a command line argument was passed:
	if(argc == 2){
		//if so, try to use it as an inout file
		strcpy(inFileName, argv[1]);
		inFileName = strcat(inFileName, ".in");
	}else{
		//otherwise, complain and quit
		printUsage();
		fatal("No input file provided.\nAborting...");
	}

	//Read the input file
	readIn(settings, inFileName);

	/**
	if (VERBOSE)
		printSettings(settings);
	*/
	omega	= 2 * M_PI * settings->source.freqx;

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
			fatal("WIP");
			break;
			
		case CALC_TYPE__COH_TRANS_LOSS:
			printf("Calculating coherent transmission loss.\n");
			fprintf(logFile, "Coherent transmission loss.\n");
			fatal("WIP");
			break;
			
		case CALC_TYPE__PART_VEL:
			printf("Calculating particle velocity.\n");
			fprintf(logFile, "Particle velocity.\n");
			fatal("WIP");
			break;
			
		case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
			printf("Calculating coherent acoustic pressure and particle velocity.\n");
			fprintf(logFile, "Coherent acoustic pressure and particle velocity.\n");
			fatal("WIP");
			break;
			
		default:
			fatal("Unknown output option.\nAborting...");
			break;
	}

	fprintf(logFile, "%s\n", line);
	fprintf(logFile, "Done.\n");
	
	printCpuTime(stdout);
	printCpuTime(logFile);

	//free memory
	fclose(logFile);
	free(inFileName);
	free(logFileName);
	exit(EXIT_SUCCESS);
}
