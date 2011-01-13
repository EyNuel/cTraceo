//#pragma once
#include	"globals.h"
#include	"errorcodes.c"
#include	"errorcodes.h"
#include	<stdlib.h>



/****************************
 *	Function prototypes		*
 ***************************/
FILE*		openFile(const char* , const char[4]);
char*		mallocChar(int);
settings_t*	mallocSettings(void);
globals_t*	mallocGlobals(void);
double 		readDouble(FILE*);
int64_t		readInt(FILE*);
void		skipLine(FILE*);
void		printSettings(globals_t*);


/****************************
 *	Actual Functions		*
 ***************************/
FILE* 		openFile(const char *filename, const char mode[4]) {
	/* 
		Opens a file and returns a filepointer in case of success, exits with error code otherwise.
		Input values:
			filename	A string containing a full or relative path to the file.
			mode		A string containg the file acces mode.
		
		Return Value:
			A FILE pointer.
	*/
	
	FILE *temp;
	if (VERBOSE)
		printf("Accessing file: %s... ", filename);
		
	temp=fopen(filename, mode);
	if(temp==NULL) {
		printMsg(ERR__FILE_OPEN);
		exit(ERR__FILE_OPEN);
	} else {
		if (VERBOSE)
			printf("Ok.\n");
		return temp;
	}
}

char*		mallocChar(int numChars){
	/*
		Allocates a char string and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	
	char*	temp = NULL;	//temporary pointer
	temp = malloc((unsigned long)numChars*sizeof(char));
	if (temp == NULL){
		printMsg(ERR__MEMORY_ALOCATION);
		exit(ERR__MEMORY_ALOCATION);
	}
	return temp;
}
		
settings_t*	mallocSettings(void){
	/*
		Allocate memory for a settings structure.
		Return pointer in case o success, exit with error code otherwise.
	*/
	
	settings_t*	settings = NULL;
	settings = malloc(sizeof(settings_t));
	if (settings == NULL){
		printMsg(ERR__MEMORY_ALOCATION);
		exit(ERR__MEMORY_ALOCATION);
	}
	settings->cTitle = NULL;
	settings->cTitle = malloc( (MAX_LINE_LEN + 1) * sizeof(char));
	if (settings->cTitle == NULL){
		printMsg(ERR__MEMORY_ALOCATION);
		exit(ERR__MEMORY_ALOCATION);
	}
	return settings;
}

globals_t* 	mallocGlobals(void){
	/*
		Allocate memory for a globals structure.
		Return pointer in case o success, exit with error code otherwise.
	*/
	globals_t*	globals = NULL;
	globals = malloc(sizeof(globals_t));
	if(globals == NULL){
		printMsg(ERR__MEMORY_ALOCATION);
		exit(ERR__MEMORY_ALOCATION);
	}

	//globals->settings = mallocSettings();
	globals->settings.cTitle = mallocChar(MAX_LINE_LEN + 1);

	return(globals);
}

double 		readDouble(FILE* infile){
	/************************************************
	 *	Reads a double from a file and returns it	*
	 ***********************************************/
	 
	char*	junkString = NULL;
	double 	tempDouble;
	junkString = mallocChar(MAX_LINE_LEN + 1);
	
	fscanf(infile, "%s\n", junkString);
	tempDouble = atof(junkString);
	free(junkString);
	
	return(tempDouble);
}

int64_t		readInt(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	 
	char*		junkString = NULL;
	int64_t 	tempInt;
	junkString = mallocChar(MAX_LINE_LEN + 1);
	
	fscanf(infile, "%s\n", junkString);
	tempInt = atol(junkString);
	free(junkString);
	
	return(tempInt);
}

void		skipLine(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	char*		junkString = NULL;
	//int64_t 	tempInt;
	junkString = mallocChar(MAX_LINE_LEN + 1);

	fgets(junkString, MAX_LINE_LEN+1, infile);
	//fscanf(infile, "%s\n", junkString);
	//tempInt = atol(junkString);
	free(junkString);
}

void		printSettings(globals_t*	globals){
	/************************************************
	 *	Outputs a settings structure to stdout.		*
	 ***********************************************/
	 printf("settings.cTitle: \t%s", globals->settings.cTitle);	//assuming a \n at the end of cTitle
	 printf("settings.source.ds: \t%lf\n", globals->settings.source.ds);
	 printf("settings.source.rx: \t%lf\n", globals->settings.source.rx);
	 printf("settings.source.zx: \t%lf\n", globals->settings.source.zx);
	 printf("settings.source.rbox1: \t%lf\n", globals->settings.source.rbox1);
	 printf("settings.source.rbox2: \t%lf\n", globals->settings.source.rbox2);
	 printf("settings.source.freqx: \t%lf\n", globals->settings.source.freqx);
	 printf("settings.source.nThetas: \t%ld\n", globals->settings.source.nThetas);
}
