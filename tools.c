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

#pragma		once
#include	"globals.h"
#include	<stdlib.h>
#include 	<stdio.h>
#include	<string.h>
#include	<sys/time.h>		//for struct time_t
#include	<sys/resource.h>	//for getrusage()
#include	<matrix.h>			//for matlab functions (used in copyComplexToPtr and copyComplexToPtr2D)

/** NOTE: the memory reallocation functions contained in this file are mostly not in use du to random occurences of "bus error".                       
 */


/****************************
 *	Function prototypes		*
 ***************************/
uint32_t		isnan_d(double);
double			min(double, double);
double			max(double, double);
void 			fatal(const char*);
FILE*			openFile(const char* , const char[4]);
char*			mallocChar(uintptr_t);
uint32_t*		mallocUint(uintptr_t);
uint32_t*		reallocUint(uint32_t*, uintptr_t);
int32_t*		mallocInt(uintptr_t);
int32_t*		reallocInt(int32_t*, uintptr_t);
double*			mallocDouble(uintptr_t);
double*			reallocDouble(double*, uintptr_t);
void			freeDouble(double*);
double**		mallocDouble2D(uintptr_t, uintptr_t);
void			freeDouble2D(double**, uintptr_t);
complex double*	mallocComplex(uintptr_t);
complex double*	reallocComplex(complex double*, uintptr_t);
void			freeComplex(complex double*);
complex double** mallocComplex2D(uintptr_t, uintptr_t);
void			freeComplex2D(complex double**, uintptr_t);
settings_t*		mallocSettings(void);
void			freeInterface(interface_t*);
void			freeObject(object_t*);
void			freeSettings(settings_t*);
vector_t*		mallocVector(uintptr_t);
vector_t*		reallocVector(vector_t*, uintptr_t);
point_t*		mallocPoint(uintptr_t);
point_t*		reallocPoint(point_t*, uintptr_t);
double 			readDouble(FILE*);
int32_t			readInt(FILE*);
char*			readStringN(FILE*, uint32_t);
void			skipLine(FILE*);
void			printSettings(settings_t*);
ray_t*			makeRay(uintptr_t);
void			reallocRayMembers(ray_t*, uintptr_t);

void			copyDoubleToPtr(double*, double*, uintptr_t);
void			copyDoubleToPtr2D(double**, double*, uintptr_t, uintptr_t);
void			copyComplexToPtr(complex double*, mxArray*, uintptr_t);
void			copyComplexToPtr2D(complex double**, mxArray*, uintptr_t, uintptr_t);
void			printCpuTime(FILE*);

/****************************
 *	Actual Functions		*
 ***************************/


uint32_t isnan_d(double x){
	//Note that isnan() is only defined for the float data type, and not for doubles
	//NANs are never equal to anything -even themselves:
	if (x!=x){
		return TRUE;
	}else{
		return FALSE;
	}
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

	temp = malloc(numUints*sizeof(uint32_t));
	return temp;
}

uint32_t*	reallocUint(uint32_t* old, uintptr_t numUints){
	/*
		Allocates an uint array and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	DEBUG(10,"reallocUint(),\t in\n");
	uint32_t*	new = NULL;

	if(numUints == 0){
		free(old);
	}else{
		new = realloc(old, numUints*sizeof(uint32_t));
		if (new == NULL){
			fatal("Memory allocation error.\n");
		}
	}
	DEBUG(10,"reallocUint(),\t out\n");
	return new;
}

int32_t*	mallocInt(uintptr_t numInts){
	/*
		Allocates a char string and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	int32_t*	temp = NULL;	//temporary pointer

	temp = malloc(numInts*sizeof(int32_t));
	return temp;
}

int32_t*	reallocInt(int32_t* old, uintptr_t numInts){
	/*
		Allocates a char string and returns a pointer to it in case of success,
		exits with error code otherwise.
	*/
	int32_t*	new = NULL;
	
	if(numInts == 0){
		free(old);
	}else{
		new = realloc(old, numInts*sizeof(int32_t));
		if (new == NULL){
			fatal("Memory allocation error.\n");
		}
	}
	return new;
}

