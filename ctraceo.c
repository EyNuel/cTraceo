/************************************************************************
 * ctraceo.c															*
 * Main ctraceo file													*
 *	readIn.c															*
 *	read the waveguide input file.										*
 *	originally written in FORTRAN by:									*
 *  									Orlando Camargo Rodriguez:		*
 *										Copyright (C) 2010				*
 * 										Orlando Camargo Rodriguez		*
 *										orodrig@ualg.pt					*
 *										Universidade do Algarve			*
 *										Physics Department				*
 *										Signal Processing Laboratory	*
 *																		*
 *	Ported to C by:		Emanuel Ey										*
 *						emanuel.ey@gmail.com							*
 *						Signal Processing Laboratory					*
 *						Universidade do Algarve							*
 *																		*
 ************************************************************************/
#include <stdio.h>
#include "globals.h"
#include "readin.c"


int main(int argc, char **argv)
{
	const char*	   infile = NULL;
	globals_t*	globals = NULL;
	globals = mallocGlobals();

	// check if a command line argument was passed:
	if(argc == 2){
		//if so, try to use it as an inout file
		infile = argv[1];
	}else{
		//otherwise, open the standard file.
		infile = "munk.in";
	}
		
	if (VERBOSE)
		printf("Running cTraceo in verbose mode.\n\n");
	readIn(globals, infile);

	if (VERBOSE)
		printSettings(globals);
	return 0;
}
/*
		 program traceo

c***********************************************************************
c
c		TRACEO Ray tracing program
c		Written by Orlando Camargo Rodriguez
C		Copyright (C) 2010 Orlando Camargo Rodriguez
c		orodrig@ualg.pt
c		Universidade do Algarve
c		Physics Department
c		Signal Processing Laboratory
c		Faro, 10/01/2011 at 16:00
c
c***********************************************************************
c		Commands should be placed in columns 6-72!
c****&******************************************************************
c
c		GNU license: 
c
c		This program is free software: you can redistribute it and/or modify
c		it under the terms of the GNU General Public License as published by
c		the Free Software Foundation, either version 3 of the License, or
c		(at your option) any later version.
c
c		This program is distributed in the hope that it will be useful,
c		but WITHOUT ANY WARRANTY; without even the implied warranty of
c		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
c		GNU General Public License for more details.
c
c		See <http://www.gnu.org/licenses/> to obtain a copy of the 
c		GNU General Public License.

c***********************************************************************
c		Version interfaced with Matlab Engine
c****&******************************************************************
c
c***********************************************************************
c		Let's go:
c****&******************************************************************

c		Include global (common) variables:

		 include 'global.for'
		 
c		Define local variables:
		 
		 character*60 ctitle
		 character*47 lnglne
		 character*8	filnme,lognme
		 
		 integer*8 irefl(np)
		 
		 integer*8 i,j,nthtas,imax
		 
		 real*8 thetai
		 real*8 timei,timef
		 
c-----------------------------------------------------------------------

		 call cpu_time(timei)
		 
		 lnglne = '-----------------------------------------------'
		 
		 lognme = 'LOGFIL'
c-----------------------------------------------------------------------
		 
		 omega = 2.0*pi*freqx
	
		 open(unit=prtfil,file=lognme,status='new')
		
		 write(prtfil,*) 'TRACEO ray tracing program'
		 write(prtfil,*) 'Written by Orlando Camargo Rodriguez'
		 write(prtfil,*) 'SiPLAB laboratory'
		 write(prtfil,*) lnglne

c***********************************************************************
c		READ the waveguide INput file:
c***********************************************************************
 
		 write(prtfil,*) 'INPUT:'
		 
		 call readin(ctitle,nthtas)

c***********************************************************************
c		Prepare the output file:
c***********************************************************************

		 write(prtfil,*) ctitle
		 write(prtfil,*) lnglne
		 write(prtfil,*) 'OUTPUT:'
		 
		 if (catype.eq.'RCO') then

			 write(prtfil,*) 'Ray coordinates'

			 call calrco(ctitle,nthtas)

		 else if (catype.eq.'ARI') then

			 write(prtfil,*) 'Ray information'

			 call calari(ctitle,nthtas)

		 else if (catype.eq.'EPR') then

			 write(prtfil,*) 'Eigenrays (by PRoximity)'

			 call calepr(ctitle,nthtas)
		 
		 else if (catype.eq.'ERF') then

			 write(prtfil,*) 'Eigenrays (by Regula Falsi)'

			 call calerf(ctitle,nthtas)

		 else if (catype.eq.'ADP') then

			 write(prtfil,*) 'Amplitudes and Delays (by Proximity)'

			 call caladp(ctitle,nthtas)

		 else if (catype.eq.'ADR') then

			 write(prtfil,*) 'Amplitudes and Delays (by Regula falsi)'

			 call caladr(ctitle,nthtas)

		 else if (catype.eq.'CPR') then

			 write(prtfil,*) 'Coherent acoustic pressure'

			 call calcpr(ctitle,nthtas)
			 
		 else if (catype.eq.'CTL') then

			 write(prtfil,*) 'Coherent transmission loss'

			 call calctl(ctitle,nthtas)

		 else if (catype.eq.'PVL') then

			 write(prtfil,*) 'Particle velocity'

			 call calpvl(ctitle,nthtas)

		else if (catype.eq.'PAV') then

			 write(prtfil,*) 'Pressure and particle velocity'
 
			 call calpav(ctitle,nthtas)

		 else

			 write(prtfil,*) 'Unknown output option,'
		 write(prtfil,*) 'aborting calculations...'
			 stop

		 end if
		 
		 write(prtfil,*) lnglne
		 
		 call cpu_time(timef)
		 
		 write(prtfil,*) 'done.'
		 write(prtfil,*) ' '
		 write(prtfil,*) 'CPU time:', timef-timei, ' seconds'
		 
		 close(prtfil)
		 
c***********************************************************************

5000	 format(i6)

c***********************************************************************
c		C'est fini!
c***********************************************************************
		
		 end
*/
