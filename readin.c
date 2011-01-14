/***********************************************************************
*	readIn.c
*	read the waveguide input file.
*	originally written by Orlando Camargo Rodriguez
*	Ported from FORTRAN by Emanuel Ey
***********************************************************************/
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
	 *	Read the title:
	 ***********************************************************************/
	fgets(globals->settings.cTitle, MAX_LINE_LEN+1, infile);


	/************************************************************************
	 *	Read and validate the source info:
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
		fatal(	"Source initial range is outside the range box!\nAborting...");
	}
	if( globals->settings.source.nThetas > MAX_NUM_ANGLES){
		fatal(	"Input file: nThetas > MAX_NUM_ANGLES.\nAborting...");
	}
	
	/*	Allocate memory for the launching angles	*/
	globals->settings.source.thetas = mallocDouble(nThetas);
	
	/*	Read the  thetas from the file	*/
	if(globals->settings.source.nThetas == 2){
		globals->settings.source.thetas[0] = readDouble(infile);
		globals->settings.source.thetas[1] = readDouble(infile);
		//DISCUSS:	from fortran file: "dtheta = thetas(2)-thetas(1)"
		//DISCUSS:	is dtheta needed elsewhere in code?
	}else{
		theta0 = readDouble(infile);
		thetaN = readDouble(infile);
		
		globals->settings.source.thetas[0] = theta0;
		globals->settings.source.thetas[nThetas - 1] = thetaN;
		dTheta =	(thetaN - theta0 ) / ( (double)nThetas - 1 );
		
		for(i=1;i <= nThetas-2; i++){
			globals->settings.source.thetas[i] = theta0 +dTheta *(i);
		}
	}
	

	/************************************************************************
	 * Read and validate altimetry info:
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
		fatal("Input file: unknown surface type.\nAborting...");
	}
	free(tempString);

	/* surfacePropertyType;		//formerly "aptype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"H") == 0){
		globals->settings.altimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__HOMOGENEOUS;
		
	}else if(strcmp(tempString,"N") == 0){
		globals->settings.altimetry.surfacePropertyType	= SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS;
		
	}else{
		fatal("Input file: unknown surface property type.\nAborting...");
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
		fatal("Input file: unknown surface interpolation type.\nAborting...");
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
		fatal("Input file: unknown surface attenuation units.\nAborting...");
	}
	free(tempString);

	/* numSurfaceCoords;		//formerly "nati" */
	numSurfaceCoords = (uint64_t)readInt(infile);
	globals->settings.altimetry.numSurfaceCoords = numSurfaceCoords;

	//malloc interface coords
	globals->settings.altimetry.r = mallocDouble(numSurfaceCoords);
	if(globals->settings.altimetry.r == NULL)
		fatal("Memory allocation error.");
	
	globals->settings.altimetry.z = mallocDouble(numSurfaceCoords);
	if(globals->settings.altimetry.z == NULL)
		fatal("Memory allocation error.");
	
	//read the surface properties and coordinates
	switch(globals->settings.altimetry.surfacePropertyType){
		case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:
			//malloc and read only one set of interface properties:
			globals->settings.altimetry.surfaceProperties = malloc(sizeof(interfaceProperties_t));
			if( globals->settings.altimetry.surfaceProperties == NULL)
				fatal("Memory allocation error.");
				
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
				fatal("Memory allocation error.");
				
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

	/*	Validate Altimetry info		*/
	if(numSurfaceCoords > NBDRY)
		fatal("Input file: altimetry points > NBDRY.\nAborting...");	//TODO replace NBDRY


	/************************************************************************
	 * Read and validate sound speed info:
	 ***********************************************************************/
	/*	sound speed distribution	"cdist"		*/
	skipLine(infile);
	tempString = readString(infile);
	if(strcmp(tempString,"c(z,z)") == 0){
		globals->settings.soundSpeed.cDist	= C_DIST__PROFILE;
	}else if(strcmp(tempString,"c(r,z)") == 0){
		globals->settings.soundSpeed.cDist	= C_DIST__FIELD;
	}else{
		fatal("Input file: unknown sound speed distribution type.\nAborting...");
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
		fatal("Input file: unknown sound class type.\nAborting...");
	}
	free(tempString);

	/* number of points in range and depth, "nr0,nz0" */
	nr0 = (uint64_t)readInt(infile);
	nz0 = (uint64_t)readInt(infile);
	globals->settings.soundSpeed.nr0 = nr0;
	globals->settings.soundSpeed.nz0 = nz0;

	//verify that number of values of soundspeed are within limits
	if(	(globals->settings.soundSpeed.cDist == C_DIST__PROFILE) &&
		(nz0 > NC0))
		fatal("Sound speed profile points > NC0!\nAborting...");	//TODO replace NC0

	if(	globals->settings.soundSpeed.cDist == C_DIST__FIELD){
		if(nr0 > NC02D)
			fatal("Sound speed field range points > NC02D.\nAborting...");	//TODO replace NC02D

		if(nz0 > NC02D)
			fatal("Sound speed field depth points > NC02D.\nAborting...");	//TODO replace NC02D
	}

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
						fatal("Analytical sound speed: z[1] == z[0] Only valid for Isovelocity and Munk Options!\nAborting...");

					if(globals->settings.soundSpeed.c01d[0] == globals->settings.soundSpeed.c01d[1])
						fatal("Analytical sound speed: c[1] == c[0] Only valid for Isovelocity option!\nAborting...");
				}
			}else if(globals->settings.soundSpeed.cClass == C_CLASS__TABULATED){
				//malloc z0 an c0:
				globals->settings.soundSpeed.z0 = mallocDouble(nz0);
				globals->settings.soundSpeed.c01d = mallocDouble(nz0);	//TODO verify this is correct

				//read pairs of z0 and c0
				for(i=0; i<globals->settings.soundSpeed.nz0; i++){
					globals->settings.soundSpeed.z0[i]	= readDouble(infile);
					globals->settings.soundSpeed.c01d[i]= readDouble(infile);
				}
			}
			break;
		
		case C_DIST__FIELD:
			if(globals->settings.soundSpeed.cClass != C_CLASS__TABULATED)
				fatal("Unknown sound speed filed type.\nAborting...");
			
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
			//read sound speeds
			for(j=0; j<nz0; j++){		//rows
				for(i=0; i<nr0; i++){	//columns
					globals->settings.soundSpeed.c02d[j][i] = readDouble(infile);
				}
			}
			break;
	}
	if (VERBOSE)
		printSettings(globals);
	
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

