/************************************************************************************
 *	solveEikonalEq.c		 														*
 * 	(formerly "seikeq.for")															*
 * 	A raytracing subroutine for cylindrical simmetry.								*
 * 																					*
 *	originally written in FORTRAN by:												*
 *  						Orlando Camargo Rodriguez:								*
 *							Copyright (C) 2010										*
 * 							Orlando Camargo Rodriguez								*
 *							orodrig@ualg.pt											*
 *							Universidade do Algarve									*
 *							Physics Department										*
 *							Signal Processing Laboratory							*
 *																					*
 *	Ported to C by:			Emanuel Ey												*
 *							emanuel.ey@gmail.com									*
 *							Signal Processing Laboratory							*
 *							Universidade do Algarve									*
 *																					*
 *	Inputs:																			*
 * 				globals		Input information.										*
 * 	Outputs:																		*
 * 				ray:		A structure containing all ray information.				*
 * 							Note that the ray's launching angle (ray->theta) must	*
 *							be previously defined as it is an innput value.			*
 * 							See also: "globals.h" for ray structure definition.		*
 * 																					*
 ***********************************************************************************/

#include "globals.h"
#include "tools.c"
#include <complex.h>
#include <math.h>
#include "csValues.h"
#include "rkf45.c"
#include "boundaryInterpolation.c"
#include "boundaryReflectionCoeff.c"
#include "convertUnits.c"

void	solveEikonalEq(globals_t*, ray_t*);

/*
	subroutine seikeq(thetax,imax,irefl,decay,jbdry,tbdry)
	integer*8 irefl(np),jbdry(np)
	integer*8 i,j
	integer*8 jrefl,jrefr,ibdry,ithtas
	integer*8 noj,nrefl,nrk
	integer*8 imax,ikill,idn,iup
	real*8 tbdry(2,np)
	real*8 roj(npo),zupj(npo),zdnj(npo)
	real*8 yold(4),ynew(4),fold(4),fnew(4)
	real*8 taui(2),taur(2),taub(2)
	real*8 la(2),lb(2),li(2)
	real*8 e1(2),es(2)
	real*8 normal(2)
	real*8 sigma(2),gradc(2)
	real*8 v(2)
	real*8 ri,zi,zatii,zbtyi,ziup,zidn
	real*8 sigmar,sigmaz,sigmai,sri,szi
	real*8 ci,cc,cri,czi,crri,czzi,crzi,cnn
	real*8 dr,dz,dsi,ds4,ds5,dic
	real*8 ap,as,rho1,rho2,cp2,cs2
	real*8 dphase,phasei,phaseii
	real*8 theta,thetax
	real*8 za
	real*8 erros
	real*8 adBoW
	complex*8 decay(np)
	complex*8 refl,refldc
*/

//TODO:	check memory size, and realloc if necessary
//TODO: free the memory

