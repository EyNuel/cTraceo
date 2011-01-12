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

//prototype:
void readIn(globals_t*, const char*);

//actual function declaration:
void	readIn(globals_t* globals, const char* filename){
	/************************************************************************
	 *	Read a waveguide input file into global settings structure.			*
	 *	Input values:														*
	 *		filename	A string containg the file to be opened.			*
	 *																		*
	 *	Return value:														*
	 *		none		Values are read into "globals->settings"			*
	 ***********************************************************************/

	//int64_t		i, j, nthetas;
	FILE*		infile;		//a handle for the input file
	char *		junt;		//used for reading lines that should be discarded
	
	infile = openFile(filename, "r");	//open file in "read" mode

	if (VERBOSE)
		printf("Reading cTraceo input file \"%s\"", filename);

	/* Read the title */
	fgets(globals->settings->cTitle, MAX_LINE_LEN+1, infile);
	if (VERBOSE)
		printf("cTitle: %s\n", globals->settings->cTitle);
	
	//TODO Read the source info:
	
	//TODO Read altimetry info:
	
	//TODO Read sound speed info:
}

/*
       subroutine readin(ctitle,nthtas)

c      Include global (common) variables:

       include 'global.for'
       
       character*60 ctitle
       character*4  cclss
              
c***********************************************************************
c      Read source info:
c***********************************************************************

       read(inpfil,*) 
       read(inpfil,*) ds
       read(inpfil,*) rx,zx
       read(inpfil,*) rbox(1),rbox(2)
       read(inpfil,*) freqx
       read(inpfil,*) nthtas

c-----------------------------------------------------------------------
	  if (ds.eq.0.0) then
	      
          ds = abs(rbox(2)-rbox(1))/100.0

      end if
c-----------------------------------------------------------------------
	  if ((rx.lt.rbox(1)).or.(rx.gt.rbox(2))) then
	      
	      close(inpfil)
	      write(6,*) 'Source initial range is outside the range box!'
	      write(6,*) 'aborting calculations...'
	      stop

      end if
c-----------------------------------------------------------------------
	  if (nthtas.gt.nangle) then
	      
	      close(inpfil)
	      write(6,*) 'inpfil: NTHETAS > ',nangle,'!'
	      write(6,*) 'aborting calculations...'
	      stop
c-----------------------------------------------------------------------
	  elseif (nthtas.eq.2) then

	      read(inpfil,*) thetas(1),thetas(2)
	      dtheta = thetas(2)-thetas(1)
c-----------------------------------------------------------------------	      
	  else 

	      read(inpfil,*) thetas(1),thetas(nthtas)
	      dtheta = (thetas(nthtas)-thetas(1))/(nthtas-1)
	      do i = 2,nthtas-1
	      thetas(i) = thetas(1) + dtheta*(i-1)
	      end do

	  end if
	  
c***********************************************************************
c     Read altimetry info:
c***********************************************************************

       read(inpfil,*)
       read(inpfil,*)  atype
       read(inpfil,*) aptype
       read(inpfil,*) aitype
       read(inpfil,*) atiu
       read(inpfil,*) nati
       
c-----------------------------------------------------------------------
       if ((aitype.ne.'2P').and.(aitype.ne.'3P').and.(aitype.ne.'4P').
     & and.(aitype.ne.'FL').and.(aitype.ne.'SL')) then
      
          write(6,*) 'Surface interpolation:'
          write(6,*) 'unknown interpolation type,'
          write(6,*) 'aborting calculations...'
          stop
      
       end if
c-----------------------------------------------------------------------
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

