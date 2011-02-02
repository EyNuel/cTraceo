/********************************************************************************
 *  tools.c																		*
 * 	Collection of utility functions.											*
 *	All memory allocation functions in this file check for allocation errors.	*
 *																				*
 *	Written by:		Emanuel Ey													*
 *					emanuel.ey@gmail.com										*
 *					Signal Processing Laboratory								*
 *					Universidade do Algarve										*
 *																				*
 *******************************************************************************/

#pragma once
#include	"globals.h"
#include	<stdlib.h>
#include	<string.h>


/****************************
 *	Function prototypes		*
 ***************************/
double*			subVector(double*, uintptr_t);
double			min(double, double);
double			max(double, double);
void 			fatal(const char*);
FILE*			openFile(const char* , const char[4]);
char*			mallocChar(uintptr_t);
uint32_t*		mallocUint(uintptr_t);
uint32_t*		reallocUint(uint32_t*, uintptr_t);
double*			mallocDouble(uintptr_t);
double*			reallocDouble(double*, uintptr_t);
double**		mallocDouble2D(uintptr_t, uintptr_t);
complex double*	mallocComplex(uintptr_t);
complex double*	reallocComplex(complex double*, uintptr_t);
globals_t*		mallocGlobals(void);
vector_t*		mallocVector(uintptr_t);
vector_t*		reallocVector(vector_t*, uintptr_t);
point_t*		mallocPoint(uintptr_t);
point_t*		reallocPoint(point_t*, uintptr_t);
double 			readDouble(FILE*);
int32_t			readInt(FILE*);
char*			readStringN(FILE*, uint32_t);
void			skipLine(FILE*);
void			printSettings(globals_t*);
void			reallocRay(ray_t*, uintptr_t);
void			copyDoubleToPtr(double*, double*, uintptr_t);
void			copyDoubleToPtr2D(double**, double*, uintptr_t, uintptr_t);

/****************************
 *	Actual Functions		*
 ***************************/

double*	subVector(double* vector, uintptr_t iniPos){
	/*
	 * Returns the address of a subposition of a vector.
	 * Bounds are left unchecked, so be carefull or segfault will occur.
	 * TODO: this function is rather redundant, but it may make code more readable (?)
	 */
	return( &vector[iniPos]);
}

double	min(double a, double b){
	if( a <= b){
		return a;
	}else{
		return b;
	}
}

double max(double a, double b){
	if( a > b){
		return a;
	}else{
		return b;
	}
}

void fatal(const char* message){
	/*
		Prints a message and exits terminates the program.
		Closes all open i/o streams befre exiting.
	*/
	printf("%s\n", message);
	fflush(NULL);				//flushes all i/o streams.
	exit(EXIT_FAILURE);
}

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
		fatal("Error while opening file.\n");
		exit(EXIT_FAILURE);		//this is redundant but avoids "control may reach end of non-void function" warning
	} else {
		if (VERBOSE)
			printf("Ok.\n");
		return temp;
	}
}

char*		mallocChar(uintptr_t numChars){
	/*
		Allocates a char string and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	
	char*	temp = NULL;	//temporary pointer
	temp = malloc((unsigned long)numChars*sizeof(char));
	if (temp == NULL){
		fatal("Memory allocation error.\n");
	}
	return temp;
}

uint32_t*	mallocUint(uintptr_t numUints){
	/*
		Allocates a char string and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	uint32_t*	temp = NULL;	//temporary pointer

	temp = malloc((unsigned long)numUints*sizeof(uint32_t));
	return reallocUint(temp, numUints);
}

uint32_t*	reallocUint(uint32_t* temp, uintptr_t numUints){
	/*
		Allocates a char string and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	temp = realloc(temp, numUints*sizeof(uint32_t));
	if (temp == NULL){
		fatal("Memory allocation error.\n");
	}
	return temp;
}

double*		mallocDouble(uintptr_t numDoubles){
	/*
		Allocates an array of doubles and returns a pointer to it in case of success,
		Exits with error code otherwise.
	*/
	
	double*	temp = NULL;	//temporary pointer
	return	reallocDouble(temp, numDoubles);
}

double*		reallocDouble(double* temp, uintptr_t numDoubles){
	/*
		Reallocates a pointer to doubles and returns its pointer in case of success,
		exits with error code otherwise.
	*/
	temp = realloc(temp, numDoubles*sizeof(double));
	if (temp == NULL){
		fatal("Memory allocation error.\n");
	}
	return temp;
}

