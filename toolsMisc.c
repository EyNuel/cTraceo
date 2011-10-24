/********************************************************************************
 *  toolsMisc.c																		*
 * 	Collection of miscelaneous utility functions.								*
 *																				*
 *	Written by:		Emanuel Ey													*
 *					emanuel.ey@gmail.com										*
 *					Signal Processing Laboratory								*
 *					Universidade do Algarve										*
 *																				*
 *******************************************************************************/

#pragma once
#include	<string.h>
#include	<sys/time.h>		//for struct time_t
#include	<sys/resource.h>	//for getrusage()
#include	"globals.h"
#include	<stdbool.h>
#include	<stdlib.h>


///Prototypes:

uint32_t	isnan_d(double);
double		min(double, double);
double		max(double, double);
void 		fatal(const char*);
void		printCpuTime(FILE*);


///Functions:

uint32_t isnan_d(double x){
	//Note that isnan() is only defined for the float data type, and not for doubles
	//NANs are never equal to anything -even themselves:
	if (x!=x){
		return true;
	}else{
		return false;
	}
}

double		min(double a, double b){
	if( a <= b){
		return a;
	}else{
		return b;
	}
}

double		max(double a, double b){
	if( a > b){
		return a;
	}else{
		return b;
	}
}

void 		fatal(const char* message){
	/*
		Prints a message and exits terminates the program.
		Closes all open i/o streams befre exiting.
	*/
	printf("%s\n", message);
	fflush(NULL);				//flushes all i/o streams.
	exit(EXIT_FAILURE);
}

void		printCpuTime(FILE* stream){
	/*
	 * prints total cpu time used by process.
	 */
	struct rusage	usage;

	getrusage(RUSAGE_SELF, &usage);
	fprintf(stream, "%ld.%06ld seconds user CPU time,\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
	fprintf(stream, "%ld.%06ld seconds system CPU time used.\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
}
