/********************************************************************************
 *	calcParticleVel.c	 														*
 * 	(formerly "calpvl.for")														*
 *	Calculates particle velocity from coherent acoustic pressure.				*
 *																				*
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
 * 				settings:	Pointer to structure containing all input info.		*
 * 	Outputs:																	*
 * 				none:		Writes to file "pvl.mat".							*
 * 																				*
 *******************************************************************************/

#include "globals.h"
#include <mat.h>
#include <matrix.h>

void calcParticleVel(settings_t*);

void calcParticleVel(settings_t* settings){
	MATFile*		matfile	= NULL;
	mxArray*		pTitle	= NULL;
	uintptr_t		j, k;
	double			rHyd, zHyd;
	
/*
c	  Include global (common) variables:

include 'global.for'

c	  Define local variables:

character*60 ctitle

integer*8 irefl(np),jbdry(np)
integer*8 i,ih,ii,imax,j,jn,k,nthtas

integer*8 matOpen, matClose
integer*8 mxCreateDoubleMatrix, mxCreateString, mxGetPr
integer*8 mp, ptitle, pthtas, prh, pzh
integer*8 puu, pww, pru2d, piu2d, prw2d, piw2d
integer*8 matPutVariable, matDeleteVariable
integer*8 status

real*8 ru2d(nhyd2,nhyd2),rw2d(nhyd2,nhyd2)
real*8 iu2d(nhyd2,nhyd2),iw2d(nhyd2,nhyd2)
real*8 uu(2,np),ww(2,np)
real*8 tbdry(2,np)
real*8 xp(3)
real*8 es(2),e1(2),deltar(2)

real*8 thetai,ctheta
real*8 rh,zh,n,nxn,dzdr,qray
real*8 sray,dr,dz,q0,w
real*8 drarray,dzarray

complex*8 dpdr2d(nhyd2,nhyd2),dpdz2d(nhyd2,nhyd2)
complex*8   pl2d(nhyd2,nhyd2),  pc2d(nhyd2,nhyd2)
complex*8   pr2d(nhyd2,nhyd2)
complex*8   pu2d(nhyd2,nhyd2),  pd2d(nhyd2,nhyd2)

complex*8   dpdr(np),  dpdz(np)
complex*8 pressl(np),pressc(np),pressr(np),pressu(np),pressd(np)
complex*8  decay(np)
complex*8 zp(3)

complex*8 aray,pc,pl,pr,pu,pd
complex*8 dpdri,dpdzi
complex*8 dummi

c***********************************************************************

c***********************************************************************
*/

	matfile		= matOpen("pvl.mat", "u");		//open file in "update" mode

	//write title to matfile:
	pTitle = mxCreateString("TRACEO: Coherent Acoustic Pressure");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);

	/*
	 *	Horizontal and vertical pressure components where calculated in calcCohAcoustpress.
	 *	We can now use this to calculate the actual particle velocity components:
	 */

	switch(settings->output.arrayType){
		case ARRAY_TYPE__HORIZONTAL:
			/* TODO
			zh = zarray(1)
			do j = 1,nra
				rh = rarray(j)

				xp(1) = rh - drarray
				xp(2) = rh
				xp(3) = rh + drarray

				zp(1) = pressl(j)
				zp(2) = pressc(j)
				zp(3) = pressr(j)

				call cbpa1d(xp,zp,rh,dummi,dpdri,dummi)

				dpdr(j) = -imunit*dpdri

				xp(1) = zh - dzarray
				xp(2) = zh
				xp(3) = zh + dzarray

				zp(1) = pressd(j)
				zp(3) = pressu(j)

				call cbpa1d(xp,zp,zh,dummi,dpdzi,dummi)

				dpdz(j) = -imunit*dpdzi
			end do
			*/
			break;
		case ARRAY_TYPE__VERTICAL:
			/* TODO
			rh = rarray(1)
			do j = 1,nza
				zh = zarray(j)

				xp(1) = rh - drarray
				xp(2) = rh
				xp(3) = rh + drarray

				zp(1) = pressl(j)
				zp(2) = pressc(j)
				zp(3) = pressr(j)

				call cbpa1d(xp,zp,rh,dummi,dpdri,dummi)

				dpdr(j) = -imunit*dpdri

				xp(1) = zh - dzarray
				xp(2) = zh
				xp(3) = zh + dzarray

				zp(1) = pressd(j)
				zp(3) = pressu(j)

				call cbpa1d(xp,zp,zh,dummi,dpdzi,dummi)

				dpdz(j) = -imunit*dpdzi
			end do
			*/
			break;
		case ARRAY_TYPE__LINEAR:
			/* TODO
			do j = 1,nra
				rh = rarray(j)
				zh = zarray(j)

				xp(1) = rh - drarray
				xp(2) = rh
				xp(3) = rh + drarray

				zp(1) = pressl(j)
				zp(2) = pressc(j)
				zp(3) = pressr(j)

				call cbpa1d(xp,zp,rh,dummi,dpdri,dummi)

				dpdr(j) = -imunit*dpdri

				xp(1) = zh - dzarray
				xp(2) = zh
				xp(3) = zh + dzarray

				zp(1) = pressd(j)
				zp(3) = pressu(j)

				call cbpa1d(xp,zp,zh,dummi,dpdzi,dummi)

				dpdz(j) = -imunit*dpdzi
			end do
			*/
			break;
		case ARRAY_TYPE__RECTANGULAR:
			for(j=0; j<settings->output.nArrayR){
				//TODO invert for-loops to improove performance (by looping over rightmost element of array)
				rHyd
			rh = rarray(j)
				do k = 1,nza
					zh = zarray(k)
					xp(1) = rh - drarray
					xp(2) = rh
					xp(3) = rh + drarray

					zp(1) = pl2d(k,j)
					zp(2) = pc2d(k,j)
					zp(3) = pr2d(k,j)

					call cbpa1d(xp,zp,rh,dummi,dpdri,dummi)

					dpdr2d(k,j) = -imunit*dpdri

					xp(1) = zh - dzarray
					xp(2) = zh
					xp(3) = zh + dzarray

					zp(1) = pd2d(k,j)
					zp(3) = pu2d(k,j)

					call cbpa1d(xp,zp,zh,dummi,dpdzi,dummi)

					dpdz2d(k,j) = -imunit*dpdzi
				end do
			end do
			break;
	}

	c***********************************************************************
	c	  Write the data to the output file:
	c***********************************************************************
	if (artype.ne.'RRY') then
		jj = max(nra,nza)

		do j = 1,jj
			uu(1,j) = realpart( dpdr(j) )
			uu(2,j) = imagpart( dpdr(j) )
			ww(1,j) = realpart( dpdz(j) )
			ww(2,j) = imagpart( dpdz(j) )
		end do

		puu = mxCreateDoubleMatrix(2,jj,0)
		call mxCopyReal8ToPtr(uu,mxGetPr(puu),2*jj)
		status = matPutVariable(mp,'u',puu)
		call mxDestroyArray(puu)

		pww = mxCreateDoubleMatrix(2,jj,0)
		call mxCopyReal8ToPtr(ww,mxGetPr(pww),2*jj)
		status = matPutVariable(mp,'w',pww)
		call mxDestroyArray(pww)
	else
		//I'm having problems passing complex matrices to Matlab; therefore I will save separately the real and complex parts:
		do i = 1,nza
			do j = 1,nra
				ru2d(i,j) = realpart( dpdr2d(i,j) )
				iu2d(i,j) = imagpart( dpdr2d(i,j) )
				rw2d(i,j) = realpart( dpdz2d(i,j) )
				iw2d(i,j) = imagpart( dpdz2d(i,j) )
			end do
		end do 

		pru2d = mxCreateDoubleMatrix(nza,nra,0)
		call mxCopyReal8ToPtr(ru2d(1:nza,1:nra), mxGetPr(pru2d),nra*nza)
		status = matPutVariable(mp,'ru',pru2d)
		call mxDestroyArray(pru2d)

		piu2d = mxCreateDoubleMatrix(nza,nra,0)
		call mxCopyReal8ToPtr(iu2d(1:nza,1:nra), mxGetPr(piu2d),nra*nza)
		status = matPutVariable(mp,'iu',piu2d)
		call mxDestroyArray(piu2d)

		prw2d = mxCreateDoubleMatrix(nza,nra,0)
		call mxCopyReal8ToPtr(rw2d(1:nza,1:nra), mxGetPr(prw2d),nra*nza)
		status = matPutVariable(mp,'rw',prw2d)
		call mxDestroyArray(prw2d)

		piw2d = mxCreateDoubleMatrix(nza,nra,0)
		call mxCopyReal8ToPtr(iw2d(1:nza,1:nra), mxGetPr(piw2d),nra*nza)
		status = matPutVariable(mp,'iw',piw2d)
		call mxDestroyArray(piw2d)
	end if

	c***********************************************************************
	c	  The Happy End:
	c***********************************************************************

	status = matClose(mp)

c***********************************************************************
c	 Back to main:
c***********************************************************************

}return

end
