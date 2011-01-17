/************************************************************************
*	readIn.c															*
*	read the waveguide input file.										*
*	originally written by 	Orlando Camargo Rodriguez:					*
*							Copyright (C) 2010							*
* 							Orlando Camargo Rodriguez					*
* 							orodrig@ualg.pt								*
*							Universidade do Algarve						*
*							Physics Department							*
*							Signal Processing Laboratory				*
*	Ported from FORTRAN by Emanuel Ey									*
************************************************************************/
#include <stdio.h>
#include <inttypes.h>		//contains definitions of integer data types that are inequivocal.
#include "tools.c"			
#include "globals.h"		//Include global variables
#include <math.h>

//prototype:
void readIn(globals_t*, const char*);

//actual function declaration:
void	readIn(globals_t* globals, const char* filename){
	/************************************************************************
	 *	Read a waveguide input file into global settings structure.			*
	 *	Input values:														*
	 * 		globals		A pointer to the "global" variables					*
	 *		filename	A string containg the file to be opened.			*
	 *																		*
	 *	Return value:														*
	 *		none		Values are read into "globals->settings"			*
	 ***********************************************************************/

	uint64_t	i,j;
	double		dTheta;				//DISCUSS: really necessary?
	uint64_t	nThetas;			//used locally to make code more readable. Value is stored in settings.
	double		theta0;				//used locally to make code more readable. Value is stored in settings.
	double		thetaN;				//used locally to make code more readable. Value is stored in settings.
	uint64_t	numSurfaceCoords;	//used locally to make code more readable. Value is stored in settings.
	uint64_t	nr0, nz0;			//used locally to make code more readable. Value is stored in settings.
	char*		tempString;
	FILE*		infile;					//a pointer for the input file
	infile = openFile(filename, "r");	//open file in "read" mode

	if (VERBOSE)
		printf("Reading cTraceo input file \"%s\"\n", filename);


	/************************************************************************
	 *	Read the title:														*
	 ***********************************************************************/
	fgets(globals->settings.cTitle, MAX_LINE_LEN+1, infile);


	/************************************************************************
	 *	Read and validate the source info:									*
	 ***********************************************************************/
	 skipLine(infile);
	 globals->settings.source.ds		= readDouble(infile);
	 globals->settings.source.rx		= readDouble(infile);
	 globals->settings.source.zx		= readDouble(infile);
	 globals->settings.source.rbox1		= readDouble(infile);
	 globals->settings.source.rbox2		= readDouble(infile);
	 globals->settings.source.freqx		= readDouble(infile);
	 nThetas = (uint64_t)readInt(infile);
	 globals->settings.source.nThetas	= nThetas;

	/*	Source validation	*/
	if(globals->settings.source.ds == 0.0 ){
		globals->settings.source.ds = fabs(	globals->settings.source.rbox2 -
											globals->settings.source.rbox1)/100;
	}
	if(	(globals->settings.source.rx < globals->settings.source.rbox1) ||
		(globals->settings.source.rx > globals->settings.source.rbox2)){
		fatal(	"Input file: Source: initial range is outside the range box!\nAborting...");
	}
	
	/*	Allocate memory for the launching angles	*/
	globals->settings.source.thetas = mallocDouble(nThetas);
	
	/*	Read the  thetas from the file	*/
	theta0 = readDouble(infile);
	thetaN = readDouble(infile);
	if(globals->settings.source.nThetas == 2){
		globals->settings.source.thetas[0] = theta0;
		globals->settings.source.thetas[1] = thetaN;
		dTheta = thetaN - theta0;
	}else{
		globals->settings.source.thetas[0] = theta0;
		globals->settings.source.thetas[nThetas - 1] = thetaN;
		dTheta =	(thetaN - theta0 ) / ( (double)nThetas - 1 );
		
		for(i=1;i <= nThetas-2; i++){
			globals->settings.source.thetas[i] = theta0 +dTheta *(double)(i);
		}
	}
	globals->settings.source.dTheta = dTheta;

	/************************************************************************
	 * Read and validate altimetry info:									*
	 ***********************************************************************/
	 
	skipLine(infile);

	/* surfaceType;	formerly "atype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"A") == 0){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__ABSORVENT;
		
	}else if(strcmp(tempString,"E") == 0){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__ELASTIC;
		
	}else if(strcmp(tempString,"R") == 0){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__RIGID;
		
	}else if(strcmp(tempString,"V") == 0){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__VACUUM;
		
	}else{
		fatal("Input file: altimetry: unknown surface type.\nAborting...");
	}
	free(tempString);

	/* surfacePropertyType;		//formerly "aptype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"H") == 0){
		globals->settings.altimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
		
	}else if(strcmp(tempString,"N") == 0){
		globals->settings.altimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
		
	}else{
		fatal("Input file: altimetry: unknown surface property type.\nAborting...");
	}
	free(tempString);

	/* surfaceInterpolation;	//formerly "aitype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"FL") == 0){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__FLAT;
		
	}else if(strcmp(tempString,"SL") == 0){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__SLOPED;
		
	}else if(strcmp(tempString,"2P") == 0){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__2P;
		
	}else if(strcmp(tempString,"3P") == 0){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__3P;
		
	}else if(strcmp(tempString,"4P") == 0){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__4P;
		
	}else{
		fatal("Input file: altimetry: unknown surface interpolation type.\nAborting...");
	}
	free(tempString);

	/* surfaceAttenUnits;		//formerly "atiu"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"F") == 0){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperkHz;
		
	}else if(strcmp(tempString,"M") == 0){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperMeter;
		
	}else if(strcmp(tempString,"N") == 0){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperNeper;
		
	}else if(strcmp(tempString,"Q") == 0){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__qFactor;
		
	}else if(strcmp(tempString,"W") == 0){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperLambda;
		
	}else{
		fatal("Input file: altimetry: unknown surface attenuation units.\nAborting...");
	}
	free(tempString);

	/* numSurfaceCoords;		//formerly "nati" */
	numSurfaceCoords = (uint64_t)readInt(infile);
	globals->settings.altimetry.numSurfaceCoords = numSurfaceCoords;

	//malloc interface coords
	globals->settings.altimetry.r = mallocDouble(numSurfaceCoords);
	globals->settings.altimetry.z = mallocDouble(numSurfaceCoords);
	
	//read the surface properties and coordinates
	switch(globals->settings.altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			//malloc and read only one set of interface properties:
			globals->settings.altimetry.surfaceProperties = malloc(sizeof(interfaceProperties_t));
			if( globals->settings.altimetry.surfaceProperties == NULL)
				fatal("Input file: altimetry: Memory allocation error.");
				
			globals->settings.altimetry.surfaceProperties[0].cp	= readDouble(infile);
			globals->settings.altimetry.surfaceProperties[0].cs	= readDouble(infile);
			globals->settings.altimetry.surfaceProperties[0].rho= readDouble(infile);
			globals->settings.altimetry.surfaceProperties[0].ap	= readDouble(infile);
			globals->settings.altimetry.surfaceProperties[0].as	= readDouble(infile);
			
			//read coordinates of interface points:
			for(i=0; i<numSurfaceCoords; i++){
				globals->settings.altimetry.r[i] = readDouble(infile);
				globals->settings.altimetry.z[i] = readDouble(infile);
			}
			break;
		
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			//Read coordinates and interface properties for all interface points:
			globals->settings.altimetry.surfaceProperties = malloc(numSurfaceCoords*sizeof(interfaceProperties_t));
			if( globals->settings.altimetry.surfaceProperties == NULL)
				fatal("Input file: altimetry: Memory allocation error.");
				
			for(i=0; i<numSurfaceCoords; i++){
				globals->settings.altimetry.r[i] = readDouble(infile);
				globals->settings.altimetry.z[i] = readDouble(infile);
				globals->settings.altimetry.surfaceProperties[i].cp	= readDouble(infile);
				globals->settings.altimetry.surfaceProperties[i].cs	= readDouble(infile);
				globals->settings.altimetry.surfaceProperties[i].rho= readDouble(infile);
				globals->settings.altimetry.surfaceProperties[i].ap	= readDouble(infile);
				globals->settings.altimetry.surfaceProperties[i].as	= readDouble(infile);
			}
			break;
	}

	/************************************************************************
	 * Read and validate sound speed info:									*
	 ***********************************************************************/
	/*	sound speed distribution	"cdist"		*/
	skipLine(infile);
	tempString = readString(infile);
	if(strcmp(tempString,"c(z,z)") == 0){
		globals->settings.soundSpeed.cDist	= C_DIST__PROFILE;
	}else if(strcmp(tempString,"c(r,z)") == 0){
		globals->settings.soundSpeed.cDist	= C_DIST__FIELD;
	}else{
		fatal("Input file: Sound Speed: unknown sound speed distribution type.\nAborting...");
	}
	free(tempString);

	/*	sound speed class	"cclass"		*/
	tempString = readString(infile);
	if(strcmp(tempString,"ISOV") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__ISOVELOCITY;
		
	}else if(strcmp(tempString,"LINP") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__LINEAR;
		
	}else if(strcmp(tempString,"PARP") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__PARABOLIC;
		
	}else if(strcmp(tempString,"EXPP") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__EXPONENTIAL;
		
	}else if(strcmp(tempString,"N2LP") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__N2_LINEAR;
		
	}else if(strcmp(tempString,"ISQP") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__INV_SQUARE;
		
	}else if(strcmp(tempString,"MUNK") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__MUNK;
		
	}else if(strcmp(tempString,"TABL") == 0){
		globals->settings.soundSpeed.cClass	= C_CLASS__TABULATED;
		
	}else{
		fatal("Input file: Sound Speed: unknown sound class type.\nAborting...");
	}
	free(tempString);

	/* number of points in range and depth, "nr0,nz0" */
	nr0 = (uint64_t)readInt(infile);
	nz0 = (uint64_t)readInt(infile);
	globals->settings.soundSpeed.nr0 = nr0;
	globals->settings.soundSpeed.nz0 = nz0;

	//read actual values of soundspeed profile/field:
	switch(globals->settings.soundSpeed.cDist){
		
		case C_DIST__PROFILE:
			if(globals->settings.soundSpeed.cClass != C_CLASS__TABULATED){
				//all cClasses execept for "TABULATED" only require ( z0(0),c0(0) ) and ( z0(1), c0(1) )
				//malloc z0 an c0:
				globals->settings.soundSpeed.z0 = mallocDouble(2);
				globals->settings.soundSpeed.c01d = mallocDouble(2);

				//read 4 values:
				globals->settings.soundSpeed.z0[0] = readDouble(infile);
				globals->settings.soundSpeed.c01d[0] = readDouble(infile);
				globals->settings.soundSpeed.z0[1] = readDouble(infile);
				globals->settings.soundSpeed.c01d[1] = readDouble(infile);

				// validate the values that were just read:
				if(	(globals->settings.soundSpeed.cClass != C_CLASS__ISOVELOCITY) &&
					(globals->settings.soundSpeed.cClass != C_CLASS__MUNK)){
					
					if(globals->settings.soundSpeed.z0[0] == globals->settings.soundSpeed.z0[1])
						fatal("Input file: Analytical sound speed: z[1] == z[0] Only valid for Isovelocity and Munk Options!\nAborting...");

					if(globals->settings.soundSpeed.c01d[0] == globals->settings.soundSpeed.c01d[1])
						fatal("Input file: Analytical sound speed: c[1] == c[0] Only valid for Isovelocity option!\nAborting...");
				}
			}else if(globals->settings.soundSpeed.cClass == C_CLASS__TABULATED){
				//malloc z0 an c0:
				globals->settings.soundSpeed.z0 = mallocDouble(nz0);
				globals->settings.soundSpeed.c01d = mallocDouble(nz0);

				//read pairs of z0 and c0
				for(i=0; i<globals->settings.soundSpeed.nz0; i++){
					globals->settings.soundSpeed.z0[i]	= readDouble(infile);
					globals->settings.soundSpeed.c01d[i]= readDouble(infile);
				}
			}
			break;
		
		case C_DIST__FIELD:
			if(globals->settings.soundSpeed.cClass != C_CLASS__TABULATED)
				fatal("Input file: Unknown sound speed field type.\nAborting...");
			
			//malloc ranges (vector)
			globals->settings.soundSpeed.r0 = mallocDouble(nr0);
			//read ranges
			for(i=0; i<nr0; i++)
				globals->settings.soundSpeed.r0[i] = readDouble(infile);

			//malloc depths (vector)
			globals->settings.soundSpeed.z0 = mallocDouble(nz0);
			//read depths
			for(i=0; i<nz0; i++)
				globals->settings.soundSpeed.z0[i] = readDouble(infile);

			//malloc sound speeds (2 dim matrix)
			globals->settings.soundSpeed.c02d = mallocDouble2D(nr0, nz0);	//mallocDouble2D(numCols, numRows)
			//read actual sound speeds
			for(j=0; j<nz0; j++){		//rows
				for(i=0; i<nr0; i++){	//columns
					globals->settings.soundSpeed.c02d[j][i] = readDouble(infile);
				}
			}
			break;
	}

	/************************************************************************
	 * Read and validate object info:										*
	 ***********************************************************************/
	//TODO complete object info section

	skipLine(infile);
	globals->settings.object.numObjects = (uint64_t)readInt(infile);
	 
	
	/************************************************************************
	 * Read and validate batimetry info:									*
	 ***********************************************************************/
	skipLine(infile);

	/* surfaceType;	formerly "atype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"A") == 0){
		globals->settings.batimetry.surfaceType	= SURFACE_TYPE__ABSORVENT;
		
	}else if(strcmp(tempString,"E") == 0){
		globals->settings.batimetry.surfaceType	= SURFACE_TYPE__ELASTIC;
		
	}else if(strcmp(tempString,"R") == 0){
		globals->settings.batimetry.surfaceType	= SURFACE_TYPE__RIGID;
		
	}else if(strcmp(tempString,"V") == 0){
		globals->settings.batimetry.surfaceType	= SURFACE_TYPE__VACUUM;
		
	}else{
		fatal("Input file: batimetry: unknown surface type.\nAborting...");
	}
	free(tempString);

	/* surfacePropertyType;		//formerly "aptype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"H") == 0){
		globals->settings.batimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
		
	}else if(strcmp(tempString,"N") == 0){
		globals->settings.batimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
		
	}else{
		fatal("Input file: batimetry: unknown surface property type.\nAborting...");
	}
	free(tempString);

	/* surfaceInterpolation;	//formerly "aitype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"FL") == 0){
		globals->settings.batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__FLAT;
		
	}else if(strcmp(tempString,"SL") == 0){
		globals->settings.batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__SLOPED;
		
	}else if(strcmp(tempString,"2P") == 0){
		globals->settings.batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__2P;
		
	}else if(strcmp(tempString,"3P") == 0){
		globals->settings.batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__3P;
		
	}else if(strcmp(tempString,"4P") == 0){
		globals->settings.batimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__4P;
		
	}else{
		fatal("Input file: batimetry: unknown surface interpolation type.\nAborting...");
	}
	free(tempString);

	/* surfaceAttenUnits;		//formerly "atiu"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"F") == 0){
		globals->settings.batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperkHz;
		
	}else if(strcmp(tempString,"M") == 0){
		globals->settings.batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperMeter;
		
	}else if(strcmp(tempString,"N") == 0){
		globals->settings.batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperNeper;
		
	}else if(strcmp(tempString,"Q") == 0){
		globals->settings.batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__qFactor;
		
	}else if(strcmp(tempString,"W") == 0){
		globals->settings.batimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperLambda;
		
	}else{
		fatal("Input file: batimetry: unknown surface attenuation units.\nAborting...");
	}
	free(tempString);

	/* numSurfaceCoords;		//formerly "nati" */
	numSurfaceCoords = (uint64_t)readInt(infile);
	globals->settings.batimetry.numSurfaceCoords = numSurfaceCoords;

	//malloc interface coords
	globals->settings.batimetry.r = mallocDouble(numSurfaceCoords);
	globals->settings.batimetry.z = mallocDouble(numSurfaceCoords);
	
	//read the surface properties and coordinates
	switch(globals->settings.batimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			//malloc and read only one set of interface properties:
			globals->settings.batimetry.surfaceProperties = malloc(sizeof(interfaceProperties_t));
			if( globals->settings.batimetry.surfaceProperties == NULL)
				fatal("Memory allocation error.");
				
			globals->settings.batimetry.surfaceProperties[0].cp	= readDouble(infile);
			globals->settings.batimetry.surfaceProperties[0].cs	= readDouble(infile);
			globals->settings.batimetry.surfaceProperties[0].rho= readDouble(infile);
			globals->settings.batimetry.surfaceProperties[0].ap	= readDouble(infile);
			globals->settings.batimetry.surfaceProperties[0].as	= readDouble(infile);
			
			//read coordinates of interface points:
			for(i=0; i<numSurfaceCoords; i++){
				globals->settings.batimetry.r[i] = readDouble(infile);
				globals->settings.batimetry.z[i] = readDouble(infile);
			}
			break;
		
		case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:
			//Read coordinates and interface properties for all interface points:
			globals->settings.batimetry.surfaceProperties = malloc(numSurfaceCoords*sizeof(interfaceProperties_t));
			if( globals->settings.batimetry.surfaceProperties == NULL)
				fatal("Memory allocation error.");
				
			for(i=0; i<numSurfaceCoords; i++){
				globals->settings.batimetry.r[i] = readDouble(infile);
				globals->settings.batimetry.z[i] = readDouble(infile);
				globals->settings.batimetry.surfaceProperties[i].cp	= readDouble(infile);
				globals->settings.batimetry.surfaceProperties[i].cs	= readDouble(infile);
				globals->settings.batimetry.surfaceProperties[i].rho= readDouble(infile);
				globals->settings.batimetry.surfaceProperties[i].ap	= readDouble(infile);
				globals->settings.batimetry.surfaceProperties[i].as	= readDouble(infile);
			}
			break;
	}
	
	/************************************************************************
	 * Read and validate array info:										*
	 ***********************************************************************/
	skipLine(infile);
	/*	output array type "artype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"RRY") == 0){
		globals->settings.output.arrayType	= ARRAY_TYPE__RECTANGULAR;
		
	}else if(strcmp(tempString,"HRY") == 0){
		globals->settings.output.arrayType	= ARRAY_TYPE__HORIZONTAL;
		
	}else if(strcmp(tempString,"VRY") == 0){
		globals->settings.output.arrayType	= ARRAY_TYPE__VERTICAL;
		
	}else if(strcmp(tempString,"LRY") == 0){
		globals->settings.output.arrayType	= ARRAY_TYPE__LINEAR;
		
	}else{
		fatal("Input file: output: unknown array type.\nAborting...");
	}
	free(tempString);
	
	/*	output array dimensions "nra, nrz"		*/
	globals->settings.output.nArrayR = (uint64_t)readInt(infile);
	globals->settings.output.nArrayZ = (uint64_t)readInt(infile);

	globals->settings.output.arrayR = mallocDouble(globals->settings.output.nArrayR);
	globals->settings.output.arrayZ = mallocDouble(globals->settings.output.nArrayZ);
	if(	(globals->settings.output.arrayR == NULL) || (globals->settings.output.arrayZ == NULL))
		fatal("Memory allocation error.");

	//read the actual array values
	for(i=0; i<globals->settings.output.nArrayR; i++){
		globals->settings.output.arrayR[i] = readDouble(infile);
	}
	for(i=0; i<globals->settings.output.nArrayZ; i++){
		globals->settings.output.arrayZ[i] = readDouble(infile);
	}

	/************************************************************************
	 * Read and validate output info:
	 ***********************************************************************/
	skipLine(infile);
	
	/*	output calculation type "catype"	*/
	tempString = readStringN(infile,6);
	if(strcmp(tempString,"'RCO'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__RAY_COORDS;
		
	}else if(strcmp(tempString,"'ARI'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__ALL_RAY_INFO;
		
	}else if(strcmp(tempString,"'ERF'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__EIGENRAYS_REG_FALSI;
		
	}else if(strcmp(tempString,"'EPR'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__EIGENRAYS_PROXIMITY;
		
	}else if(strcmp(tempString,"'ADR'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__AMP_DELAY_REG_FALSI;
		
	}else if(strcmp(tempString,"'ADP'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__AMP_DELAY_PROXIMITY;
		
	}else if(strcmp(tempString,"'CPR'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__COH_ACOUS_PRESS;
		
	}else if(strcmp(tempString,"'CTL'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__COH_TRANS_LOSS;
		
	}else if(strcmp(tempString,"'PVL'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__COH_PART_VEL;
		
	}else if(strcmp(tempString,"'PAV'") == 0){
		globals->settings.output.calcType	= CALC_TYPE__COH_ACOUS_PRESS_PART_VEL;
		
	}else{

		fatal("Input file: unknown output calculation type.\nAborting...");
	}
	free(tempString);

	/*	output calculation type "catype"	*/
	globals->settings.output.miss = readDouble(infile);

	/* Check batimetry/altimetry	*/
	if(globals->settings.altimetry.r[0] < globals->settings.source.rbox1)
		fatal("Minimum altimetry range > minimum rbox range.\nAborting...");
	if(globals->settings.altimetry.r[globals->settings.altimetry.numSurfaceCoords-1] < globals->settings.source.rbox2)
		fatal("Maximum altimetry range < maximum rbox range.\nAborting...");
	if(globals->settings.batimetry.r[0] < globals->settings.source.rbox1)
		fatal("Minimum batimetry range > minimum rbox range.\nAborting...");
	if(globals->settings.batimetry.r[globals->settings.batimetry.numSurfaceCoords-1] < globals->settings.source.rbox2)
		fatal("Maximum batimetry range < maximum rbox range.\nAborting...");
	
}
/*
c***********************************************************************
c     Read object info:
c***********************************************************************

       read(inpfil,*) 
       read(inpfil,*) nobj

       if (nobj.gt.nmaxo) then
        write(6,*) 'inpfil: # of objects >',nmaxo,'!'
        write(6,*) 'aborting calculations...'
        stop 
       end if 
       
       if (nobj.gt.0) then

       read(inpfil,*) oitype

       do i = 1,nobj

          read(inpfil,*)  otype(i:i)
          read(inpfil,*)   obju(i:i)
          read(inpfil,*) no(i)  
          
          if (no(i).gt.npo) then 
           write(6,*) 'Object depth points > ',npo,'!'
           write(6,*) 'aborting calculations...'
           stop
          end if
       
          read(inpfil,*) ocp(i), ocs(i), orho(i), oap(i), oas(i)
          
          do j = 1,no(i)
             
             read(inpfil,*) ro(i,j),zdn(i,j),zup(i,j)
             
          end do 
       
       end do
       
       end if
*/