double**	mallocDouble2D(uintptr_t numRows, uintptr_t numCols){
	/*
	 * Returns a pointer to an array of pointer do doubles.
	 * Or:
	 * Return a 2D Array.
	 */

	uint32_t	i;
	double**	array = NULL;
	array = malloc(numRows * sizeof(uintptr_t*));	//malloc an array of pointers
	
	if(array == NULL)
		fatal("Memory allocation error.\n");

	for(i = 0; i < numRows; i++){
		array[i] = mallocDouble(numCols);	//Nota that mallocDouble() already checks for allocation errors
	}

	return array;
}

complex double*		mallocComplex(uintptr_t numComplex){
	/*
		Allocates an array of doubles and returns a pointer to it in case of success,
		Exits with error code otherwise.
	*/
	
	complex double*	temp = NULL;	//temporary pointer
	return reallocComplex(temp, numComplex);
}

complex double*		reallocComplex(complex double* temp, uintptr_t numComplex){
	/*
		Reallocates an array of doubles and returns a pointer to it in case of success,
		Exits with error code otherwise.
	*/
	temp = realloc(temp, numComplex*sizeof(complex double));
	if (temp == NULL){
		fatal("Memory allocation error.\n");
	}
	return temp;
}

globals_t* 	mallocGlobals(void){
	/*
		Allocate memory for a globals structure.
		Return pointer in case o success, exit with error code otherwise.
	*/
	globals_t*	globals = NULL;
	globals = malloc(sizeof(globals_t));
	if(globals == NULL){
		fatal("Memory allocation error.\n");
	}

	globals->settings.cTitle = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	globals->settings.source.thetas = NULL; //memory will be properly allocated in "readin.c"

	globals->settings.altimetry.r = NULL;
	globals->settings.altimetry.z = NULL;
	globals->settings.altimetry.surfaceProperties = NULL;

	globals->settings.batimetry.r = NULL;
	globals->settings.batimetry.z = NULL;
	globals->settings.batimetry.surfaceProperties = NULL;

	globals->settings.output.arrayR = NULL;
	globals->settings.output.arrayZ = NULL;

	return(globals);
}

vector_t*	mallocVector(uintptr_t	numVectors){
	vector_t*	temp = NULL;
	return reallocVector(temp, numVectors);
}

vector_t*	reallocVector(vector_t* temp, uintptr_t	numVectors){
	temp = realloc(temp, numVectors * sizeof(vector_t));
	if (temp == NULL){
		fatal("Memory allocation error.\n");
	}
	return temp;
}

point_t*	mallocPoint(uintptr_t	numPoints){
	point_t*	temp = NULL;
	return realloc(temp, numPoints);
}

point_t*	reallocPoint(point_t* temp, uintptr_t	numPoints){
	temp = realloc(temp, numPoints * sizeof(point_t));
	if (temp == NULL){
		fatal("Memory allocation error.\n");
	}
	return temp;
}

double 		readDouble(FILE* infile){
	/************************************************
	 *	Reads a double from a file and returns it	*
	 ***********************************************/
	 
	char*	junkString = NULL;
	double 	tempDouble;
	junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	
	fscanf(infile, "%s\n", junkString);
	tempDouble = atof(junkString);
	free(junkString);
	
	return(tempDouble);
}

int32_t		readInt(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	 
	char*		junkString = NULL;
	int32_t 	tempInt;
	junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	
	fscanf(infile, "%s\n", junkString);
	tempInt = (int32_t)atol(junkString);
	free(junkString);
	
	return(tempInt);
}

char*		readStringN(FILE* infile, uint32_t length){
	/********************************************************************
	 *	Reads a <lenght> chars from a filestream.						*
	 *******************************************************************/
	//TODO: replace readString() by readStringN()
	char*		outputString = NULL;
	outputString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));

	fgets(outputString, (int32_t)length, infile);
	return(outputString);
}

void		skipLine(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	char*		junkString = NULL;
	junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	fgets(junkString, MAX_LINE_LEN+1, infile);

	free(junkString);
}