void	solveEikonalEq(globals_t* globals, ray_t* ray){
	double			cx,	cc,	sigmaI,	cri, czi, crri,	czzi, crzi, ;
	uint32_t		iKill, iUp, iDown, iReturn;
	uint32_t		sRefl, bRefl, oRefl, jRefl;		//counters for number of reflections at _s_urface, _s_ottom, _o_bject and total (j)
	uint32_t		numRungeKutta;					//counts the number o RKF45 iterations
	uint32_t		i;
	complex double	refl, reflDecay;
	vector_t		es;				//ray's tangent vector
	vector_t		e1;				//ray's normal vector
	vector_t		slowness;
	vector_t		junkVector;
	vector_t		normal;
	vector_t		tauB;
	vector_t		tauR;
	double*			yOld			= mallocDouble(4);
	double*			fOld 			= mallocDouble(4);
	double*			yNew 			= mallocDouble(4);
	double*			fNew 			= mallocDouble(4);
	double*			dsi				= mallocDouble(1);
	double*			ds4				= mallocDouble(1);
	double*			ds5				= mallocDouble(1);
	double			stepError;
	double*			ri				= mallocDouble(1);
	double*			zi				= mallocDouble(1);
	double*			altInterpolatedZ= mallocDouble(1);
	double*			batInterpolatedZ= mallocDouble(1);
	double_t		thetaRefl;
	point			pointA, pointB, pointIsect;
	double			rho1, rho2, cp2, cs2, ap, as, lambda, tempDouble;
	
	//set parameters:
	rho1 = 1.0;			//density of water.

	//define initial conditions:
	iKill	= FALSE;
	iUp		= FALSE;
	iDown	= FALSE;
	sRefl	= 0;
	bRefl	= 0;
	oRefl	= 0;
	jRefl	= 0;
	ray->iRefl[0] = jRefl;

	iReturn = FALSE;
	numRungeKutta = 0;
	reflDecay = 1 + 0*I;
	ray->decay[0] = reflDecay;
	ray->phase[0] = 0.0;

	ray->r[0]	= globals->settings.source.rx;
	ray->rMin	= ray->r[0];
	ray->rMax	= ray->r[0];
	ray->z[0]	= globals->settings.source.zx;

	es.r = cos( ray->theta );
	es.z = sin( ray->theta );
	e1.r = -es.z;
	e1.z =  es.r;

	//Calculate initial sound speed and its derivatives:
	csValues( 	globals,
				globals->settings.source.*rx,
				globals->settings.source.*zx,
				&cx,
				&cc,
				&sigmaI,
				&cri,
				&czi,
				&slowness,
				&crri,
				&czzi,
				&crzi);

	sigmar	= sigmaI * es.r;	//TODO isn't this the slowness vector (which is already calculated in csValues)?
	sigmaz	= sigmaI * es.z;
	
	ray->c[0]	= cx;
	ray->tau[0]	= 0;
	ray->s[0]	= 0;
	ray->ic[0]	= 0;

	//prepare for Runge-Kutta-Fehlberg integration
	yOld[0] = globals->settings.source.rx;
	yOld[1] = globals->settings.source.zx;
	yOld[2] = sigmar;
	yOld[3] = sigmaz;
	fOld[0] = es.r;
	fOld[1] = es.z;
	fOld[2] = slowness.r;
	fOld[3] = slowness.z;



	/************************************************************************
	 *	Start tracing the ray:												*
	 ***********************************************************************/
//1000	if (( ray->r[i).lt.rbox(2) ).and.( ray->r[i).gt.rbox(1) ).and.( ikill.eq.0 )) then
	i = 0:
	while(	(iKill == FALSE )	&&
			(ray->r[i] < globals->settings.source.rbox2 ) &&
			(ray->r[i] > globals->settings.source.rbox1 )){
			//repeat while the ray is whithin the range box (rbox), and hasn't been killed by any other condition.

		//Runge-Kutta integration:
 		*dsi = globals->settings.source.ds;
 		stepError = 1;
 		numRungeKutta = 0;
 		
		while(stepError > 0.1){
			if(numRungeKutta > 100){
				fatal("Runge-Kutta integration: failure in step convergence.\nAborting...");
			}
			rkf45(dsi, yOld, fOld, yNew, fNew, ds4, ds5);
			numRungeKutta++;
			stepError = abs( *ds4 - *ds5) / (0.5 * (*ds4 + *ds5));
			*dsi *= 0.5;
		}
		
		es.r = fNew[0];
		es.z = fNew[1];
		*ri = yNew[0];
		*zi = yNew[1];

		/**		Check for boundary intersections:	**/
		//verify that the ray is still within the defined coordinates of the surface and the bottom:
		if (	(ri > globals->settings.altimetry.r[0]) &&
				(ri < globals->settings.altimetry.r[globals->settings.altimetry.numSurfaceCoords]) &&
				(ri > globals->settings.batimetry.r[0]) &&
				(ri < globals->settings.batimetry.r[globals->settings.batimetry.numSurfaceCoords]) ){
			//calculate surface and bottom z at current ray position:
			boundaryInterpolation(	&(globals->settings.altimetry), ri, altInterpolatedZ, &junkVector, &normal);
			boundaryInterpolation(	&(globals->settings.batimetry), ri, batInterpolatedZ, &junkVector, &normal);
		}else{
			iKill = TRUE;
		}
		
		//Check if the ray is still between the boundaries; if not, find the intersection point and calculate the reflection:
		if ((iKill == FALSE ) && (zi < altInterpolatedZ || zi > batInterpolatedZ)){
			pointA.r = yOld[0];
			pointA.z = yOld[1];
			pointB.r = yNew[0];
			pointB.z = yNew[1];
			
			//	Ray above surface?
			if (zi < altInterpolatedZ){
				rayBoundaryIntersection(&(globals->settings.altimetry), &pointA, &pointB, &pointIsect);
				ri = pointIsect.r;
				zi = pointIsect.z;
				
				boundaryInterpolation(	&(globals->settings.altimetry), ri, altInterpolatedZ, &tauB, &normal);
				ibdry = -1;
				sRefl = sRefl + 1;
				jRefl = 1;
				
				//Calculate surface reflection:
				specularReflection(&normal, &es, &tauR, &thetaRefl);
				
				//get the reflection coefficient (kill the ray is the surface is an absorver):
				switch(globals->settings.altimetry.surfaceType){
					
					case SURFACE_TYPE__ABSORVENT:	//"A"
						refl = 0 +0*I;
						iKill = TRUE;
						break;
						
					case SURFACE_TYPE__RIGID:		//"R"
						refl = 1 +0*I;
						break;
						
					case SURFACE_TYPE__VACUUM:		//"V"
						refl = -1 +0*I;
						break;
						
					case SURFACE_TYPE__ELASTIC:		//"E"
						switch(globals->settings.altimetry.surfacePropertyType){
							
							case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:		//"H"
								rho2= globals->settings.altimetry.surfaceProperties[0].rho;
								cp2	= globals->settings.altimetry.surfaceProperties[0].cp;
								cs2	= globals->settings.altimetry.surfaceProperties[0].cs;
								ap	= globals->settings.altimetry.surfaceProperties[0].ap;
								as	= globals->settings.altimetry.surfaceProperties[0].as;
								lambda = cp2 / globals->settings.source.freqx;
								convertUnits(	&ap,
												&lambda,
												&(globals->settings.source.freqx),
												&(globals->settings.altimetry.surfaceAttenUnits),
												&tempDouble
											);
								ap		= tempDouble;
								lambda	= cs2 / globals->settings.source.freqx;
								convertUnits(	&as,
												&lambda,
												&(globals->settings.source.freqx),
												&(globals->settings.altimetry.surfaceAttenUnits),
												&tempDouble
											);
								as		= tempDouble;
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &refl);
								break;
							
							case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:	//"N"
								fatal("Non-homogeneous surface properties are WIP.");	//TODO restructure interfaceProperties to contain pointers to cp, cs, etc;
								/*
								//Non-Homogeneous interface =>rho, cp, cs, ap, as are variant with range, and thus have to be interpolated
								boundaryInterpolationExplicit(	&(globals->settings.altimetry.numSurfaceCoords),
																globals->settings.altimetry.r,
																&(globals->settings.altimetry.surfaceProperties.rho),
																&(globals->settings.altimetry.surfaceInterpolation),
																&ri,
																&rho2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(globals->settings.altimetry.numSurfaceCoords),
																globals->settings.altimetry.r,
																&(globals->settings.altimetry.surfaceProperties.cp),
																&(globals->settings.altimetry.surfaceInterpolation),
																&ri,
																&cp2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(globals->settings.altimetry.numSurfaceCoords),
																globals->settings.altimetry.r,
																&(globals->settings.altimetry.surfaceProperties.cs),
																&(globals->settings.altimetry.surfaceInterpolation),
																&ri,
																&rho2,
																&junkVector,
																&junkVector
															);
								call bdryi(nati,rati, csati,aitype,ri, cs2,v,v)
								call bdryi(nati,rati, apati,aitype,ri,  ap,v,v)
								call bdryi(nati,rati, asati,aitype,ri,  as,v,v)
								lambda = cp2/freqx
								call cnvnts(ap,lambda,freqx,atiu,tempDouble)
								ap = tempDouble
								lambda = cs2/freqx
								call cnvnts(as,lambda,freqx,atiu,tempDouble)
								as = tempDouble
								call bdryr(rho1,rho2,ci,cp2,cs2,ap,as,theta,refl)
								break;
								*/
							default:
								fatal("Unknown surface properties (neither H or N).\nAborting...");
								break;
							}
						break;
					default:
						fatal("Unknown surface type (neither A,E,R or V).\nAborting...");
						break;
				}
				reflDecay = reflDecay * refl;

				//Kill the ray if the reflection coefficient is too small: 
				if ( abs(refl) < 1.0e-5 ){
					iKill = TRUE;
				}
			}		//end of "ray above surface?"
c======================================================================
c				Ray below bottom?
			if (zi.gt.batInterpolatedZ) then
				call raybi(nbty,rbty,zbty,bitype,la,lb,li)
				ri = li(1)
				zi = li(2)
				call bdryi(nbty,rbty,zbty,bitype,ri,batInterpolatedZ,taub,normal)

c					Invert the normal at the bottom for reflection:
				normal(1) = -normal(1)
				normal(2) = -normal(2)
				ibdry = 1

				bRefl = bRefl + 1
				jRefl = 1

c            		Calculate bottom reflection:
				call reflct( normal, es, taur, theta )
c           		Bottom reflection => get the reflection coefficient  

c		            (kill the ray if the bottom is an absorver):
				if (btype.eq.'A') then
					refl = (0.0,0.0)
					ikill = 1
					refl = (0.0,0.0)
				elseif (btype.eq.'R') then
					refl = (1.0,0.0)
				elseif (btype.eq.'V') then
					refl = (-1.0,0.0)
				elseif (btype.eq.'E') then
					if (bptype.eq.'H') then
						rho2 = rhobty(1)
						cp2 =  cpbty(1)
						cs2 =  csbty(1)
						ap  =  apbty(1)
						as  =  asbty(1)
						lambda = cp2/freqx
						call cnvnts(ap,lambda,freqx,btyu,adBoW)
						ap  = adBoW
						lambda = cs2/freqx
						call cnvnts(as,lambda,freqx,btyu,adBoW)
						as  = adBoW
						call bdryr(rho1,rho2,ci,cp2,cs2,ap,as,theta,refl)
					elseif (bptype.eq.'N') then
						call bdryi(nbty,rbty,rhobty,bitype,ri,rho2,v,v)
						call bdryi(nbty,rbty, cpbty,bitype,ri, cp2,v,v)
						call bdryi(nbty,rbty, csbty,bitype,ri, cs2,v,v)
						call bdryi(nbty,rbty,acpbty,bitype,ri,  ap,v,v)
						call bdryi(nbty,rbty,acsbty,bitype,ri,  as,v,v)
						lambda = cp2/freqx
						call cnvnts(ap,lambda,freqx,btyu,adBoW)
						ap = adBoW
						lambda = cs2/freqx
						call cnvnts(as,lambda,freqx,btyu,adBoW)
						as = adBoW
						call bdryr(rho1,rho2,ci,cp2,cs2,ap,as,theta,refl)
					else
						write(6,*) 'Unknown bottom properties'
						write(6,*) '(neither H or N),'
						write(6,*) 'aborting calculations...'
						stop
					end if
				else
					write(6,*) 'Unknown bottom type (neither A,E,R or V),'
					write(6,*) 'aborting calculations...'
					stop
					end if
				reflDecay = reflDecay*refl

c          			Kill the ray if the reflection coefficient is too small:
				if ( abs(refl) .lt. 1.0e-5 ) ikill = 1
			end if
c======================================================================
c         		Update marching solution and function:
			ri = li(1)
			zi = li(2)
			call csvals(ri,zi,ci,cc,sigmaI,cri,czi,sri,szi,crri,czzi,crzi)

			yNew[1) = ri
			yNew[2) = zi
			yNew[3) = sigmaI*taur(1)
			yNew[4) = sigmaI*taur(2)

			fNew[1) = taur(1)
			fNew[2) = taur(2)
			fNew[3) = sri
			fNew[4) = szi
		end if
c[====================================================================]
c[oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo]
c         	Object reflection:
		if (nobj.gt.0) then
c 				For each object detect if the ray is inside the object range: 
			do j = 1,nobj
				noj = no(j) 
				if ((ri.ge.ro(j,1)).and.(ri.lt.ro(j,noj))) then
					do k = 1,noj
						roj(k) =  ro(j,k)
						zdnj(k) = zdn(j,k)
						zupj(k) = zup(j,k)
					end do        
					if (zdnj(1).ne.zupj(1)) then
						write(6,*) 'Lower and upper faces do not start at the same'
						write(6,*) 'depth! aborting calculations...' 
						stop
					end if 
					call bdryi(noj,roj,zdnj,oitype,ri,zidn,v,normal)
					call bdryi(noj,roj,zupj,oitype,ri,ziup,v,normal)

c   		   			Second point is inside the object?
					if ((yNew[2).ge.zidn).and.(yNew[2).lt.ziup)) then
						la(1) = yOld[1)
						la(2) = yOld[2)
						lb(1) = yNew[1)
						lb(2) = yNew[2)
c=============================================================================>
c            				Which face was crossed by the ray: upper or lower?
c            				Case 1: beginning in box & end in box:
						if (yOld[1).ge.roj(1)) then
							call bdryi(noj,roj,zdnj,oitype,yOld[1),zidn,v,normal)
							call bdryi(noj,roj,zupj,oitype,yOld[1),ziup,v,normal)
							if (yOld[2).lt.zidn) then
								call raybi(noj,roj,zdnj,oitype,la,lb,li)
								iDown = 1
								iUp = 0
							else 
								call raybi(noj,roj,zupj,oitype,la,lb,li)
								iDown = 0
								iUp = 1
							end if
c            				Case 2: beginning out box & end in box
c							(zdnj(1) = zupj(1)):
						elseif (yOld[1).lt.roj(1)) then
							za = lb(2)-(lb(2)-la(2))/(lb(1)-la(1))*(lb(1)-roj(1))
							la(1) = roj(1)
							la(2) = za
							if (za.lt.zupj(1)) then
								call raybi(noj,roj,zdnj,oitype,la,lb,li)
								iDown = 1
								iUp = 0
							else
								call raybi(noj,roj,zupj,oitype,la,lb,li)
								iDown = 0
								iUp = 1
							end if
						else
							write(6,*) 'Object reflection case: ray beginning'
							write(6,*) 'neither behind or between object box,'
							write(6,*) 'check object coordinates...'
							stop
						end if
c=============================================================================>
						ri = li(1)
						zi = li(2)

c            				Face reflection: upper or lower? 
						if ((iDown.eq.1).and.(iUp.eq.0)) then 
							call bdryi(noj,roj,zdnj,oitype,ri,zidn,taub,normal)
							normal(1) = -normal(1)
							normal(2) = -normal(2)
							ibdry = -1
						elseif ((iDown.eq.0).and.(iUp.eq.1)) then
							call bdryi(noj,roj,zupj,oitype,ri,ziup,taub,normal)
							ibdry =  1
						else
							write(6,*) 'Object reflection case: ray neither being'
							write(6,*) 'reflected on down or up faces,'
							write(6,*) 'check object coordinates...'
							stop
						end if
						oRefl = oRefl + 1 
						jRefl = 1

c   						Calculate object reflection: 
						call reflct( normal, es, taur, theta )

c            				Object reflection => get the reflection coefficient
c            				(kill the ray is the object is an absorver):

						if (otype(j:j).eq.'A') then
							refl = (0.0,0.0)
							ikill = 1
						elseif (otype(j:j).eq.'R') then
							refl = (1.0,0.0)
						elseif (otype(j:j).eq.'V') then
							refl = (-1.0,0.0)
						elseif (otype(j:j).eq.'E') then
							rho2 = orho(j)
							cp2 =  ocp(j)
							cs2 =  ocs(j)
							ap  =  oap(j)
							as  =  oas(j)
							lambda = cp2/freqx
							call cnvnts(ap,lambda,freqx,obju(j:j),adBoW)
							ap  = adBoW
							lambda = cs2/freqx
							call cnvnts(as,lambda,freqx,obju(j:j),adBoW)
							as  = adBoW
							call bdryr(rho1,rho2,ci,cp2,cs2,ap,as,theta,refl)
						else
							write(6,*) 'Unknown object type (neither A,E,R or V),'
							write(6,*) 'aborting calculations...'
							stop
						end if
						reflDecay = reflDecay*refl

c          					Kill the ray if the reflection coefficient is too small: 

						if ( abs(refl) .lt. 1.0e-5 ) ikill = 1

c          					Update marching solution and function:
						es.r = taur(1)
						es.z = taur(2)
						e1.r = -es.z
						e1.z =  es.r
						call csvals(ri,zi,ci,cc,sigmaI,cri,czi,sri,szi,crri,czzi,crzi)
						yNew[1) = ri
						yNew[2) = zi
						yNew[3) = sigmaI*es.r
						yNew[4) = sigmaI*es.z

						fNew[1) = es.r
						fNew[2) = es.z
						fNew[3) = sri
						fNew[4) = szi
					end if
				end if 
			end do 
		end if 
		c[oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo]
		//actualização das coord do raio
		ri     = yNew[1)
		zi     = yNew[2)
		ray->r[i+1) = yNew[1)
		ray->z[i+1) = yNew[2)
	  
		es.r = fNew[1)
		es.z = fNew[2)
	  
		call csvals(ri,zi,ci,cc,sigmaI,cri,czi,sri,szi,crri,czzi,crzi)
		dr = ray->r[i+1) - ray->r[i)
		dz = ray->z[i+1) - ray->z[i)
	  
		dsi = sqrt( dr*dr + dz*dz )
	  
		ray->tau[i+1) = ray->tau[i) + dsi/ci
		ray->c[i+1) =    ci
		ray->s[i+1) =   ray->s[i) + dsi
		ray->ic[i+1) =  ray->ic[i) + dsi*ray->c[i+1)

		ray->iRefl(i+1) = jRefl
		jbdry(i+1) = ibdry

		tbdry(1,i+1) = taub(1)
		tbdry(2,i+1) = taub(2)

		if (jRefl.eq.1) then
			ray->phase(i+1) = ray->phase(i) - atan2(imagpart(refl),realpart(refl))
		else 
			ray->phase(i+1) = ray->phase(i)
		end if 

		jRefl = 0
		ibdry = 0
		numRungeKutta   = 0
		taub(1) = 0.0
		taub(2) = 0.0
		ray->decay(i+1) = reflDecay

		do j = 1,4
			yOld[j) = yNew[j)
			fOld[j) = fNew[j)
		end do

c-----------------------------------------------------------------------
c			Next one:
		i = i + 1
c-----------------------------------------------------------------------
c 			Prevent further calculations if there is no more space in 
c         	the memory for the ray coordinates:
		if ( i.gt.np ) then
			write(6,*) 'Ray step too small, number of points in ray'
			write(6,*) 'coordinates > ',np,',' 
			write(6,*) 'aborting calculations...'
		stop
	end if
c-----------------------------------------------------------------------
	goto 1000
end if
imax = i
nrefl = sRefl + bRefl + oRefl

c-----------------------------------------------------------------------
c   Cut the ray at box exit:
dr =  ray->r[imax)- ray->r[imax-1)
dz =  ray->z[imax)- ray->z[imax-1)
dic = ray->ic[imax)-ray->ic[imax-1)

if (ray->r[imax).gt.rbox(2)) then
	ray->z[imax) =  ray->z[imax-1) + (rbox(2)-ray->r[imax-1))* dz/dr
	ray->ic[imax) = ray->ic[imax-1) + (rbox(2)-ray->r[imax-1))*dic/dr
	ray->r[imax) = rbox(2)
end if

if (ray->r[imax).lt.rbox(1)) then
	ray->z[imax) =  ray->z[imax-1) + (rbox(1)-ray->r[imax-1))* dz/dr
	ray->ic[imax) = ray->ic[imax-1) + (rbox(1)-ray->r[imax-1))*dic/dr
	ray->r[imax) = rbox(1)
end if

c-----------------------------------------------------------------------
c	Search for refraction points (refraction angles are zero!), 
c   rmin, rmax and twisting of rays: 
nrefr = 0
do i = 2,imax-1
	rmin = min( rmin, ray->r[i) )
	rmax = max( rmax, ray->r[i) )
	prod = ( ray->z[i+1)-ray->z[i) )*( ray->z[i)-ray->z[i-1) )
	if ( ( prod.lt.0 ).and.(ray->iRefl(i).ne.1) ) then
		if (nrefr.gt.np2) then
			write(6,*) 'Refraction points: nrefr > ',np2
			write(6,*) 'aborting calculations...'
			stop
		end if
		nrefr = nrefr + 1
		rrefr(nrefr) = ray->r[i)
		zrefr(nrefr) = ray->z[i)
	end if

	prod = ( ray->r[i+1)-ray->r[i) )*( ray->r[i)-ray->r[i-1) )
	if ( prod.lt.0 ) then
		iReturn = 1
	end if
	prod = 0.0 
end do
rmin = min( rmin, ray->r[imax) )
rmax = max( rmax, ray->r[imax) )

c***********************************************************************
c     Back to main:
c***********************************************************************

   return

   end
*/
}
