/********************************************************************************
 *	calcEigenRayPr.c		 													*
 * 	(formerly "calepr.for")														*
 *	Calculates eigenrays using proximity method.								*
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
 * 				n:		number of elements in vector x							*
 * 				x:		vector to be searched									*
 * 				xi:		scalar who's bracketing elements are to be found		*
 * 	Outputs:																	*
 * 				nb:		number of bracketing pairs found.						*
 *				ib:		the indexes of the lower bracketing elements.			*
 * 						Note: shall be previously allocated.					*
 * 																				*
 *******************************************************************************/

#include <complex.h>
#include "globals.h"
#include "tools.c"
#include "solveDynamicEq.c"
#include "solveEikonalEq.c"
#include <mat.h>
#include "matrix.h"
#include "interpolation.h"

void	calcEigenRayPr(settings_t*);

/*
	character*60 ctitle
	character*10 eray
	character*9 cix
	character*8 cviii
	character*7 cvii
	character*6 cvi
	character*5 cv
	character*4 civ
	character*3 ciii
	character*2 cii
	character*1 ci
		 
	integer*8 irefl(np),jbdry(np)
	integer*8 iret(51)
	integer*8 i,ifail,iHyd,imax,j,jj,jjj,k,l,m
	integer*8 nthtas,nret,nEigenRays
	
	integer*8 matOpen, matClose
	integer*8 mxCreateDoubleMatrix, mxCreateString, mxGetPr
	integer*8 matfile, ptitle, pthtas, prh, pzh, pray, perays
	integer*8 matPutVariable, matDeleteVariable
	integer*8 status
	
	real*8 raydat(5,np)
	real*8  tbdry(2,np)
	
	real*8 xl(2),yl(2)
	
	real*8 thetai,ctheta
	real*8 rHyd,zHyd,zRay,dz,tauRay
	real*8 rerays
	
	complex*8 decay(np)
	complex*8 zl(2)
	complex*8 ampRay
*/

