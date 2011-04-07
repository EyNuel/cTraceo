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
#include <complex.h>
#include <mat.h>
#include <matrix.h>
#include "interpolation.h"

void calcParticleVel(settings_t*);

void calcParticleVel(settings_t* settings){
	MATFile*			matfile	= NULL;
	mxArray*			pTitle	= NULL;
	mxArray*			pru2D	= NULL;
	mxArray*			piu2D	= NULL;
	mxArray*			prw2D	= NULL;
	mxArray*			piw2D	= NULL;
	uintptr_t			i, j, k;
	double				rHyd, zHyd;
	double				xp[3];
	double 				dr, dz;		//used locally to make code more efficient (and more readable)
	complex double 		junkComplex, dP_dRi, dP_dZi;
	complex double**	dP_dR2D = NULL;
	complex double**	dP_dZ2D = NULL;
	//the next 4 variables are temporary variables used to write particle velocity components to matfiles
	double**			ru2D = NULL;
	double**			iu2D = NULL;
	double**			rw2D = NULL;
	double**			iw2D = NULL;
	
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

	//get dr, dz:
	dr = settings->output.dr;
	dz = settings->output.dz;

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
			dP_dR2D = mallocComplex2D(settings->output.nArrayR, settings->output.nArrayZ);
			dP_dZ2D = mallocComplex2D(settings->output.nArrayR, settings->output.nArrayZ);
			for(j=0; j<settings->output.nArrayR; j++){
				//TODO invert for-loops to improove performance (by looping over rightmost element of array)
				rHyd = settings->output.arrayR[j];
				for(k=0; k<settings->output.nArrayZ; k++){
					zHyd = settings->output.arrayZ[k],
					
					//TODO	get these values from a struct, instead of calculating them again?
					//		(they where previously calculated in pressureStar.c)
					
					xp[0] = rHyd - dr;
					xp[1] = rHyd;
					xp[2] = rHyd + dr;
					
					intComplexBarycParab1D(xp, settings->output.pressure_H[j][k], rHyd, &junkComplex, &dP_dRi, &junkComplex);
					
					dP_dR2D[j][k] = -I*dP_dRi;
					
					xp[0] = zHyd - dz;
					xp[1] = zHyd;
					xp[2] = zHyd + dz;
					
					intComplexBarycParab1D(xp, settings->output.pressure_H[j][k], zHyd, &junkComplex, &dP_dZi, &junkComplex);
					
					dP_dZ2D[j][k] = -I*dP_dZi;
				}
			}
			break;
	}

	/**
	 *	Write the data to the output file:
	 */
	if(settings->output.arrayType == ARRAY_TYPE__RECTANGULAR){
		DEBUG(3,"Writing pressure output of rectangular array to file:\n");
		//In the fortran version there were problems when passing complex matrices to Matlab; 
		//therefore the real and complex parts will be saved separately.
		//TODO correct this, this is way to expensive
		
		ru2D = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		iu2D = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		rw2D = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		iw2D = mallocDouble2D(settings->output.nArrayZ, settings->output.nArrayR);
		
		/*
		do i = 1,nza
			do j = 1,nra
				ru2d(i,j) = realpart( dpdr2d(i,j) )
				iu2d(i,j) = imagpart( dpdr2d(i,j) )
				rw2d(i,j) = realpart( dpdz2d(i,j) )
				iw2d(i,j) = imagpart( dpdz2d(i,j) )
			end do
		end do
		*/
		
		for(i=0; i<settings->output.nArrayZ; i++){
			for(j=0; j<settings->output.nArrayR; j++){
				//NOTE: in xx2D, i corresponds to Z, and j for R. in dP_dX2D, it is the other way around.
				ru2D[i][j] = creal( dP_dR2D[j][i] );
				iu2D[i][j] = cimag( dP_dR2D[j][i] );
				rw2D[i][j] = creal( dP_dZ2D[j][i] );
				iw2D[i][j] = cimag( dP_dZ2D[j][i] );
			}
		}
		
		/*
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
		call mxDestroyArray(piw2d
		*/
		
		//write the real part to the mat-file:
		pru2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(pru2D == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(ru2D, mxGetPr(pru2D), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "ru", pru2D);
		mxDestroyArray(pru2D);
		
		piu2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(piu2D == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(iu2D, mxGetPr(piu2D), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "iu", piu2D);
		mxDestroyArray(piu2D);
		
		prw2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(prw2D == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(rw2D, mxGetPr(prw2D), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "rw", prw2D);
		mxDestroyArray(prw2D);
		
		piw2D = mxCreateDoubleMatrix((MWSIZE)settings->output.nArrayZ, (MWSIZE)settings->output.nArrayR, mxREAL);
		if(piw2D == NULL){
			fatal("Memory alocation error.");
		}
		copyDoubleToPtr2D(ru2D, mxGetPr(piw2D), settings->output.nArrayR, settings->output.nArrayZ);
		matPutVariable(matfile, "iw", piw2D);
		mxDestroyArray(piw2D);
		
	}else{
		/*
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
		*/
	}

	//free memory
	matClose(matfile);
	freeComplex2D(dP_dR2D, settings->output.nArrayR);
	freeComplex2D(dP_dZ2D, settings->output.nArrayR);
	freeDouble2D(ru2D, settings->output.nArrayZ);
	freeDouble2D(iu2D, settings->output.nArrayZ);
	freeDouble2D(rw2D, settings->output.nArrayZ);
	freeDouble2D(iw2D, settings->output.nArrayZ);
}