c***********************************************************************
c     Read bathymetry info:
c***********************************************************************

       read(inpfil,*)
       read(inpfil,*)  btype
       read(inpfil,*) bptype
       read(inpfil,*) bitype
       read(inpfil,*) btyu
       read(inpfil,*) nbty
       
c-----------------------------------------------------------------------
       if ((bitype.ne.'2P').and.(bitype.ne.'3P').and.(bitype.ne.'4P').
     & and.(bitype.ne.'FL').and.(bitype.ne.'SL')) then
      
          write(6,*) 'Bottom interpolation:'
          write(6,*) 'unknown interpolation type,'
          write(6,*) 'aborting calculations...'
          stop
      
       end if
c-----------------------------------------------------------------------
       if (nbty.gt.nbdry) then

           write(6,*) 'bathymetry points  > ',nbdry,'!'
	       write(6,*) 'aborting calculations...'
           stop
           
       end if
c-----------------------------------------------------------------------
       if ((btyu.ne.'F').and.(btyu.ne.'M').and.(btyu.ne.'N').
     & and.(btyu.ne.'Q').and.(btyu.ne.'W')) then
      
          write(6,*) 'Bottom attenuation: unknown units,'
          write(6,*) 'aborting calculations...'
          stop
      
       end if
c-----------------------------------------------------------------------
       if (bptype.eq.'H') then
	  
	   read(inpfil,*) cpbty(1), csbty(1), rhobty(1), 
     &                apbty(1), asbty(1)
	   
       do i = 1,nbty
          
	      read(inpfil,*) rbty(i), zbty(i)
          
       end do
       
       else if (bptype.eq.'N') then
	        
       do i = 1,nbty
          
	   read(inpfil,*) rbty(i),  zbty(i), cpbty(i), csbty(i),
     &                  rhobty(i), apbty(i), asbty(i)
          
       end do
       
       else
       
           write(6,*) 'inpfil: Unknown bottom properties,'
           write(6,*) 'aborting calculations...'
           stop

       end if