double*		mallocDouble(uintptr_t numDoubles){
	/*
		Allocates an array of doubles and returns a pointer to it in case of success,
		Exits with error code otherwise.
	*/
	DEBUG(9,"in\n");
	double*	temp = NULL;	//temporary pointer
	temp = malloc(numDoubles * sizeof(double));
	if(temp == NULL){
		fatal("Memory alocation error.\n");
	}
	DEBUG(9,"out\n");
	return temp;
}

double*		reallocDouble(double* old, uintptr_t numDoubles){
	DEBUG(10,"in\n");
	double*		new = NULL;
	
	if(numDoubles == 0){
		free(old);
	}else{
		new = realloc(old, numDoubles*sizeof(double));
		if (new == NULL){
			fatal("Memory allocation error.\n");
		}
	}
	DEBUG(10,"out\n");
	return new;
}

void		freeDouble(double* greenMile){
	if(greenMile != NULL){
		free(greenMile);
	}
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

void		freeDouble2D(double** greenMile, uintptr_t items){
	/*
	 * frees the memory allocated to a double pointer of type double.
	 */
	 uintptr_t	i;
	 
	for(i=0; i<items; i++){
		if(greenMile[i] != NULL){
			free(greenMile[i]);
		}
	}
	free(greenMile);
}

complex double*		mallocComplex(uintptr_t numComplex){
	complex double*	temp = NULL;
	
	temp = malloc(numComplex * sizeof(complex double));
	if(temp == NULL)
		fatal("Memory alocation error.");
	return temp;
}

complex double*		reallocComplex(complex double* old, uintptr_t numComplex){
	complex double*	new = NULL;

	if(numComplex == 0){
		free(old);
	}else{
		new = realloc(old, numComplex*sizeof(complex double));
		if (new == NULL){
			fatal("Memory allocation error.\n");
		}
	}
	return new;
}

void		freeComplex(complex double* greenMile){
	if(greenMile != NULL){
		free(greenMile);
	}
}

complex double**	mallocComplex2D(uintptr_t numRows, uintptr_t numCols){
	/*
	 * Returns a pointer to an array of pointer do doubles.
	 * Or:
	 * Return a 2D Array.
	 */

	uint32_t	i;
	complex double**	array = NULL;
	array = malloc(numRows * sizeof(uintptr_t*));	//malloc an array of pointers
	
	if(array == NULL)
		fatal("Memory allocation error.\n");

	for(i = 0; i < numRows; i++){
		array[i] = mallocComplex(numCols);	//Nota that mallocDouble() already checks for allocation errors
	}

	return array;
}

void		freeComplex2D(complex double** greenMile, uintptr_t items){
	/*
	 * frees the memory allocated to a double pointer of type complex double.
	 */
	 uintptr_t	i;
	 
	for(i=0; i<items; i++){
		if(greenMile[i] != NULL){
			free(greenMile[i]);
		}
	}
	free(greenMile);
}

settings_t* 	mallocSettings(void){
	/*
		Allocate memory for a settings structure.
		Return pointer in case o success, exit with error code otherwise.
	*/
	settings_t*	settings = NULL;
	settings = malloc(sizeof(settings_t));
	if(settings == NULL){
		fatal("Memory allocation error.\n");
	}

	settings->cTitle = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	settings->source.thetas = NULL; //memory will be properly allocated in "readin.c"

	settings->altimetry.r = NULL;
	settings->altimetry.z = NULL;
	//settings->altimetry.surfaceProperties = NULL;

	settings->batimetry.r = NULL;
	settings->batimetry.z = NULL;
	//settings->batimetry.surfaceProperties = NULL;

	settings->output.arrayR = NULL;
	settings->output.arrayZ = NULL;

	return(settings);
}

void		freeInterface(interface_t* interface){
	if(interface != NULL){
		//Note that reallcing to size 0, corresponds to deallocing the memory
		reallocDouble(interface->r, 0);
		reallocDouble(interface->z, 0);
		reallocDouble(interface->cp, 0);
		reallocDouble(interface->cs, 0);
		reallocDouble(interface->rho, 0);
		reallocDouble(interface->ap, 0);
		reallocDouble(interface->as, 0);
		//free(interface);
	}
}

void		freeSettings(settings_t* settings){
	/*
	 * Go through all items in a settings struct and free the alocated memory.
	 */

	uintptr_t		i;

	if(settings != NULL){
		
		//free title:
		if(settings->cTitle != NULL){
			free(settings->cTitle);
		}
		
		//free source:
		if(&settings->source != NULL){
			reallocDouble(settings->source.thetas, 0);
		}
		
		//free altimetry:
		freeInterface(&settings->altimetry);
		
		//free soundSpeed:
		if(&settings->soundSpeed != NULL){
			freeDouble(settings->soundSpeed.z);
			//note that the range coordinates of the soundspeed are only allocated for cDist = C_DIST__FIELD
			
			switch (settings->soundSpeed.cDist){
				case C_DIST__PROFILE:
					freeDouble(settings->soundSpeed.c1D);
					break;
					
				case C_DIST__FIELD:
					freeDouble(settings->soundSpeed.r);
					freeDouble2D(settings->soundSpeed.c2D, settings->soundSpeed.nz);
					break;
					
				default:
					//this should not be possible, as the values are verified when reading the input file
					//if this occurs, some kind of memmory corruption must have taken place
					fatal("freeSettings(): Unknown Sound distribution type (neither profile nor field)");
					break;
			}
		}
		//free objects:
		if(&settings->objects != NULL){
			if(settings->objects.numObjects > 0){
				for (i=0; i<settings->objects.numObjects; i++){
					if (settings->objects.object[i].nCoords > 0){
						freeDouble(settings->objects.object[i].r);
						freeDouble(settings->objects.object[i].zDown);
						freeDouble(settings->objects.object[i].zUp);
					}
				}
				free(settings->objects.object);
			}
		}
		
		//free batimetry:
		freeInterface(&settings->batimetry);
		
		//free output (array configuration and acoustic pressure -if calculated):
		if(&settings->output != NULL){
			if(settings->output.nArrayR > 0){
				freeDouble(settings->output.arrayR);
			}
			if (settings->output.nArrayZ > 0){
				freeDouble(settings->output.arrayZ);
			}
			
			//Acoustic pressure is only calculated for some types of output
			if(	settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS	||
				settings->output.calcType == CALC_TYPE__COH_TRANS_LOSS	||
				settings->output.calcType == CALC_TYPE__PART_VEL		||
				settings->output.calcType == CALC_TYPE__COH_ACOUS_PRESS_PART_VEL){
					
				if (settings->output.arrayType == ARRAY_TYPE__RECTANGULAR){
					if(settings->output.pressure2D != NULL){
						freeComplex2D(settings->output.pressure2D, settings->output.nArrayZ);
					}
				}else{
					freeComplex(settings->output.pressure1D);
				}
			}
		}

		//free the actual settings struct:
		free(settings);
	}
}

vector_t*	mallocVector(uintptr_t	numVectors){
	vector_t*	temp = NULL;

	temp = malloc(numVectors * sizeof(vector_t));
	if(temp == NULL)
		fatal("Memory alocation error.");
	return temp;
}

vector_t*	reallocVector(vector_t* old, uintptr_t	numVectors){
	vector_t*	new = NULL;

	if(numVectors == 0){
		free(old);
	}else{
		new = realloc(old, numVectors * sizeof(vector_t));
		if (new == NULL){
			fatal("Memory allocation error.\n");
		}
	}
	return new;
}

point_t*	mallocPoint(uintptr_t	numPoints){
	point_t*	temp = NULL;

	temp = malloc(numPoints * sizeof(point_t));
	if(temp == NULL)
		fatal("Memory alocation error.");
	return temp;
}

point_t*	reallocPoint(point_t* old, uintptr_t	numPoints){
	point_t*	new = NULL;

	if(numPoints == 0){
		free(old);
	}else{
		new = realloc(old, numPoints * sizeof(point_t));
		if (new == NULL){
			//NOTE when freeing memory (setting it to size 0) a null pointer is not an error.
			fatal("reallocPoint(): Memory allocation error.\n");
		}
	}
	return new;
}

double 		readDouble(FILE* infile){
	/************************************************
	 *	Reads a double from a file and returns it	*
	 ***********************************************/
	 
	char*	junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));;
	double 	tempDouble;
	int32_t	junkInt;
	
	junkInt = fscanf(infile, "%s\n", junkString);
	tempDouble = atof(junkString);
	free(junkString);
	
	return(tempDouble);
}

