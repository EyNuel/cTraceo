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
#include "globals.c"
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

	uint64_t	i;
	double		dTheta;		//DISCUSS: really necessary?
	uint64_t	nThetas;	//used locally to make code more readable. Value is stored in settings.
	double		theta0;		//used locally to make code more readable. Value is stored in settings.
	double		thetaN;		//used locally to make code more readable. Value is stored in settings.
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
		fatal(	"Input file: NTHETAS > MAX_NUM_ANGLES.\nAborting...");
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
	 * Read altimetry info:
	 ***********************************************************************/
	 
	skipLine(infile);

	/* surfaceType;	formerly "atype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"A")){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__ABSORVENT;
	}else if(strcmp(tempString,"E")){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__ELASTIC;
	}else if(strcmp(tempString,"R")){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__RIGID;
	}else if(strcmp(tempString,"V")){
		globals->settings.altimetry.surfaceType	= SURFACE_TYPE__VACUUM;
	}else{
		fatal("Input file: unknown surface type.\nAborting...");
	}
	free(tempString);

	/* surfaceProperties;		//formerly "aptype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"H")){
		globals->settings.altimetry.surfaceProperties	= SURFACE_PROPERTIES__HOMOGENEOUS;
	}else if(strcmp(tempString,"N")){
		globals->settings.altimetry.surfaceProperties	= SURFACE_PROPERTIES__NON_HOMOGENEOUS;
	}else{
		fatal("Input file: unknown surface properties.\nAborting...");
	}
	free(tempString);

	/* surfaceInterpolation;	//formerly "aitype"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"FL")){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__FLAT;
	}else if(strcmp(tempString,"SL")){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__SLOPED;
	}else if(strcmp(tempString,"2P")){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__2P;
	}else if(strcmp(tempString,"3P")){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__3P;
	}else if(strcmp(tempString,"4P")){
		globals->settings.altimetry.surfaceInterpolation	= SURFACE_INTERPOLATION__4P;
	}else{
		fatal("Input file: unknown surface interpolation type.\nAborting...");
	}
	free(tempString);

	/* surfaceAttenUnits;		//formerly "atiu"	*/
	tempString = readString(infile);
	if(strcmp(tempString,"F")){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperkHz;
	}else if(strcmp(tempString,"M")){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperMeter;
	}else if(strcmp(tempString,"N")){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperNeper;
	}else if(strcmp(tempString,"Q")){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__qFactor;
	}else if(strcmp(tempString,"W")){
		globals->settings.altimetry.surfaceAttenUnits	= SURFACE_ATTEN_UNITS__dBperLambda;
	}else{
		fatal("Input file: unknown surface attenuation units.\nAborting...");
	}
	free(tempString);

	/* numSurfaceCoords;		//formerly "nati" */
	globals->settings.altimetry.numSurfaceCoords = readInt(infile);

	
	switch(globals->settings.altimetry.surfaceProperties){
		case SURFACE_PROPERTIES__HOMOGENEOUS:
			//Read only one set of interface properties: 
			//TODO continue here (see page 39 of manual)
			//Read coordinates of interface points:
			
			break;
		case SURFACE_PROPERTIES__NON_HOMOGENEOUS:
			//Read coordinates and interface properties for all interface points:
			
			break;
	}

	if (VERBOSE)
		printSettings(globals);
}
/*
       if (nati.gt.nbdry) then

           write(6,*) 'altimetry points  > ',nbdry,'!'
	   write(6,*) 'aborting calculations...'
           stop
           
       end if
c-----------------------------------------------------------------------
       if ((atiu.ne.'F').and.(atiu.ne.'M').and.(atiu.ne.'N').
     & and.(atiu.ne.'Q').and.(atiu.ne.'W')) then
      
          write(6,*) 'Surface attenuation: unknown units,'
          write(6,*) 'aborting calculations...'
          stop
      
       end if
c-----------------------------------------------------------------------

       if (aptype.eq.'H') then
	  
       read(inpfil,*) cpati(1),csati(1),rhoati(1),apati(1),asati(1)
	   
       do i = 1,nati
          
	  read(inpfil,*) rati(i), zati(i)
	      
       end do
       
       else if (aptype.eq.'N') then
	        
       do i = 1,nati
          
	  read(inpfil,*) rati(i),  zati(i), cpati(i), csati(i),
     &                 rhoati(i), apati(i), asati(i)
          
       end do
		
       else
       
	   write(6,*) 'inpfil: Unknown surface properties,'
           write(6,*) 'aborting calculations...'
	   stop

       end if

c***********************************************************************
c     Read sound speed info:
c***********************************************************************

       read(inpfil,*)
       read(inpfil,*) cdist
       read(inpfil,*) cclass
       read(inpfil,*) nr0,nz0
       
       cclss = cclass(1:4)
       
       if ((cdist.eq.'c(z,z)').and.(nz0.gt.nc0)) then
	
           write(6,*) 'Sound speed profile points > ',nc0,'!'
           write(6,*) 'aborting calculations...'
           stop
 
       end if
	     
       if (cdist.eq.'c(r,z)') then
 
            if (nr0.gt.nc02d) then
             write(6,*) 'Sound speed field range points > ',nc02d,'!'
             write(6,*) 'aborting calculations...'
             stop
            end if
 
            if (nz0.gt.nc02d) then
             write(6,*) 'Sound speed field depth points > ',nc02d,'!'
             write(6,*) 'aborting calculations...'
             stop
            end if
 
       end if
c-----------------------------------------------------------------------

c      Sound speed profile case:

       if (cdist.eq.'c(z,z)') then

c#######################################################################
        if ((cclss.ne.'TABL')) then
	    
	     read(inpfil,*) z0(1), c0(1)
	     read(inpfil,*) z0(2), c0(2)
	       
	     if ((cclss.ne.'ISOV').and.(cclss.ne.'MUNK')) then
	
 	       if ( z0(1).eq.z0(2) ) then
 	     	write(6,*) 'Analytical sound speed: z(2) = z(1)'
	     	write(6,*) 'valid only for isovelocity and!'
	     	write(6,*) 'Munk options!'
 	     	stop
 	       end if

	       if ( c0(1).eq.c0(2) ) then
 	     	write(6,*) 'Analytical sound speed: c(2) = c(1)'
	     	write(6,*) 'valid only for isovelocity option!'
 	     	stop
	       end if

	     end if

c#######################################################################
	     else if (cclss.eq.'TABL') then
	
	       do i = 1,nz0
 
	          read(inpfil,*) z0(i), c0(i)
 
	       end do

c#######################################################################
	     else
	   
	      write(6,*) 'inpfil: Unknown sound speed profile type,'
	      write(6,*) 'aborting calculations...'
              stop
	    
	     end if
c#######################################################################

c      Sound speed field on a rectangular grid:    

       else if (cdist.eq.'c(r,z)') then

c#######################################################################
	    if (cclss.eq.'TABL') then
	   
c           Read the vector of ranges: 
  
	        read(inpfil,*) (r0(i),i=1,nr0)
		   
c           Read the vector of depths:
	   
	        read(inpfil,*) (z0(i),i=1,nz0)

c           Read the matrix of sound speed: 
      
 	        do i = 1,nz0

                read(inpfil,*) (c02d(i,j), j = 1,nr0)

 	        end do

c#######################################################################
	       else 
	       
	        write(6,*) 'inpfil: Unknown sound speed field type,'
	        write(6,*) 'aborting calculations...'
 	        stop

	       end if
c#######################################################################

       else
       
           write(6,*) 'inpfil: Unknown sound speed distribution,'
	   write(6,*) 'aborting calculations...'
           stop
       
       end if

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

