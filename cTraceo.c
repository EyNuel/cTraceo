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
#include <sys/time.h>		//for struct timeval
#include <sys/resource.h>	//for getrusage()


int main(int argc, char **argv)
{
	const char*		infile = NULL;
	settings_t*		settings = mallocSettings();
	double			omega;
	const char*		logFilename = "flat.log";
	const char*		line = "-----------------------------------------------";
	FILE*			logFile = NULL;

	// check if a command line argument was passed:
	if(argc == 2){
		//if so, try to use it as an inout file
		infile = argv[1];
	}else{
		//otherwise, open the standard file.
		infile = "flat.in";
	}

	DEBUG(1,"Running cTraceo in verbose mode.\n\n");
	readIn(settings, infile);

//	if (VERBOSE)
//		printSettings(settings);

	omega	= 2 * M_PI * settings->source.freqx;

	//open the log file and write the header:
	logFile = openFile(logFilename, "w");
	fprintf(logFile, "TRACEO ray tracing program.\n");
	fprintf(logFile, "TODO: write a nice header for the log file.\n");
	fprintf(logFile, "%s\n", line);

	fprintf(logFile, "INPUT:\n");
	fprintf(logFile, "%s\n", settings->cTitle);
	fprintf(logFile, "%s\n", line);

	fprintf(logFile, "OUTPUT:\n");
	// if (catype.eq.'RCO'){
		fprintf(logFile, "Ray coordinates\n");
		calcRayCoords(settings);
/*
	 else if (catype.eq.'ARI') then
		 write(prtfil,*) 'Ray information'
		 call calari(ctitle,nthtas)

	 else if (catype.eq.'EPR') then
		 write(prtfil,*) 'Eigenrays (by PRoximity)'
		 call calepr(ctitle,nthtas)
	 
	 else if (catype.eq.'ERF') then
		 write(prtfil,*) 'Eigenrays (by Regula Falsi)'
		 call calerf(ctitle,nthtas)

	 else if (catype.eq.'ADP') then
		 write(prtfil,*) 'Amplitudes and Delays (by Proximity)'
		 call caladp(ctitle,nthtas)

	else if (catype.eq.'ADR') then
		 write(prtfil,*) 'Amplitudes and Delays (by Regula falsi)'
		 call caladr(ctitle,nthtas)

	 else if (catype.eq.'CPR') then
		write(prtfil,*) 'Coherent acoustic pressure'
		call calcpr(ctitle,nthtas)
		 
	 else if (catype.eq.'CTL') then
		 write(prtfil,*) 'Coherent transmission loss'
		 call calctl(ctitle,nthtas)

	else if (catype.eq.'PVL') then
		 write(prtfil,*) 'Particle velocity'
		 call calpvl(ctitle,nthtas)

	else if (catype.eq.'PAV') then
		 write(prtfil,*) 'Pressure and particle velocity'
		 call calpav(ctitle,nthtas)

	 else
		 write(prtfil,*) 'Unknown output option,'
		write(prtfil,*) 'aborting calculations...'
		 stop

	 end if
*/
	fprintf(logFile, "%s\n", line);
	fprintf(logFile, "Done.\n");
	printCpuTime(stdout);
	printCpuTime(logFile);
	fclose(logFile);
	exit(EXIT_SUCCESS);
}
