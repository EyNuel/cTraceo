//#pragma once
#include	"globals.h"
#include	"errorcodes.c"
#include	"errorcodes.h"
#include	<stdlib.h>
#include	<string.h>


/****************************
 *	Function prototypes		*
 ***************************/
FILE*		openFile(const char* , const char[4]);
char*		mallocChar(uint64_t);
double*		mallocDouble(uint64_t);
double**	mallocDouble2D(uint64_t, uint64_t);
settings_t*	mallocSettings(void);
globals_t*	mallocGlobals(void);
double 		readDouble(FILE*);
int64_t		readInt(FILE*);
char*		readString(FILE*);
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

char*		mallocChar(uint64_t numChars){
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

double*		mallocDouble(uint64_t numDoubles){
	/*
		Allocates an array of doubles and returns a pointer to it in case of success,
		Exits with error code otherwise.
	*/
	
	double*	temp = NULL;	//temporary pointer
	temp = malloc((unsigned long)numDoubles*sizeof(double));
	if (temp == NULL){
		printMsg(ERR__MEMORY_ALOCATION);
		exit(ERR__MEMORY_ALOCATION);
	}
	return temp;
}

double**	mallocDouble2D(uint64_t numRows, uint64_t numCols){
	/*
	 * Returns a pointer to an array of pointer do doubles.
	 * Or:
	 * Return a 2D Array.
	 */

	uint64_t	i;
	double**	array = NULL;
	array = malloc(numRows * sizeof(uint64_t*));	//malloc an array of pointers
	
	if(array == NULL)
		fatal("Memory allocation error.\n");

	for(i = 0; i < numRows; i++){
		array[i] = mallocDouble(numCols);	//Nota that mallocDouble() already checks for allocation errors
	}

	return array;
}

	//TODO can mallocSettings() be removed safely?
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

	globals->settings.cTitle = mallocChar(MAX_LINE_LEN + 1);
	globals->settings.source.thetas = NULL; //memory will be properly allocated in "readin.c"

	globals->settings.altimetry.r = NULL;
	globals->settings.altimetry.z = NULL;
	globals->settings.altimetry.surfaceProperties = NULL;

	globals->settings.bathymetry.r = NULL;
	globals->settings.bathymetry.z = NULL;
	globals->settings.bathymetry.surfaceProperties = NULL;

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

char*		readString(FILE* infile){
	/********************************************************************
	 *	Reads a line from a file, strips quotes and return it content.	*
	 *******************************************************************/
	 
	char*		inputString = NULL;
	char*		outputString = NULL;
	inputString = mallocChar(MAX_LINE_LEN + 1);

	//if we assume that the file is correctly formatted,
	//there will be a single quote at the beginning and at the end of a line.
	//So we copy the string for position 1 till 2 positions from end (thus stripping ' and \n ):
	fgets(inputString, MAX_LINE_LEN+1, infile);

	outputString = mallocChar(strlen(inputString)-2);
	strncpy(outputString, inputString + 1, strlen(inputString)-3);
	free(inputString);
	
	return(outputString);
}

void		skipLine(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	char*		junkString = NULL;
	junkString = mallocChar(MAX_LINE_LEN + 1);
	fgets(junkString, MAX_LINE_LEN+1, infile);

	free(junkString);
}

void		printSettings(globals_t*	globals){
	/************************************************
	 *	Outputs a settings structure to stdout.		*
	 ***********************************************/

		uint64_t	i;
	
	printf("settings.cTitle: \t%s", globals->settings.cTitle);	//assuming a \n at the end of cTitle
	printf("settings.source.ds: \t%12.5lf\t[m]\n", globals->settings.source.ds);
	printf("settings.source.rx: \t%12.5lf\t[m]\n", globals->settings.source.rx);
	printf("settings.source.zx: \t%12.5lf\t[m]\n", globals->settings.source.zx);
	printf("settings.source.rbox1: \t%12.5lf\t[m]\n", globals->settings.source.rbox1);
	printf("settings.source.rbox2: \t%12.5lf\t[m]\n", globals->settings.source.rbox2);
	printf("settings.source.freqx: \t%12.5lf\t[m]\n", globals->settings.source.freqx);
	printf("settings.source.nThetas: %6.0ld\n", globals->settings.source.nThetas);

	/* uncoment the following block to output all launching angles */
	/*
	for(i=0; i<globals->settings.source.nThetas; i++){
		printf("settings.source.thetas[%ld\t]: \t%lf\n", i, globals->settings.source.thetas[i]);
	}
	*/
	printf("settings.altimetry.surfaceType: \t");
	switch(globals->settings.altimetry.surfaceType){
		case SURFACE_TYPE__ABSORVENT:
			printf("Absorvent\n");
			break;
		case SURFACE_TYPE__ELASTIC:
			printf("Elastic\n");
			break;
		case SURFACE_TYPE__RIGID:
			printf("Rigid\n");
		case SURFACE_TYPE__VACUUM:
			printf("Vacuum\n");
			break;
	}

	printf("settings.altimetry.surfacePropertyType: ");
	switch(globals->settings.altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("Homogeneous\n");
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			printf("Non-Homogeneous\n");
			break;
	}

	printf("settings.altimetry.surfaceInterpolation:");
	switch(globals->settings.altimetry.surfaceInterpolation){
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

	printf("settings.altimetry.surfaceAttenUnits: \t");
	switch(globals->settings.altimetry.surfaceAttenUnits){
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

	printf("settings.altimetry.numSurfaceCoords:\t%ld\n", globals->settings.altimetry.numSurfaceCoords);

	printf("settings.altimetry.surfaceProperties:	");
	switch(globals->settings.altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("cp:%lf; ",	globals->settings.altimetry.surfaceProperties[0].cp);
			printf("cs:%lf; ",	globals->settings.altimetry.surfaceProperties[0].cs);
			printf("rho:%lf; ",	globals->settings.altimetry.surfaceProperties[0].rho);
			printf("ap:%lf; ",	globals->settings.altimetry.surfaceProperties[0].ap);
			printf("as:%lf;\n",	globals->settings.altimetry.surfaceProperties[0].as);
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			for(i=0; i<globals->settings.altimetry.numSurfaceCoords; i++){
				printf("cp:%lf; ",	globals->settings.altimetry.surfaceProperties[i].cp);
				printf("cs:%lf; ",	globals->settings.altimetry.surfaceProperties[i].cs);
				printf("rho:%lf; ",	globals->settings.altimetry.surfaceProperties[i].rho);
				printf("ap:%lf; ",	globals->settings.altimetry.surfaceProperties[i].ap);
				printf("as:%lf;\n",	globals->settings.altimetry.surfaceProperties[i].as);
			}
			break;
	}

	
}
