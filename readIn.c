/********************************************************************************
 *	readIn.c																	*
 * 	(formerly "readin.for")														*
 *	read the waveguide input file.												*
 * 																				*
 *	originally written in FORTRAN by:											*
 *  					Orlando Camargo Rodriguez:								*
 *						Copyright (C) 2010										*
 * 						Orlando Camargo Rodriguez								*
 *						orodrig@ualg.pt											*
 *						Universidade do Algarve									*
 *						Physics Department										*
 *						Signal Processing Laboratory							*
 *																				*
 *	Ported to C for project SENSOCEAN by:										*
 * 						Emanuel Ey												*
 *						emanuel.ey@gmail.com									*
 *						Signal Processing Laboratory							*
 *						Universidade do Algarve									*
 *																				*
 *	Inputs:																		*
 * 			settings	A pointer to the input settings.						*
 *			filename	A string containg the path to the file to be opened.	*
 * 	Outputs:																	*
 * 			All values are stored in "globals"									*
 * 																				*
 *******************************************************************************/

#include <stdio.h>
#include <inttypes.h>		//contains definitions of integer data types that are inequivocal.
#include "tools.c"			
#include "globals.h"		//Include global variables
#include <math.h>

//prototype:
void	readIn(settings_t*, const char*);

//actual function declaration:
void	readIn(settings_t* settings, const char* filename){

	uint32_t	i,j;
	float		dTheta;
	uint32_t	nThetas;			//used locally to make code more readable. Value is stored in settings.
	float		theta0;				//used locally to make code more readable. Value is stored in settings.
	float		thetaN;				//used locally to make code more readable. Value is stored in settings.
	uint32_t	numSurfaceCoords;	//used locally to make code more readable. Value is stored in settings.
	uint32_t	nr0, nz0;			//used locally to make code more readable. Value is stored in settings.
	char*		tempString;
	char*		junkChar;
	FILE*		infile;					//a pointer for the input file
	infile = openFile(filename, "r");	//open file in "read" mode

	if (VERBOSE)
		printf("Reading cTraceo input file \"%s\"\n", filename);


	/************************************************************************
	 *	Read the title:														*
	 ***********************************************************************/
	junkChar = fgets(settings->cTitle, MAX_LINE_LEN+1, infile);


	/************************************************************************
	 *	Read and validate the source info:									*
	 ***********************************************************************/
	 skipLine(infile);
	 settings->source.ds		= readfloat(infile);
	 settings->source.rx		= readfloat(infile);
	 settings->source.zx		= readfloat(infile);
	 settings->source.rbox1		= readfloat(infile);
	 settings->source.rbox2		= readfloat(infile);
	 settings->source.freqx		= readfloat(infile);
	 nThetas = (uint32_t)readInt(infile);
	 settings->source.nThetas	= nThetas;

	/*	Source validation	*/
	if(settings->source.ds == 0.0 ){
		settings->source.ds = fabs(	settings->source.rbox2 -
											settings->source.rbox1)/100;
	}
	if(	(settings->source.rx < settings->source.rbox1) ||
		(settings->source.rx > settings->source.rbox2)){
		fatal(	"Input file: Source: initial range is outside the range box!\nAborting...");
	}
	
	/*	Allocate memory for the launching angles	*/
	settings->source.thetas = mallocfloat(nThetas);
	DEBUG(8,"number of launching angles in infile: %u\n", nThetas);
	/*	Read the  thetas from the file	*/
	theta0 = readfloat(infile);
	thetaN = readfloat(infile);
	if(settings->source.nThetas == 1){
		settings->source.thetas[0] = theta0;
		dTheta = 0;
	}else if(settings->source.nThetas == 2){
		settings->source.thetas[0] = theta0;
		settings->source.thetas[1] = thetaN;
		dTheta = thetaN - theta0;
	}else{
		settings->source.thetas[0] = theta0;
		settings->source.thetas[nThetas - 1] = thetaN;
		dTheta =	(thetaN - theta0 ) / ( (float)nThetas - 1 );
		
		for(i=1;i <= nThetas-2; i++){
			settings->source.thetas[i] = theta0 +dTheta *(float)(i);
		}
	}
	settings->source.dTheta = dTheta;
	DEBUG(10,"\n");
	/************************************************************************
	 * Read and validate altimetry info:									*
	 ***********************************************************************/
	 
	skipLine(infile);

	/* surfaceType;	formerly "atype"	*/
	tempString = readStringN(infile,5);
	if(strcmp(tempString,"'A'\n") == 0){
		settings->altimetry.surfaceType	= SURFACE_TYPE__ABSORVENT;
		
	}else if(strcmp(tempString,"'E'\n") == 0){
		settings->altimetry.surfaceType	= SURFACE_TYPE__ELASTIC;
		
	}else if(strcmp(tempString,"'R'\n") == 0){
		settings->altimetry.surfaceType	= SURFACE_TYPE__RIGID;
		
	}else if(strcmp(tempString,"'V'\n") == 0){
		settings->altimetry.surfaceType	= SURFACE_TYPE__VACUUM;
		
	}else{
		printf("Input file: batimetry: unknown surface type %s\n", tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* surfacePropertyType;		//formerly "aptype"	*/
	tempString = readStringN(infile,5);
	if(strcmp(tempString,"'H'\n") == 0){
		settings->altimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
		
	}else if(strcmp(tempString,"'N'\n") == 0){
		settings->altimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
		
	}else{
		printf("Input file: altimetry: surface property type: '%s'\n",tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* surfaceInterpolation;	//formerly "aitype"	*/
	tempString = readStringN(infile,6);
	if(strcmp(tempString,"'FL'\n") == 0){
		settings->altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__FLAT;
		
	}else if(strcmp(tempString,"'SL'\n") == 0){
		settings->altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__SLOPED;
		
	}else if(strcmp(tempString,"'2P'\n") == 0){
		settings->altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__2P;
		
	}else if(strcmp(tempString,"'3P'\n") == 0){
		settings->altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__3P;
		
	}else if(strcmp(tempString,"'4P'\n") == 0){
		settings->altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__4P;
		
	}else{
		printf("Input file: altimetry: unknown surface interpolation type: '%s'\n",tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* surfaceAttenUnits;		//formerly "atiu"	*/
	tempString = readStringN(infile,5);
	if(strcmp(tempString,"'F'\n") == 0){
		settings->altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperkHz;
		
	}else if(strcmp(tempString,"'M'\n") == 0){
		settings->altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperMeter;
		
	}else if(strcmp(tempString,"'N'\n") == 0){
		settings->altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperNeper;
		
	}else if(strcmp(tempString,"'Q'\n") == 0){
		settings->altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__qFactor;
		
	}else if(strcmp(tempString,"'W'\n") == 0){
		settings->altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperLambda;
		
	}else{
		printf("Input file: altimetry: unknown surface attenuation units: '%s'\n",tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* numSurfaceCoords;		//formerly "nati" */
	numSurfaceCoords = (uint32_t)readInt(infile);
	settings->altimetry.numSurfaceCoords = numSurfaceCoords;

	//malloc interface coords
	settings->altimetry.r = mallocfloat(numSurfaceCoords);
	settings->altimetry.z = mallocfloat(numSurfaceCoords);
	
	//read the surface properties and coordinates
	switch(settings->altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			//malloc and read only one set of interface properties:
			settings->altimetry.cp	= mallocfloat(1);
			settings->altimetry.cs	= mallocfloat(1);
			settings->altimetry.rho	= mallocfloat(1);
			settings->altimetry.ap	= mallocfloat(1);
			settings->altimetry.as	= mallocfloat(1);
			
			settings->altimetry.cp[0]	= readfloat(infile);
			settings->altimetry.cs[0]	= readfloat(infile);
			settings->altimetry.rho[0]	= readfloat(infile);
			settings->altimetry.ap[0]	= readfloat(infile);
			settings->altimetry.as[0]	= readfloat(infile);
			
			//read coordinates of interface points:
			for(i=0; i<numSurfaceCoords; i++){
				settings->altimetry.r[i] = readfloat(infile);
				settings->altimetry.z[i] = readfloat(infile);
			}
			break;
		
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			//Read coordinates and interface properties for all interface points:
			settings->altimetry.cp	= mallocfloat(numSurfaceCoords);
			settings->altimetry.cs	= mallocfloat(numSurfaceCoords);
			settings->altimetry.rho	= mallocfloat(numSurfaceCoords);
			settings->altimetry.ap	= mallocfloat(numSurfaceCoords);
			settings->altimetry.as	= mallocfloat(numSurfaceCoords);
				
			for(i=0; i<numSurfaceCoords; i++){
				settings->altimetry.r[i]	= readfloat(infile);
				settings->altimetry.z[i]	= readfloat(infile);
				settings->altimetry.cp[i]	= readfloat(infile);
				settings->altimetry.cs[i]	= readfloat(infile);
				settings->altimetry.rho[i]	= readfloat(infile);
				settings->altimetry.ap[i]	= readfloat(infile);
				settings->altimetry.as[i]	= readfloat(infile);
			}
			break;
	}

	/************************************************************************
	 * Read and validate sound speed info:									*
	 ***********************************************************************/
	/*	sound speed distribution	"cdist"		*/
	skipLine(infile);
	tempString = readStringN(infile,10);
	if(strcmp(tempString,"'c(z,z)'\n") == 0){
		settings->soundSpeed.cDist	= C_DIST__PROFILE;
	}else if(strcmp(tempString,"'c(r,z)'\n") == 0){
		settings->soundSpeed.cDist	= C_DIST__FIELD;
	}else{
		printf("Input file: Sound Speed: unknown sound speed distribution type: '%s'\n",tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/*	sound speed class	"cclass"		*/
	tempString = readStringN(infile,7);
	if(strcmp(tempString,"'ISOV'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__ISOVELOCITY;
		
	}else if(strcmp(tempString,"'LINP'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__LINEAR;
		
	}else if(strcmp(tempString,"'PARP'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__PARABOLIC;
		
	}else if(strcmp(tempString,"'EXPP'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__EXPONENTIAL;
		
	}else if(strcmp(tempString,"'N2LP'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__N2_LINEAR;
		
	}else if(strcmp(tempString,"'ISQP'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__INV_SQUARE;
		
	}else if(strcmp(tempString,"'MUNK'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__MUNK;
		
	}else if(strcmp(tempString,"'TABL'") == 0){
		settings->soundSpeed.cClass	= C_CLASS__TABULATED;
		
	}else{
		printf("Input file: Sound Speed: unknown sound class type: '%s'\n",tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* number of points in range and depth, "nr0,nz0" */
	nr0 = (uint32_t)readInt(infile);
	nz0 = (uint32_t)readInt(infile);
	settings->soundSpeed.nr0 = nr0;
	settings->soundSpeed.nz0 = nz0;

	//read actual values of soundspeed profile/field:
	switch(settings->soundSpeed.cDist){
		
		case C_DIST__PROFILE:
			if(settings->soundSpeed.cClass != C_CLASS__TABULATED){
				//all cClasses execept for "TABULATED" only require ( z0(0),c0(0) ) and ( z0(1), c0(1) )
				//malloc z0 an c0:
				settings->soundSpeed.z0 = mallocfloat(2);
				settings->soundSpeed.c01d = mallocfloat(2);

				//read 4 values:
				settings->soundSpeed.z0[0] = readfloat(infile);
				settings->soundSpeed.c01d[0] = readfloat(infile);
				settings->soundSpeed.z0[1] = readfloat(infile);
				settings->soundSpeed.c01d[1] = readfloat(infile);

				// validate the values that were just read:
				if(	(settings->soundSpeed.cClass != C_CLASS__ISOVELOCITY) &&
					(settings->soundSpeed.cClass != C_CLASS__MUNK)){
					
					if(settings->soundSpeed.z0[0] == settings->soundSpeed.z0[1])
						fatal("Input file: Analytical sound speed: z[1] == z[0] Only valid for Isovelocity and Munk Options!\nAborting...");

					if(settings->soundSpeed.c01d[0] == settings->soundSpeed.c01d[1])
						fatal("Input file: Analytical sound speed: c[1] == c[0] Only valid for Isovelocity option!\nAborting...");
				}
			}else if(settings->soundSpeed.cClass == C_CLASS__TABULATED){
				//malloc z0 an c0:
				settings->soundSpeed.z0 = mallocfloat(nz0);
				settings->soundSpeed.c01d = mallocfloat(nz0);

				//read pairs of z0 and c0
				for(i=0; i<settings->soundSpeed.nz0; i++){
					settings->soundSpeed.z0[i]	= readfloat(infile);
					settings->soundSpeed.c01d[i]= readfloat(infile);
				}
			}
			break;
		
		case C_DIST__FIELD:
			if(settings->soundSpeed.cClass != C_CLASS__TABULATED)
				fatal("Input file: Unknown sound speed field type.\nAborting...");
			
			//malloc ranges (vector)
			settings->soundSpeed.r0 = mallocfloat(nr0);
			//read ranges
			for(i=0; i<nr0; i++)
				settings->soundSpeed.r0[i] = readfloat(infile);

			//malloc depths (vector)
			settings->soundSpeed.z0 = mallocfloat(nz0);
			//read depths
			for(i=0; i<nz0; i++)
				settings->soundSpeed.z0[i] = readfloat(infile);

			//malloc sound speeds (2 dim matrix)
			settings->soundSpeed.c02d = mallocfloat2D(nr0, nz0);	//mallocfloat2D(numCols, numRows)
			//read actual sound speeds
			for(j=0; j<nz0; j++){		//rows
				for(i=0; i<nr0; i++){	//columns
					settings->soundSpeed.c02d[j][i] = readfloat(infile);
				}
			}
			break;
	}

	/************************************************************************
	 * Read and validate object info:										*
	 ***********************************************************************/
	skipLine(infile);
	settings->objects.numObjects = (uint32_t)readInt(infile);

	/* only attempt to read object info if at least one object exists:	*/
	if(settings->objects.numObjects > 0){
		//malloc memory for the objects:
		settings->objects.object = malloc((uintptr_t)settings->objects.numObjects * sizeof(object_t));
		if (settings->objects.object == NULL)
			fatal("Memory allocation error.");

		/*	interpolation type	(formerly "oitype")		*/
		tempString = readStringN(infile,6);
		if(strcmp(tempString,"'FL'\n") == 0){
			settings->objects.surfaceInterpolation	= SURFACE_INTERPOLATION__FLAT;
			
		}else if(strcmp(tempString,"'SL'\n") == 0){
			settings->objects.surfaceInterpolation	= SURFACE_INTERPOLATION__SLOPED;
			
		}else if(strcmp(tempString,"'2P'\n") == 0){
			settings->objects.surfaceInterpolation	= SURFACE_INTERPOLATION__2P;
			
		}else if(strcmp(tempString,"'3P'\n") == 0){
			settings->objects.surfaceInterpolation	= SURFACE_INTERPOLATION__3P;
			
		}else if(strcmp(tempString,"'4P'\n") == 0){
			settings->objects.surfaceInterpolation	= SURFACE_INTERPOLATION__4P;
			
		}else{
			printf("Input file: altimetry: unknown surface interpolation type: '%s'\n",tempString);
			fatal("Aborting...");
		}
		free(tempString);
		
		for(i=0; i<settings->objects.numObjects; i++){
			/*	surfaceType		*/
			tempString = readStringN(infile,5);
			if(strcmp(tempString,"'A'\n") == 0){
				settings->objects.object[i].surfaceType	= SURFACE_TYPE__ABSORVENT;
				
			}else if(strcmp(tempString,"'E'\n") == 0){
				settings->objects.object[i].surfaceType	= SURFACE_TYPE__ELASTIC;
				
			}else if(strcmp(tempString,"'R'\n") == 0){
				settings->objects.object[i].surfaceType	= SURFACE_TYPE__RIGID;
				
			}else if(strcmp(tempString,"'V'\n") == 0){
				settings->objects.object[i].surfaceType	= SURFACE_TYPE__VACUUM;
				
			}else{
				printf("Input file: Object %u: unknown surface type: %s\n", i, tempString);
				fatal("Aborting...");
			}
			free(tempString);
			
			/* surfaceAttenUnits	*/
			tempString = readStringN(infile,5);
			if(strcmp(tempString,"'F'\n") == 0){
				settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperkHz;
				
			}else if(strcmp(tempString,"'M'\n") == 0){
				settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperMeter;
				
			}else if(strcmp(tempString,"'N'\n") == 0){
				settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperNeper;
				
			}else if(strcmp(tempString,"'Q'\n") == 0){
				settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__qFactor;
				
			}else if(strcmp(tempString,"'W'\n") == 0){
				settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperLambda;
				
			}else{
				printf("Input file: Object %u: unknown surface attenuation units: %s\n", i, tempString);
				fatal("Aborting...");
			}
			free(tempString);
			
			settings->objects.object[i].nCoords	= (uint32_t)readInt(infile);
			settings->objects.object[i].cp		= readfloat(infile);				//compressional speed
			settings->objects.object[i].cs 		= readfloat(infile);				//shear speed
			settings->objects.object[i].rho 	= readfloat(infile);				//density
			settings->objects.object[i].ap 		= readfloat(infile);				//compressional attenuation
			settings->objects.object[i].as 		= readfloat(infile);				//shear attenuation

			//malloc memory for the object coordinates:
			settings->objects.object[i].r 		= mallocfloat((uintptr_t)settings->objects.object[i].nCoords);
			settings->objects.object[i].zDown	= mallocfloat((uintptr_t)settings->objects.object[i].nCoords);
			settings->objects.object[i].zUp		= mallocfloat((uintptr_t)settings->objects.object[i].nCoords);
			//verify succesfull allocation of memory:
			if(	(settings->objects.object[i].r 		==	NULL)	||
				(settings->objects.object[i].zDown 	==	NULL)	||
				(settings->objects.object[i].zUp 	==	NULL))
				fatal("Memory allocation error.");
			
			//read the coords of the object:
			for(j=0; j<settings->objects.object[i].nCoords; j++){
				settings->objects.object[i].r[j] 		= readfloat(infile);
				settings->objects.object[i].zDown[j]	= readfloat(infile);
				settings->objects.object[i].zUp[j] 		= readfloat(infile);
			}
		}
	}
	
	/************************************************************************
	 * Read and validate batimetry info:									*
	 ***********************************************************************/
	skipLine(infile);

	/* surfaceType;	formerly "atype"	*/
	tempString = readStringN(infile,5);
	if(strcmp(tempString,"'A'\n") == 0){
		settings->batimetry.surfaceType	= SURFACE_TYPE__ABSORVENT;
		
	}else if(strcmp(tempString,"'E'\n") == 0){
		settings->batimetry.surfaceType	= SURFACE_TYPE__ELASTIC;
		
	}else if(strcmp(tempString,"'R'\n") == 0){
		settings->batimetry.surfaceType	= SURFACE_TYPE__RIGID;
		
	}else if(strcmp(tempString,"'V'\n") == 0){
		settings->batimetry.surfaceType	= SURFACE_TYPE__VACUUM;
		
	}else{
		printf("Input file: batimetry: unknown surface type: %s\n", tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* surfacePropertyType;		//formerly "aptype"	*/
	tempString = readStringN(infile,5);
	if(strcmp(tempString,"'H'\n") == 0){
		settings->batimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
		
	}else if(strcmp(tempString,"'N'\n") == 0){
		settings->batimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
		
	}else{
		printf("Input file: batimetry: unknown surface property type: %s\n", tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* surfaceInterpolation;	//formerly "aitype"	*/
	tempString = readStringN(infile,6);
	if(strcmp(tempString,"'FL'\n") == 0){
		settings->batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__FLAT;
		
	}else if(strcmp(tempString,"'SL'\n") == 0){
		settings->batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__SLOPED;
		
	}else if(strcmp(tempString,"'2P'\n") == 0){
		settings->batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__2P;
		
	}else if(strcmp(tempString,"'3P'\n") == 0){
		settings->batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__3P;
		
	}else if(strcmp(tempString,"'4P'\n") == 0){
		settings->batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__4P;
		
	}else{
		printf("Input file: batimetry: unknown surface interpolation type: %s\n", tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* surfaceAttenUnits;		//formerly "atiu"	*/
	tempString = readStringN(infile,5);
	if(strcmp(tempString,"'F'\n") == 0){
		settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperkHz;
		
	}else if(strcmp(tempString,"'M'\n") == 0){
		settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperMeter;
		
	}else if(strcmp(tempString,"'N'\n") == 0){
		settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperNeper;
		
	}else if(strcmp(tempString,"'Q'\n") == 0){
		settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__qFactor;
		
	}else if(strcmp(tempString,"'W'\n") == 0){
		settings->batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperLambda;
		
	}else{
		printf("Input file: batimetry: unknown surface attenuation units: %s\n", tempString);
		fatal("Aborting...");
	}
	free(tempString);

	/* numSurfaceCoords;		//formerly "nati" */
	numSurfaceCoords = (uint32_t)readInt(infile);
	settings->batimetry.numSurfaceCoords = numSurfaceCoords;

	//malloc interface coords
	settings->batimetry.r = mallocfloat(numSurfaceCoords);
	settings->batimetry.z = mallocfloat(numSurfaceCoords);
	
	//read the surface properties and coordinates
	switch(settings->batimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			//malloc and read only one set of interface properties:
			settings->batimetry.cp	= mallocfloat(1);
			settings->batimetry.cs	= mallocfloat(1);
			settings->batimetry.rho	= mallocfloat(1);
			settings->batimetry.ap	= mallocfloat(1);
			settings->batimetry.as	= mallocfloat(1);
							
			settings->batimetry.cp[0]	= readfloat(infile);
			settings->batimetry.cs[0]	= readfloat(infile);
			settings->batimetry.rho[0]	= readfloat(infile);
			settings->batimetry.ap[0]	= readfloat(infile);
			settings->batimetry.as[0]	= readfloat(infile);
			
			//read coordinates of interface points:
			for(i=0; i<numSurfaceCoords; i++){
				settings->batimetry.r[i] = readfloat(infile);
				settings->batimetry.z[i] = readfloat(infile);
			}
			break;
		
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			//Read coordinates and interface properties for all interface points:
			settings->batimetry.cp	= mallocfloat(numSurfaceCoords);
			settings->batimetry.cs	= mallocfloat(numSurfaceCoords);
			settings->batimetry.rho	= mallocfloat(numSurfaceCoords);
			settings->batimetry.ap	= mallocfloat(numSurfaceCoords);
			settings->batimetry.as	= mallocfloat(numSurfaceCoords);
				
			for(i=0; i<numSurfaceCoords; i++){
				settings->batimetry.r[i]	= readfloat(infile);
				settings->batimetry.z[i]	= readfloat(infile);
				settings->batimetry.cp[i]	= readfloat(infile);
				settings->batimetry.cs[i]	= readfloat(infile);
				settings->batimetry.rho[i]	= readfloat(infile);
				settings->batimetry.ap[i]	= readfloat(infile);
				settings->batimetry.as[i]	= readfloat(infile);
			}
			break;
	}
	
	/************************************************************************
	 * Read and validate array info:										*
	 ***********************************************************************/
	skipLine(infile);
	/*	output array type "artype"	*/
	tempString = readStringN(infile,6);
	if(strcmp(tempString,"'RRY'") == 0){
		settings->output.arrayType	= ARRAY_TYPE__RECTANGULAR;
		
	}else if(strcmp(tempString,"'HRY'") == 0){
		settings->output.arrayType	= ARRAY_TYPE__HORIZONTAL;
		
	}else if(strcmp(tempString,"'VRY'") == 0){
		settings->output.arrayType	= ARRAY_TYPE__VERTICAL;
		
	}else if(strcmp(tempString,"'LRY'") == 0){
		settings->output.arrayType	= ARRAY_TYPE__LINEAR;
		
	}else{
		fatal("Input file: output: unknown array type.\nAborting...");
	}
	free(tempString);
	
	/*	output array dimensions "nra, nrz"		*/
	settings->output.nArrayR = (uint32_t)readInt(infile);
	settings->output.nArrayZ = (uint32_t)readInt(infile);

	settings->output.arrayR = mallocfloat(settings->output.nArrayR);
	settings->output.arrayZ = mallocfloat(settings->output.nArrayZ);
	if(	(settings->output.arrayR == NULL) || (settings->output.arrayZ == NULL))
		fatal("Memory allocation error.");

	//read the actual array values
	for(i=0; i<settings->output.nArrayR; i++){
		settings->output.arrayR[i] = readfloat(infile);
	}
	for(i=0; i<settings->output.nArrayZ; i++){
		settings->output.arrayZ[i] = readfloat(infile);
	}

	/************************************************************************
	 * Read and validate output info:
	 ***********************************************************************/
	skipLine(infile);
	
	/*	output calculation type "catype"	*/
	tempString = readStringN(infile,6);
	if(strcmp(tempString,"'RCO'") == 0){
		settings->output.calcType	= CALC_TYPE__RAY_COORDS;
		
	}else if(strcmp(tempString,"'ARI'") == 0){
		settings->output.calcType	= CALC_TYPE__ALL_RAY_INFO;
		
	}else if(strcmp(tempString,"'ERF'") == 0){
		settings->output.calcType	= CALC_TYPE__EIGENRAYS_REG_FALSI;
		
	}else if(strcmp(tempString,"'EPR'") == 0){
		settings->output.calcType	= CALC_TYPE__EIGENRAYS_PROXIMITY;
		
	}else if(strcmp(tempString,"'ADR'") == 0){
		settings->output.calcType	= CALC_TYPE__AMP_DELAY_REG_FALSI;
		
	}else if(strcmp(tempString,"'ADP'") == 0){
		settings->output.calcType	= CALC_TYPE__AMP_DELAY_PROXIMITY;
		
	}else if(strcmp(tempString,"'CPR'") == 0){
		settings->output.calcType	= CALC_TYPE__COH_ACOUS_PRESS;
		
	}else if(strcmp(tempString,"'CTL'") == 0){
		settings->output.calcType	= CALC_TYPE__COH_TRANS_LOSS;
		
	}else if(strcmp(tempString,"'PVL'") == 0){
		settings->output.calcType	= CALC_TYPE__PART_VEL;
		
	}else if(strcmp(tempString,"'PAV'") == 0){
		settings->output.calcType	= CALC_TYPE__COH_ACOUS_PRESS_PART_VEL;
		
	}else{

		fatal("Input file: unknown output calculation type.\nAborting...");
	}
	free(tempString);

	/*	output calculation type "catype"	*/
	settings->output.miss = readfloat(infile);

	/* Check batimetry/altimetry	*/
	if(settings->altimetry.r[0] > settings->source.rbox1)
		fatal("Minimum altimetry range > minimum rbox range.\nAborting...");
	if(settings->altimetry.r[settings->altimetry.numSurfaceCoords-1] < settings->source.rbox2)
		fatal("Maximum altimetry range < maximum rbox range.\nAborting...");
	if(settings->batimetry.r[0] > settings->source.rbox1)
		fatal("Minimum batimetry range > minimum rbox range.\nAborting...");
	if(settings->batimetry.r[settings->batimetry.numSurfaceCoords-1] < settings->source.rbox2)
		fatal("Maximum batimetry range < maximum rbox range.\nAborting...");
	
}