void	calcEigenRayPr(settings_t* settings){
	DEBUG(1,"in\n");
	MATFile*	matfile	= NULL;
	mxArray*	pThetas	= NULL;
	mxArray*	pTitle	= NULL;
	mxArray*	pHydArrayR = NULL;
	mxArray*	pHydArrayZ = NULL;
	
	uintptr_t	nEigenRays = 0;
	uintptr_t	i, j, jj, k;
	double 		rHyd, zHyd, zRay, ampRay, tauRay;
	double		dz;
	

	matfile 	= matOpen('eig.mat', 'w');
	pThetas		= mxCreateDoubleMatrix(1, (int32_t)settings->source.nThetas, mxREAL);
	if(matfile == NULL || pThetas == NULL){
		fatal("Memory alocation error.");
	}


	//copy angles in cArray to mxArray:
	copyDoubleToPtr(	settings->source.thetas,
						mxGetPr(pThetas),
						settings->source.nThetas);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "thetas", pThetas);
	mxDestroyArray(pThetas);


	//write title to matfile:
	pTitle = mxCreateString("TRACEO: EIGenrays (by proximity)");
	if(pTitle == NULL){
		fatal("Memory alocation error.");
	}
	matPutVariable(matfile, "caseTitle", pTitle);
	mxDestroyArray(pTitle);


	//write hydrophone array ranges to file:
	pHydArrayR			= mxCreateDoubleMatrix(1, (int32_t)settings->output.nArrayR, mxREAL);
	if(pHydArrayR == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToPtr(	settings->output.arrayR,
						mxGetPr(pHydArrayR),
						(int32_t)settings->output.nArrayR);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "rarray", pHydArrayR);
	mxDestroyArray(pHydArrayR);


	//write hydrophone array ranges to file:
	pHydArrayZ			= mxCreateDoubleMatrix(1, (int32_t)settings->output.nArrayZ, mxREAL);
	if(pHydArrayZ == NULL){
		fatal("Memory alocation error.");
	}
	copyDoubleToPtr(	settings->output.arrayZ,
						mxGetPr(pHydArrayZ),
						(int32_t)settings->output.nArrayZ);
	//move mxArray to file and free memory:
	matPutVariable(matfile, "zarray", pHydArrayZ);
	mxDestroyArray(pHydArrayZ);


	//allocate memory for the rays:
	ray = makeRay(settings->source.nThetas);


	/** Trace the rays:  */
	for(i=0; i<settings.source.nThetas; i++){
		thetai = -settings->source.thetas[i] * M_PI/180.0;
		ray[i].theta = thetai;
		DEBUG(2,"ray[%u].theta: %lf\n", (uint32_t)i, settings->source.thetas[i]);
		ctheta = fabs( cos(thetai));
		
		//Trace a ray as long as it is neither 90 or -90:
		if (ctheta > 1.0e-7){
			solveEikonalEq(settings, &ray[i]);
			solveDynamicEq(settings, &ray[i]);

			//test for proximity of ray to each hydrophone 
			//(yes, this is slow, can you figure out a better way to do it?)
			for(j=0; j<settings.output.nArrayR; j++){
				rHyd = settings->output.arrayR[j];

				if ( (rHyd >= ray[i].rMin) && (rHyd < ray[i].rMax)){

					//	Check if the ray is returning back or not;
					//	if not:		we can bracket it without problems,
					//	otherwise:	we need to know how many times it passed by the given array range: 
					if (ray[i].iReturn == FALSE){

						//get the index of the lower bracketing element:
						bracket(			ray[i].nCoords,	ray[i].r,rHyd,iHyd);
						//from index interpolate the rays' depth, travel time and amplitude:
						intLinear1D(		&ray[i].r[iHyd], &ray[i].z[iHyd],	rHyd, &zRay,	&junkDouble);
						intLinear1D(		&ray[i].r[iHyd], &ray[i].tau[iHyd],	rHyd, &tauRay,	&junkDouble);
						intComplexLinear1D(	&ray[i].r[iHyd], &ray[i].amp[iHyd],	rHyd, &ampRay,	&junkDouble);

						//for every hydrophone check distance to ray
						for(jj=1; jj<=settings->output.nArrayZ; jj++){
							zHyd = settings->output.arrayZ[jj];
							dz = abs(zRay-zHyd);
							
							if (dz settings->output.miss){
								nEigenRays++;
/*
c#######################################################################
								if (nEigenRays.lt.10) then
									jjj = 4
									write (ci, '(I1)'), nEigenRays
									eray(4:4) = ci
								elseif ((nEigenRays.ge.10).and.(nEigenRays.lt.100)) then
									jjj = 5
									write (cii, '(I2)'), nEigenRays
									eray(4:5) = cii
								elseif ((nEigenRays.ge.100).and.(nEigenRays.lt.1000))  then
									jjj = 6
									write (ciii, '(I3)'), nEigenRays
									eray(4:6) = ciii
								elseif ((nEigenRays.ge.1000).and.(nEigenRays.lt.10000))  then
									jjj = 7
									write (civ, '(I4)'), nEigenRays
									eray(4:7) = civ
								elseif ((nEigenRays.ge.1000).and.(nEigenRays.lt.10000))  then
									jjj = 8
									write (cv, '(I5)'), nEigenRays
									eray(4:8) = cv
								else
									jjj = 9
									write (cvi, '(I6)'), nEigenRays
									eray(4:9) = cvi
								end if										
c#######################################################################
*/
								//adjust the ray's last coordinate so that it matches up with the hydrophone
								ray[i].r[iHyd+1]	= rHyd;
								ray[i].z(iHyd+1)	= zRay;
								ray[i].tau(iHyd+1)	= tauRay;
								ray[i].amp(iHyd+1)	= ampRay;

								//TODO continue here:
								///save ray information:
								rayInfo[0][i] = (double)ray[i].iReturn;
								rayInfo[1][i] = ray[i].sRefl;
								rayInfo[2][i] = ray[i].bRefl;
								rayInfo[3][i] = ray[i].oRefl;
								rayInfo[4][i] = (double)ray[i].nRefrac;
								
								do k = 1,iHyd+1
									raydat(1,k) =   r(k)
									raydat(2,k) =   z(k)
									raydat(3,k) = tau(k)
									raydat(4,k) = realpart( amp(k) )
									raydat(5,k) = imagpart( amp(k) )
								end do

								pray	= mxCreateDoubleMatrix(5,iHyd+1,0)
								call 	mxCopyReal8ToPtr(raydat,mxGetPr(pray),5*(iHyd+1))
								status 	= matPutVariable(matfile,eray(1:jjj),pray)
								call 	mxDestroyArray(pray)
							}
						}
					}else{ 	//if (ray[i].iReturn == FALSE)
						call ebrckt(imax,r,rHyd,nret,iret)
						do l = 1,nret
							xl(1) =   r(iret(l)  )
							xl(2) =   r(iret(l)+1)
							yl(1) =   z(iret(l)  )
							yl(2) =   z(iret(l)+1)
							
							call lini1d(xl,yl,rHyd,zRay,&junkDouble)
							yl(1) = tau(iret(l)  )
							yl(2) = tau(iret(l)+1)
							
							call lini1d(xl,yl,rHyd,tauRay,&junkDouble)
							zl(1) = amp(iret(l)  )
							zl(2) = amp(iret(l)+1)
							
							call clin1d(xl,zl,rHyd,ampRay,&junkDouble)
							do jj = 1,nza
								zHyd = zArray(jj)
								dz = abs(zRay-zHyd)
								if (dz.lt.miss) then
									nEigenRays = nEigenRays + 1
/*
c#######################################################################
								if (nEigenRays.lt.10) then
									jjj = 4
									write (ci, '(I1)'), nEigenRays
									eray(4:4) = ci
								elseif ((nEigenRays.ge.10).and.(nEigenRays.lt.100)) then
									jjj = 5
									write (cii, '(I2)'), nEigenRays
									eray(4:5) = cii
								elseif ((nEigenRays.ge.100).and.(nEigenRays.lt.1000))  then
									jjj = 6
									write (ciii, '(I3)'), nEigenRays
									eray(4:6) = ciii
								elseif ((nEigenRays.ge.1000).and.(nEigenRays.lt.10000))  then
									jjj = 7
									write (civ, '(I4)'), nEigenRays
									eray(4:7) = civ
								elseif ((nEigenRays.ge.1000).and.(nEigenRays.lt.10000))  then
									jjj = 8
									write (cv, '(I5)'), nEigenRays
									eray(4:8) = cv
								else
									jjj = 9
									write (cvi, '(I6)'), nEigenRays
									eray(4:9) = cvi
								end if
c#######################################################################
*/
									r(iHyd+1) =	  rHyd
									z(iHyd+1) =	zRay
									tau(iHyd+1) = tauRay
									amp(iHyd+1) =	ampRay
									
									do k = 1,iHyd+1
										raydat(1,k) =   r(k)
										raydat(2,k) =   z(k)
										raydat(3,k) = tau(k)
										raydat(4,k) = realpart( amp(k) )
										raydat(5,k) = imagpart( amp(k) )
									end do
									
									pray = mxCreateDoubleMatrix(5,iHyd+1,0)
									call mxCopyReal8ToPtr(raydat,mxGetPr(pray),5*(iHyd+1))
									status = matPutVariable(matfile,eray(1:jjj),pray)
									call mxDestroyArray(pray)
								end if
							end do
						end do
					end if 		//(iretrn.eq.0)
c***********************************************************************
				end if 
			end do 
		end if
	end do

c***********************************************************************
c      The Happy End:
c***********************************************************************

	//rerays = 1.0*erays 
	pnEigenRays = mxCreateDoubleMatrix(1,1,0)

	call mxCopyReal8ToPtr(nEigenRays,mxGetPr(pnEigenRays),1)
	status = matPutVariable(matfile,'nerays',pnEigenRays)
	call mxDestroyArray(pnEigenRays)
	status = matClose(matfile)

c***********************************************************************
c     Back to main:
c***********************************************************************

return

end
								