c***********************************************************************
c     Read array info:
c***********************************************************************

       read(inpfil,*)
       read(inpfil,*) artype
       read(inpfil,*) nra,nza

c-----------------------------------------------------------------------

c      Horizontal, Linear, Vertical or Rectangular Array:

       if (artype.eq.'HRY') then

	     if (nra.gt.nhyd) then

 	       close(inpfil)
 	       write(6,*) 'inpfil: horizontal array NHYD > ',nhyd,'!'
 	       write(6,*) 'aborting calculations...'
 	       stop 
         
         else
	     
	       read(inpfil,*) (rarray(i),i=1,nra)
	       read(inpfil,*)  zarray(1)

         end if
         
       elseif (artype.eq.'VRY') then

	     if (nza.gt.nhyd) then

 	       close(inpfil)
 	       write(6,*) 'inpfil: vertical array NHYD > ',nhyd,'!'
 	       write(6,*) 'aborting calculations...'
 	       stop
         
         else
	     
	       read(inpfil,*)  rarray(1)
	       read(inpfil,*) (zarray(i),i=1,nza)
         
         end if

       elseif (artype.eq.'LRY') then
         
         nra = max(nra,nza)
         nza = nra  
         
	     if (nra.gt.nhyd) then

 	       close(inpfil)
 	       write(6,*) 'inpfil: linear array NHYD > ',nhyd,'!'
 	       write(6,*) 'aborting calculations...'
 	       stop
         
         else
         
           read(inpfil,*) (rarray(i),i=1,nra)
           read(inpfil,*) (zarray(i),i=1,nra)          
         
         end if
         
       elseif (artype.eq.'RRY') then
         
         if (nra.gt.nhyd2) then

 	       close(inpfil)
 	       write(6,*) 'inpfil, rectangular array:'
 	       write(6,*) 'horizontal NHYD > ',nhyd2,'!'
 	       write(6,*) 'aborting calculations...'
 	       stop
         
         end if
	     
	 if (nza.gt.nhyd2) then

 	       close(inpfil)
 	       write(6,*) 'inpfil, rectangular array:'
 	       write(6,*) 'vertical NHYD > ',nhyd2,'!'
 	       write(6,*) 'aborting calculations...'
 	       stop
         
         else
         
           read(inpfil,*) (rarray(i),i=1,nra)
	   read(inpfil,*) (zarray(i),i=1,nza)
         
         end if
       
       else 
       
           write(6,*) 'inpfil: unknown array type,'
 	   write(6,*) 'aborting calculations...'
       end if 

c***********************************************************************
c     Read output info:
c***********************************************************************
       
       read(inpfil,*)
       read(inpfil,*) catype
       read(inpfil,*) miss
       
c***********************************************************************
c     Bathymetry/altimetry check:
c***********************************************************************

       if (rati(1).gt.rbox(1)) then
        write(6,*) 'inpfil: minimal altimetry range >'
        write(6,*) 'inpfil: minimal box       range!'
        write(6,*) 'aborting calculations...'
        stop
       end if 
       
       if (rati(nati).lt.rbox(2)) then
        write(6,*) 'inpfil: maximal altimetry range <'
        write(6,*) 'inpfil: maximal box       range!'
        write(6,*) 'aborting calculations...'
        stop
       end if 
     
       if (rbty(1).gt.rbox(1)) then
        write(6,*) 'inpfil: minimal bathymetry range >'
        write(6,*) 'inpfil: minimal box        range!'
        write(6,*) 'aborting calculations...'
        stop
       end if
       
       if (rbty(nbty).lt.rbox(2)) then
        write(6,*) 'inpfil: maximal bathymetry range <'
        write(6,*) 'inpfil: maximal box        range!'
        write(6,*) 'aborting calculations...'
        stop
       end if
       
c***********************************************************************
c     Back to main:
c***********************************************************************

      return
      
      end
*/