int32_t		readInt(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	 
	char*		junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	int32_t 	tempInt;
	int32_t		junkInt;
	
	junkInt = fscanf(infile, "%s\n", junkString);
	tempInt = (int32_t)atol(junkString);
	free(junkString);
	
	return(tempInt);
}

char*		readStringN(FILE* infile, uint32_t length){
	/********************************************************************
	 *	Reads a <lenght> chars from a filestream.						*
	 *******************************************************************/
	char*		outputString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));;
	char*		junkChar;

	junkChar = fgets(outputString, (int32_t)length, infile);
	return(outputString);
}

void		skipLine(FILE* infile){
	/************************************************
	 *	Reads a int from a file and returns it		*
	 ***********************************************/
	char*		junkString = mallocChar((uintptr_t)(MAX_LINE_LEN + 1));
	char*		junkChar;
	
	junkChar = fgets(junkString, MAX_LINE_LEN+1, infile);

	free(junkString);
}

void		printSettings(settings_t*	settings){
	/************************************************
	 *	Outputs a settings structure to stdout.		*
	 ***********************************************/
	DEBUG(1, "in\n");
	uint32_t	i;
	
	printf("cTitle: \t\t\t%s", settings->cTitle);	//assuming a \n at the end of cTitle
	printf("source.ds: \t\t\t%12.5lf\t[m]\n", settings->source.ds);
	printf("source.rx: \t\t\t%12.5lf\t[m]\n", settings->source.rx);
	printf("source.zx: \t\t\t%12.5lf\t[m]\n", settings->source.zx);
	printf("source.rbox1: \t\t\t%12.5lf\t[m]\n", settings->source.rbox1);
	printf("source.rbox2: \t\t\t%12.5lf\t[m]\n", settings->source.rbox2);
	printf("source.freqx: \t\t\t%12.5lf\t[m]\n", settings->source.freqx);
	printf("source.nThetas: \t\t%6.0lu\n", (long unsigned int)settings->source.nThetas);

	/* uncoment the following block to output all launching angles */
	/*
	for(i=0; i<settings->source.nThetas; i++){
		printf("source.thetas[%ld\t]: \t%lf\n", i, settings->source.thetas[i]);
	}
	*/
	printf("altimetry.surfaceType: \t\t");
	switch(settings->altimetry.surfaceType){
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
	switch(settings->altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("Homogeneous\n");
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			printf("Non-Homogeneous\n");
			break;
	}

	printf("altimetry.surfaceInterpolation: ");
	switch(settings->altimetry.surfaceInterpolation){
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
	switch(settings->altimetry.surfaceAttenUnits){
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

	printf("altimetry.numSurfaceCoords:\t%lu\n", (long unsigned int)settings->altimetry.numSurfaceCoords);

	printf("altimetry.surfaceProperties:	");
	switch(settings->altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("cp:%lf\n ",	settings->altimetry.cp[0]);
			printf("\t\t\t\tcs:%lf\n ",	settings->altimetry.cs[0]);
			printf("\t\t\t\trho:%lf\n ",	settings->altimetry.rho[0]);
			printf("\t\t\t\tap:%lf\n ",	settings->altimetry.ap[0]);
			printf("\t\t\t\tas:%lf\n",	settings->altimetry.as[0]);
			break;
			
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			for(i=0; i<settings->altimetry.numSurfaceCoords; i++){
				printf("cp:%lf; ",	settings->altimetry.cp[i]);
				printf("cs:%lf; ",	settings->altimetry.cs[i]);
				printf("rho:%lf; ",	settings->altimetry.rho[i]);
				printf("ap:%lf; ",	settings->altimetry.ap[i]);
				printf("as:%lf;\n",	settings->altimetry.as[i]);
			}
			break;
	}
	printf("altimetry.r[0]:\t\t\t%lf\n", settings->altimetry.r[0]);
	printf("altimetry.r[N]:\t\t\t%lf\n", settings->altimetry.r[settings->altimetry.numSurfaceCoords-1]);
	

	/* sound speed block */
	
	/*	object block	*/
	printf("objects.numObjects:\t\t%u\n",settings->objects.numObjects);
	if(settings->objects.numObjects > 0){
		for(i=0; i<settings->objects.numObjects; i++){
			printf("object[%u].surfaceType: \t", i);
			switch(settings->objects.object[i].surfaceType){
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
			switch(settings->objects.object[i].surfaceAttenUnits){
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
	switch(settings->batimetry.surfaceType){
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
	switch(settings->batimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("Homogeneous\n");
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			printf("Non-Homogeneous\n");
			break;
	}

	printf("batimetry.surfaceInterpolation:");
	switch(settings->batimetry.surfaceInterpolation){
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
	switch(settings->batimetry.surfaceAttenUnits){
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

	printf("batimetry.numSurfaceCoords:\t%lu\n", (long unsigned int)settings->batimetry.numSurfaceCoords);

	printf("batimetry.surfaceProperties:	");
	switch(settings->batimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			printf("cp:%lf\n",	settings->batimetry.cp[0]);
			printf("\t\t\t\tcs:%lf\n",	settings->batimetry.cs[0]);
			printf("\t\t\t\trho:%lf\n",	settings->batimetry.rho[0]);
			printf("\t\t\t\tap:%lf\n",	settings->batimetry.ap[0]);
			printf("\t\t\t\tas:%lf\n",	settings->batimetry.as[0]);
			break;
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			for(i=0; i<settings->batimetry.numSurfaceCoords; i++){
				printf("cp:%lf; ",	settings->batimetry.cp[i]);
				printf("cs:%lf; ",	settings->batimetry.cs[i]);
				printf("rho:%lf; ",	settings->batimetry.rho[i]);
				printf("ap:%lf; ",	settings->batimetry.ap[i]);
				printf("as:%lf;\n",	settings->batimetry.as[i]);
			}
			break;
	}

	printf("output.arrayType: \t\t");
	switch(settings->output.arrayType){
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
	printf("output.nArrayR: \t\t%lu\n",(long unsigned int)settings->output.nArrayR);
	printf("output.nArrayZ: \t\t%lu\n",(long unsigned int)settings->output.nArrayZ);

	printf("output.calcType: \t\t");
	switch(settings->output.calcType){
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
		case CALC_TYPE__PART_VEL:
			printf("Coherent Particle Velocity\n");
			break;
		case CALC_TYPE__COH_ACOUS_PRESS_PART_VEL:
			printf("Coherent Acoustic Pressure and Particle Velocity\n");
			break;
	}
	printf("output.miss: \t\t\t%12.5lf\n",settings->output.miss);

	fflush(stdout); // Will now print everything in the stdout buffer

	DEBUG(1, "out\n");
}

ray_t*		makeRay(uintptr_t numRays){
	/*
	 * allocates rays and initializes its members
	 */
	ray_t*		tempRay = NULL;
	uintptr_t	i;
	 
	tempRay = malloc(numRays * sizeof(ray_t));
	if(tempRay == NULL){
		fatal("Memory alocation error.");
	}

	for(i=0; i<numRays; i++){
		tempRay[i].r 			= NULL;
		tempRay[i].z 			= NULL;
		tempRay[i].c 			= NULL;
		tempRay[i].iRefl		= NULL;
		tempRay[i].decay		= NULL;
		tempRay[i].phase		= NULL;
		tempRay[i].tau 			= NULL;
		tempRay[i].s 			= NULL;
		tempRay[i].ic 			= NULL;
		tempRay[i].boundaryTg	= NULL;
		tempRay[i].boundaryJ	= NULL;
		tempRay[i].refrac		= NULL;
		tempRay[i].p 			= NULL;
		tempRay[i].q 			= NULL;
		tempRay[i].caustc		= NULL;
		tempRay[i].amp			= NULL;
	}
	return tempRay;
}

void		reallocRayMembers(ray_t* ray, uintptr_t numRayCoords){
	/*
	 * resizes all ray members.
	 */
	DEBUG(5,"reallocRayMembers(%u),\t in\n", (uint32_t)numRayCoords);
	ray->nCoords	= numRayCoords;
	ray->r			= reallocDouble(	ray->r,			numRayCoords);
	ray->z			= reallocDouble(	ray->z,			numRayCoords);
	ray->c			= reallocDouble(	ray->c,			numRayCoords);
	ray->iRefl		= reallocUint(		ray->iRefl,		numRayCoords);
	ray->decay		= reallocComplex(	ray->decay,		numRayCoords);
	ray->phase		= reallocDouble(	ray->phase,		numRayCoords);
	ray->tau		= reallocDouble(	ray->tau,		numRayCoords);
	ray->s			= reallocDouble(	ray->s, 		numRayCoords);
	ray->ic			= reallocDouble(	ray->ic,		numRayCoords);
	ray->boundaryTg	= reallocVector(	ray->boundaryTg,numRayCoords);
	ray->boundaryJ	= reallocInt(		ray->boundaryJ,	numRayCoords);
	ray->nRefrac	= numRayCoords;
	ray->refrac		= reallocPoint(		ray->refrac,	numRayCoords);
	ray->p			= reallocDouble(	ray->p,			numRayCoords);
	ray->q			= reallocDouble(	ray->q,			numRayCoords);
	ray->caustc		= reallocDouble(	ray->caustc,	numRayCoords);
	ray->amp		= reallocComplex(	ray->amp,		numRayCoords);
	DEBUG(5,"reallocRayMembers(), \t out\n");
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
			dest[i*colSize +j] = origin[j][i];
		}
	}
}

void		copyComplexToPtr(complex double* origin, mxArray* dest, uintptr_t nItems){
	uintptr_t	i;
	double*	destImag = NULL;
	double*	destReal = NULL;
	
	//get a pointer to the real and imaginary parts of the destination:
	destReal = mxGetData(dest);
	destImag = mxGetImagData(dest);
	
	for( i=0; i<nItems; i++ ){
		destReal[i] = creal(origin[i]);
		destImag[i] = cimag(origin[i]);
	}
}

void		copyComplexToPtr2D(complex double** origin, mxArray* dest, uintptr_t rowSize, uintptr_t colSize){
	uintptr_t	i,j;
	double*	destImag = NULL;
	double*	destReal = NULL;
	
	//get a pointer to the real and imaginary parts of the destination:
	destReal = mxGetData(dest);
	destImag = mxGetImagData(dest);
	
	for( j=0; j<colSize; j++ ){
		for(i=0; i<rowSize; i++){
			destReal[i*colSize +j] = creal(origin[j][i]);
			destImag[i*colSize +j] = cimag(origin[j][i]);
		}
	}
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