void		printSettings(globals_t*	globals){
	/************************************************
	 *	Outputs a settings structure to stdout.		*
	 ***********************************************/

		uint32_t	i;
	
	printf("cTitle: \t\t\t%s", globals->settings.cTitle);	//assuming a \n at the end of cTitle
	printf("source.ds: \t\t\t%12.5lf\t[m]\n", globals->settings.source.ds);
	printf("source.rx: \t\t\t%12.5lf\t[m]\n", globals->settings.source.rx);
	printf("source.zx: \t\t\t%12.5lf\t[m]\n", globals->settings.source.zx);
	printf("source.rbox1: \t\t\t%12.5lf\t[m]\n", globals->settings.source.rbox1);
	printf("source.rbox2: \t\t\t%12.5lf\t[m]\n", globals->settings.source.rbox2);
	printf("source.freqx: \t\t\t%12.5lf\t[m]\n", globals->settings.source.freqx);
	printf("source.nThetas: \t\t%6.0lu\n", (long unsigned int)globals->settings.source.nThetas);

	/* uncoment the following block to output all launching angles */
	/*
	for(i=0; i<globals->settings.source.nThetas; i++){
		printf("source.thetas[%ld\t]: \t%lf\n", i, globals->settings.source.thetas[i]);
	}
	*/
	printf("altimetry.surfaceType: \t\t");
	switch(globals->settings.altimetry.surfaceType){
		case SURFACE_TYPE__ABSORVENT:
			printf("Absorvent\n");
			break;
		case SURFACE_TYPE__ELASTIC:
			printf("Elastic\n");
			break;
		case SURFACE_TYPE__RIGID:
			printf("Rigid\n");
			break;
		case SURFACE_TYPE__VACUUM:
			printf("Vacuum\n");
			break;
	}

	printf("altimetry.surfacePropertyType:\t");
	switch(globals->settings.altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("Homogeneous\n");
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			printf("Non-Homogeneous\n");
			break;
	}

	printf("altimetry.surfaceInterpolation: ");
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

	printf("altimetry.surfaceAttenUnits: \t");
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

	printf("altimetry.numSurfaceCoords:\t%lu\n", (long unsigned int)globals->settings.altimetry.numSurfaceCoords);

	printf("altimetry.surfaceProperties:	");
	switch(globals->settings.altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("cp:%lf\n ",	globals->settings.altimetry.surfaceProperties[0].cp);
			printf("\t\t\t\tcs:%lf\n ",	globals->settings.altimetry.surfaceProperties[0].cs);
			printf("\t\t\t\trho:%lf\n ",	globals->settings.altimetry.surfaceProperties[0].rho);
			printf("\t\t\t\tap:%lf\n ",	globals->settings.altimetry.surfaceProperties[0].ap);
			printf("\t\t\t\tas:%lf\n",	globals->settings.altimetry.surfaceProperties[0].as);
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
	printf("altimetry.r[0]:\t\t\t%lf\n", globals->settings.altimetry.r[0]);
	printf("altimetry.r[N]:\t\t\t%lf\n", globals->settings.altimetry.r[globals->settings.altimetry.numSurfaceCoords-1]);
	

	/* sound speed block */
	
	/*	object block	*/
	printf("objects.numObjects:\t\t%u\n",globals->settings.objects.numObjects);
	if(globals->settings.objects.numObjects > 0){
		for(i=0; i<globals->settings.objects.numObjects; i++){
			printf("object[%u].surfaceType: \t", i);
			switch(globals->settings.objects.object[i].surfaceType){
				case SURFACE_TYPE__ABSORVENT:
					printf("Absorvent\n");
					break;
				case SURFACE_TYPE__ELASTIC:
					printf("Elastic\n");
					break;
				case SURFACE_TYPE__RIGID:
					printf("Rigid\n");
					break;
				case SURFACE_TYPE__VACUUM:
					printf("Vacuum\n");
					break;
			}
			printf("object[%u].surfaceAttenUnits: \t", i);
			switch(globals->settings.objects.object[i].surfaceAttenUnits){
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
		}
	}
	
	/* batimetry block	*/
	printf("batimetry.surfaceType: \t");
	switch(globals->settings.batimetry.surfaceType){
		case SURFACE_TYPE__ABSORVENT:
			printf("Absorvent\n");
			break;
		case SURFACE_TYPE__ELASTIC:
			printf("Elastic\n");
			break;
		case SURFACE_TYPE__RIGID:
			printf("Rigid\n");
			break;
		case SURFACE_TYPE__VACUUM:
			printf("Vacuum\n");
			break;
	}

	printf("batimetry.surfacePropertyType:\t");
	switch(globals->settings.batimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("Homogeneous\n");
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			printf("Non-Homogeneous\n");
			break;
	}

	printf("batimetry.surfaceInterpolation:");
	switch(globals->settings.batimetry.surfaceInterpolation){
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

	printf("batimetry.surfaceAttenUnits: \t");
	switch(globals->settings.batimetry.surfaceAttenUnits){
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

	printf("batimetry.numSurfaceCoords:\t%lu\n", (long unsigned int)globals->settings.batimetry.numSurfaceCoords);

	printf("batimetry.surfaceProperties:	");
	switch(globals->settings.batimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("cp:%lf\n",	globals->settings.batimetry.surfaceProperties[0].cp);
			printf("\t\t\t\tcs:%lf\n",	globals->settings.batimetry.surfaceProperties[0].cs);
			printf("\t\t\t\trho:%lf\n",	globals->settings.batimetry.surfaceProperties[0].rho);
			printf("\t\t\t\tap:%lf\n",	globals->settings.batimetry.surfaceProperties[0].ap);
			printf("\t\t\t\tas:%lf\n",	globals->settings.batimetry.surfaceProperties[0].as);
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			for(i=0; i<globals->settings.batimetry.numSurfaceCoords; i++){
				printf("cp:%lf; ",	globals->settings.batimetry.surfaceProperties[i].cp);
				printf("cs:%lf; ",	globals->settings.batimetry.surfaceProperties[i].cs);
				printf("rho:%lf; ",	globals->settings.batimetry.surfaceProperties[i].rho);
				printf("ap:%lf; ",	globals->settings.batimetry.surfaceProperties[i].ap);
				printf("as:%lf;\n",	globals->settings.batimetry.surfaceProperties[i].as);
			}
			break;
	}

	printf("output.arrayType: \t\t");
	switch(globals->settings.output.arrayType){
		case ARRAY_TYPE__RECTANGULAR:
			printf("Rectangular\n");
			break;
		case ARRAY_TYPE__HORIZONTAL:
			printf("Horizontal\n");
			break;
		case ARRAY_TYPE__VERTICAL:
			printf("Vertical\n");
			break;
		case ARRAY_TYPE__LINEAR:
			printf("Linear\n");
			break;
	}
	printf("output.nArrayR: \t\t%lu\n",(long unsigned int)globals->settings.output.nArrayR);
	printf("output.nArrayZ: \t\t%lu\n",(long unsigned int)globals->settings.output.nArrayZ);

	printf("output.calcType: \t\t");
	switch(globals->settings.output.calcType){
		case CALC_TYPE__RAY_COORDS:
			printf("Ray Coordinates\n");
			break;
		case CALC_TYPE__ALL_RAY_INFO:
			printf("All ray information\n");
			break;
		case CALC_TYPE__EIGENRAYS_REG_FALSI:
			printf("Eigenrays using Regula Falsi\n");
			break;
		case CALC_TYPE__EIGENRAYS_PROXIMITY:
			printf("Eigenrays using Proximity method\n");
			break;
		case CALC_TYPE__AMP_DELAY_REG_FALSI:
			printf("Amplitudes and Delays using Regula Falsi\n");
			break;
		case CALC_TYPE__AMP_DELAY_PROXIMITY:
			printf("Amplitudes and Delays using Proximity method\n");
			break;
		case CALC_TYPE__COH_ACOUS_PRESS:
			printf("Coherent Acoustic Pressure\n");
			break;
		case CALC_TYPE__COH_TRANS_LOSS:
			printf("Coherent Transmission loss\n");
			break;
		case CALC_TYPE__COH_PART_VEL:
			printf("Coherent Particle Velocity\n");
			break;
		case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
			printf("Coherent Acoustic Pressure and Particle Velocity\n");
			break;
	}
	printf("output.miss: \t\t\t%12.5lf\n",globals->settings.output.miss);
}

void		reallocRay(ray_t* ray, uintptr_t numRayCoords){
	ray->nCoords	= numRayCoords;
	ray->r			= reallocDouble(	ray->r,			numRayCoords);
	ray->z			= reallocDouble(	ray->z,			numRayCoords);
	ray->c			= reallocDouble(	ray->c,			numRayCoords);
	ray->iRefl		= reallocUint(		ray->iRefl,		numRayCoords);
	ray->decay		= reallocComplex(	ray->decay,		numRayCoords);
	ray->phase		= reallocDouble(	ray->phase,		numRayCoords);
	ray->tau		= reallocDouble(	ray->tau,		numRayCoords);
	ray->s			= reallocDouble(	ray->s, 		numRayCoords);
	ray->Ic			= reallocDouble(	ray->Ic,		numRayCoords);
	ray->boundaryTg	= reallocVector(	ray->boundaryTg,numRayCoords);
	ray->boundaryJ	= reallocUint(		ray->boundaryJ,	numRayCoords);
	ray->nRefrac	= numRayCoords;
	ray->refrac		= reallocPoint(		ray->refrac,		numRayCoords);
}

void		copyDoubleToPtr(double* origin, double* dest, uintptr_t nItems){
	uintptr_t	i;

	for( i=0; i<nItems; i++ ){
		dest[i] = origin[i];
	}
}

void		copyDoubleToPtr2D(double** origin, double* dest, uintptr_t rowSize, uintptr_t colSize){
	uintptr_t	i,j;

	for( j=0; j<colSize; j++ ){
		for(i=0; i<rowSize; i++){
			dest[j*rowSize +i] = origin[j][i];
		}
	}
}
