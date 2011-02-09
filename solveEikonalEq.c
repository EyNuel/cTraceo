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
 * 				settings	Input information.										*
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
#include "csValues.c"
#include "rkf45.c"
#include "boundaryInterpolation.c"
#include "boundaryReflectionCoeff.c"
#include "rayBoundaryIntersection.c"
#include "convertUnits.c"
#include "specularReflection.c"
#if VERBOSE
	#include "mat.h"
	#include "matrix.h"
#endif

void	solveEikonalEq(settings_t*, ray_t*);

void	solveEikonalEq(settings_t* settings, ray_t* ray){
	DEBUG(1,"in. theta: %lf\n", ray->theta);
	
	double			cx, ci,	cc,	sigmaI, sigmaR, sigmaZ,	cri, czi, crri,	czzi, crzi;
	uint32_t		iUp, iDown;
	int32_t			ibdry;						//indicates at which boundary a ray is being reflected (-1 => surface, 1 => bottom)
	uint32_t		sRefl, bRefl, oRefl, nRefl;	//counters for number of reflections at _s_urface, _s_ottom, _o_bject and total (n)
	uint32_t		jRefl;						//TODO huh?!
	uint32_t		numRungeKutta;				//counts the number o RKF45 iterations
	uint32_t		i, j;
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
	double			dsi, ds4, ds5;
	double			stepError;
	double			ri, zi;
	double			altInterpolatedZ, batInterpolatedZ;
	double_t		thetaRefl;
	point_t			pointA, pointB, pointIsect;
	double			rho1, rho2, cp2, cs2, ap, as, lambda, tempDouble;
	double			dr, dz, dIc;
	double 			prod;
	uintptr_t		initialMemorySize;

	//allocate memory for ray components:
	initialMemorySize = (uintptr_t)(fabs((settings->source.rbox2 - settings->source.rbox1)/settings->source.ds))*MEM_FACTOR;
	reallocRay(ray, initialMemorySize);
	
	//set parameters:
	rho1 = 1.0;			//density of water.
DEBUG(10,"\n");
	//define initial conditions:
	ray->iKill	= FALSE;
	iUp			= FALSE;
	iDown		= FALSE;
	sRefl		= 0;
	bRefl		= 0;
	oRefl		= 0;
	jRefl		= 0;
	ray->iRefl[0] = jRefl;
DEBUG(10,":");
	ray->iReturn = FALSE;
	numRungeKutta = 0;
	reflDecay = 1 + 0*I;
	ray->decay[0] = reflDecay;
	ray->phase[0] = 0.0;
DEBUG(10,"\n");
	ray->r[0]	= settings->source.rx;
	ray->rMin	= ray->r[0];
	ray->rMax	= ray->r[0];
	ray->z[0]	= settings->source.zx;
DEBUG(10,"\n");
	es.r = cos( ray->theta );
	es.z = sin( ray->theta );
	e1.r = -es.z;
	e1.z =  es.r;
DEBUG(10,"\n");
	//Calculate initial sound speed and its derivatives:
	csValues( 	settings,
				&(settings->source.rx),
				&(settings->source.zx),
				&cx,
				&cc,
				&sigmaI,
				&cri,
				&czi,
				&slowness,
				&crri,
				&czzi,
				&crzi);
DEBUG(10,":");
	sigmaR	= sigmaI * es.r;
	sigmaZ	= sigmaI * es.z;
	
	ray->c[0]	= cx;
	ray->tau[0]	= 0;
	ray->s[0]	= 0;
	ray->ic[0]	= 0;

	//prepare for Runge-Kutta-Fehlberg integration
	yOld[0] = settings->source.rx;
	yOld[1] = settings->source.zx;
	yOld[2] = sigmaR;
	yOld[3] = sigmaZ;
	fOld[0] = es.r;
	fOld[1] = es.z;
	fOld[2] = slowness.r;
	fOld[3] = slowness.z;



	/************************************************************************
	 *	Start tracing the ray:												*
	 ***********************************************************************/
	i = 0;
	while(	(ray->iKill == FALSE )	&&
			(ray->r[i] < settings->source.rbox2 ) &&
			(ray->r[i] > settings->source.rbox1 )){
			//repeat while the ray is whithin the range box (rbox), and hasn't been killed by any other condition.

		//Runge-Kutta integration:
 		dsi = settings->source.ds;
 		stepError = 1;
 		numRungeKutta = 0;
 		
		while(stepError > 0.1){
			if(numRungeKutta > 100){
				fatal("Runge-Kutta integration: failure in step convergence.\nAborting...");
			}
			rkf45(settings, &dsi, yOld, fOld, yNew, fNew, &ds4, &ds5);

			numRungeKutta++;
			stepError = fabs( ds4 - ds5) / (0.5 * (ds4 + ds5));
			dsi *= 0.5;
		}
		
		es.r = fNew[0];
		es.z = fNew[1];
		ri = yNew[0];
		zi = yNew[1];

		/**		Check for boundary intersections:	**/
		//verify that the ray is still within the defined coordinates of the surface and the bottom:
		if (	(ri > settings->altimetry.r[0]) &&
				(ri < settings->altimetry.r[settings->altimetry.numSurfaceCoords -1]) ||
				(ri > settings->batimetry.r[0]) &&
				(ri < settings->batimetry.r[settings->batimetry.numSurfaceCoords -1] ) ){
			//calculate surface and bottom z at current ray position:
			boundaryInterpolation(	&(settings->altimetry), &ri, &altInterpolatedZ, &junkVector, &normal);
			boundaryInterpolation(	&(settings->batimetry), &ri, &batInterpolatedZ, &junkVector, &normal);
		}else{
			DEBUG(8,"ray killed\n");
			ray->iKill = TRUE;
		}
		DEBUG(9,"altInterpolatedZ: %lf\n", altInterpolatedZ);
		DEBUG(9,"batInterpolatedZ: %lf\n", batInterpolatedZ);
		//Check if the ray is still between the boundaries; if not, find the intersection point and calculate the reflection:
		if ((ray->iKill == FALSE ) && (zi < altInterpolatedZ || zi > batInterpolatedZ)){
			pointA.r = yOld[0];
			pointA.z = yOld[1];
			pointB.r = yNew[0];
			pointB.z = yNew[1];
			
			//	Ray above surface?
			if (zi < altInterpolatedZ){
				DEBUG(5,"ray above surface.\n");
				rayBoundaryIntersection(&(settings->altimetry), &pointA, &pointB, &pointIsect);
				ri = pointIsect.r;
				zi = pointIsect.z;
				
				boundaryInterpolation(	&(settings->altimetry), &ri, &altInterpolatedZ, &tauB, &normal);
				ibdry = -1;
				sRefl = sRefl + 1;
				jRefl = 1;
				
				//Calculate surface reflection:
				specularReflection(&normal, &es, &tauR, &thetaRefl);
				
				//get the reflection coefficient (kill the ray is the surface is an absorver):
				switch(settings->altimetry.surfaceType){
					
					case SURFACE_TYPE__ABSORVENT:	//"A"
						refl = 0 +0*I;
						ray->iKill = TRUE;
						break;
						
					case SURFACE_TYPE__RIGID:		//"R"
						refl = 1 +0*I;
						break;
						
					case SURFACE_TYPE__VACUUM:		//"V"
						refl = -1 +0*I;
						break;
						
					case SURFACE_TYPE__ELASTIC:		//"E"
						switch(settings->altimetry.surfacePropertyType){
							
							case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:		//"H"
								rho2= settings->altimetry.rho[0];
								cp2	= settings->altimetry.cp[0];
								cs2	= settings->altimetry.cs[0];
								ap	= settings->altimetry.ap[0];
								as	= settings->altimetry.as[0];
								lambda = cp2 / settings->source.freqx;
								convertUnits(	&ap,
												&lambda,
												&(settings->source.freqx),
												&(settings->altimetry.surfaceAttenUnits),
												&tempDouble
											);
								ap		= tempDouble;
								lambda	= cs2 / settings->source.freqx;
								convertUnits(	&as,
												&lambda,
												&(settings->source.freqx),
												&(settings->altimetry.surfaceAttenUnits),
												&tempDouble
											);
								as		= tempDouble;
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &refl);
								break;
							
							case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:	//"N"
								//Non-Homogeneous interface =>rho, cp, cs, ap, as are variant with range, and thus have to be interpolated
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.rho,
																&(settings->altimetry.surfaceInterpolation),
																&ri,
																&rho2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.cp,
																&(settings->altimetry.surfaceInterpolation),
																&ri,
																&cp2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.cs,
																&(settings->altimetry.surfaceInterpolation),
																&ri,
																&cs2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.ap,
																&(settings->altimetry.surfaceInterpolation),
																&ri,
																&ap,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.as,
																&(settings->altimetry.surfaceInterpolation),
																&ri,
																&as,
																&junkVector,
																&junkVector
															);
								lambda = cp2/settings->source.freqx;
								convertUnits(&ap, &lambda, &settings->source.freqx, &settings->altimetry.surfaceAttenUnits, &tempDouble);
								ap = tempDouble;
								lambda = cs2/settings->source.freqx;
								convertUnits(&as, &lambda, &settings->source.freqx, &settings->altimetry.surfaceAttenUnits, &tempDouble);
								as = tempDouble;
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &refl);
								break;
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
				if ( cabs(refl) < 1.0e-5 ){
					ray->iKill = TRUE;
				}
			//	end of "ray above surface?"
			}else if (zi > batInterpolatedZ){	//	Ray below bottom?
				DEBUG(5,"ray below bottom.\n");
				DEBUG(3,"ri: %lf, zi: %lf\n", ri, zi);
				rayBoundaryIntersection(&(settings->batimetry), &pointA, &pointB, &pointIsect);
				ri = pointIsect.r;
				zi = pointIsect.z;
				DEBUG(3,"ri: %lf, zi: %lf\n", ri, zi);
				boundaryInterpolation(	&(settings->batimetry), &ri, &batInterpolatedZ, &tauB, &normal);
				//Invert the normal at the bottom for reflection:
				normal.r = -normal.r;	//NOTE: differs from altimetry
				normal.z = -normal.z;	//NOTE: differs from altimetry
				
				ibdry = 1;			
				sRefl = sRefl + 1;
				jRefl = 1;
				
				//Calculate surface reflection:
				specularReflection(&normal, &es, &tauR, &thetaRefl);
				
				//get the reflection coefficient (kill the ray is the surface is an absorver):
				switch(settings->batimetry.surfaceType){
					
					case SURFACE_TYPE__ABSORVENT:	//"A"
						refl = 0 +0*I;
						ray->iKill = TRUE;
						break;
						
					case SURFACE_TYPE__RIGID:		//"R"
						refl = 1 +0*I;
						break;
						
					case SURFACE_TYPE__VACUUM:		//"V"
						refl = -1 +0*I;
						break;
						
					case SURFACE_TYPE__ELASTIC:		//"E"
						switch(settings->batimetry.surfacePropertyType){
							
							case SURFACE_PROPERTY_TYPE__HOMOGENEOUS:		//"H"
								rho2= settings->batimetry.rho[0];
								cp2	= settings->batimetry.cp[0];
								cs2	= settings->batimetry.cs[0];
								ap	= settings->batimetry.ap[0];
								as	= settings->batimetry.as[0];
								lambda = cp2 / settings->source.freqx;
								convertUnits(	&ap,
												&lambda,
												&(settings->source.freqx),
												&(settings->batimetry.surfaceAttenUnits),
												&tempDouble
											);
								ap		= tempDouble;
								lambda	= cs2 / settings->source.freqx;
								convertUnits(	&as,
												&lambda,
												&(settings->source.freqx),
												&(settings->batimetry.surfaceAttenUnits),
												&tempDouble
											);
								as		= tempDouble;
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &refl);
								break;
							
							case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:	//"N"
								//Non-Homogeneous interface =>rho, cp, cs, ap, as are variant with range, and thus have to be interpolated
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.rho,
																&(settings->batimetry.surfaceInterpolation),
																&ri,
																&rho2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.cp,
																&(settings->batimetry.surfaceInterpolation),
																&ri,
																&cp2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.cs,
																&(settings->batimetry.surfaceInterpolation),
																&ri,
																&cs2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.ap,
																&(settings->batimetry.surfaceInterpolation),
																&ri,
																&ap,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.as,
																&(settings->batimetry.surfaceInterpolation),
																&ri,
																&as,
																&junkVector,
																&junkVector
															);
								lambda = cp2/settings->source.freqx;
								convertUnits(&ap, &lambda, &settings->source.freqx, &settings->batimetry.surfaceAttenUnits, &tempDouble);
								ap = tempDouble;
								lambda = cs2/settings->source.freqx;
								convertUnits(&as, &lambda, &settings->source.freqx, &settings->batimetry.surfaceAttenUnits, &tempDouble);
								as = tempDouble;
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &refl);
								break;
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
				if ( cabs(refl) < 1.0e-5 ){
					ray->iKill = TRUE;
				}
			}

			/*	Update marching solution and function:	*/
			//TODO: replace ri, li with pointIsect.r, pointIsect.z
			ri = pointIsect.r;
			zi = pointIsect.z;
			csValues( 	settings, &ri, &zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
			yNew[0] = ri;
			yNew[1] = zi;
			yNew[2] = sigmaI*tauR.r;
			yNew[3] = sigmaI*tauR.z;

			fNew[0] = tauR.r;
			fNew[1] = tauR.z;
			fNew[2] = slowness.r;
			fNew[3] = slowness.z;
		}
		/* TODO Object reflection	*/
		if (settings->objects.numObjects > 0){
			fatal("Object support is WIP.");
			/*
 			//For each object detect if the ray is inside the object range: 
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
							ray->iKill = 1
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

						if ( abs(refl) .lt. 1.0e-5 ) ray->iKill = 1

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
			*/
		} 
		
		
		/*	prepare for next loop	*/
		ri			= yNew[0];
		zi			= yNew[1];
		ray->r[i+1] = yNew[0];
		ray->z[i+1] = yNew[1];
		
		es.r = fNew[0];
		es.z = fNew[1];
		csValues( 	settings, &ri, &zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
		
		dr = ray->r[i+1] - ray->r[i];
		dz = ray->z[i+1] - ray->z[i];
		
		dsi = sqrt( dr*dr + dz*dz );
		
		ray->tau[i+1]	= ray->tau[i] + (dsi)/ci;
		ray->c[i+1]		= ci;
		ray->s[i+1]		= ray->s[i] + (dsi);
		ray->ic[i+1]	= ray->ic[i] + (dsi) * ray->c[i+1];

		ray->iRefl[i+1]		= jRefl;
		ray->boundaryJ[i+1]	= ibdry;

		ray->boundaryTg[i+1].r	= tauB.r;
		ray->boundaryTg[i+1].z	= tauB.z;

		if (jRefl == 1){	//TODO huh?!
			ray->phase[i+1] = ray->phase[i] - atan2( cimag(refl), creal(refl) );
		}else{
			ray->phase[i+1] = ray->phase[i];
		}

		jRefl			= 0;
		ibdry			= 0;
		numRungeKutta	= 0;
		tauB.r			= 0.0;
		tauB.z			= 0.0;
		ray->decay[i+1] = reflDecay;

		for(j=0; j<4; j++){
			yOld[j] = yNew[j];
			fOld[j] = fNew[j];
		}
		//next step:
		i++;
		
		//Prevent further calculations if there is no more space in the memory for the ray coordinates:
		if ( i > ray->nCoords - 1){
			#if VERBOSE
				//when debugging, save the coordinates of the last ray to a separate matfile, before exiting
				mxArray*	pRay	= NULL;
				MATFile*	matfile	= NULL;
				double**	temp2D 	= malloc(2*sizeof(uintptr_t));
				char* 		string	= mallocChar(10);
				
				temp2D[0]	= ray->r;
				temp2D[1]	= ray->z;
				matfile		= matOpen("dyingRay.mat", "w");
				pRay		= mxCreateDoubleMatrix(2, (int32_t)ray->nCoords, mxREAL);
				if(pRay == NULL || matfile == NULL)
					fatal("Memory alocation error.");
				copyDoubleToPtr2D(temp2D, mxGetPr(pRay), ray->nCoords,2);

				sprintf(string, "dyingRay");
				matPutVariable(matfile, (const char*)string, pRay);
				mxDestroyArray(pRay);
				matClose(matfile);
			#endif
			fatal("Ray step too small, number of points in ray coordinates exceeds allocated memory.\nTry changing MEM_FACTOR (in globals.h) to a higher value and recompile.\nAborting...");
			//double the memory allocated for the ray:
			//TODO find bus error that happens on realloc when realloccing to a larger value (tools.c".)
			//reallocRay(ray, ray->nCoords * 2);		//TODO disabled as this sometimes results in "bus error".
			
		}
	}
	/*	Ray coordinates have been computed. Finalizing */
	//adjust memory size of the ray (we don't need more memory than nCoords)
	ray -> nCoords	= i;
	reallocRay(ray, i);

	nRefl 	= sRefl + bRefl + oRefl;		//TODO is this used later?
	
	//Cut the ray at box exit:
	dr	= ray->r[ray->nCoords - 1] - ray->r[ray->nCoords-2];
	dz	= ray->z[ray->nCoords - 1] - ray->z[ray->nCoords-2];
	dIc	= ray->ic[ray->nCoords - 1] -ray->ic[ray->nCoords-2];

	if (ray->r[ray->nCoords-1] > settings->source.rbox2){
		ray->z[ray->nCoords-1]	= ray->z[ray->nCoords-2] + (settings->source.rbox2 - ray->r[ray->nCoords-2])* dz/dr;
		ray->ic[ray->nCoords-1]	= ray->ic[ray->nCoords-2] + (settings->source.rbox2 - ray->r[ray->nCoords-2])* dIc/dr;
		ray->r[ray->nCoords-1]	= settings->source.rbox2;
	}

	if (ray->r[ray->nCoords-1] < settings->source.rbox1){
		ray->z[ray->nCoords-1]	= ray->z[ray->nCoords-2] + (settings->source.rbox1-ray->r[ray->nCoords-2])* dz/dr;
		ray->ic[ray->nCoords-1]	= ray->ic[ray->nCoords-2] + (settings->source.rbox1-ray->r[ray->nCoords-2]) * dIc/dr;
		ray->r[ray->nCoords-1]	= settings->source.rbox1;
	}

	/* Search for refraction points (refraction angles are zero!), rMin, rMax and twisting of rays:	*/
	//NOTE: We are assuming (safely) that there can't be more refraction points than the ray has coordinates,
	//		so we can skip memory bounds-checking.
	ray->nRefrac = 0;
	
	for(i=1; i>ray->nCoords-2; i++){
		ray->rMin = min( ray->rMin, ray->r[i] );
		ray->rMax = max( ray->rMax, ray->r[i] );
		prod = ( ray->z[i+1] - ray->z[i] )*( ray->z[i] - ray->z[i-1] );	//TODO: this may be buggy
		
		if ( (prod < 0) && (ray->iRefl[i] != 1) ){
			ray->nRefrac++;
			ray->refrac[ray->nRefrac - 1].r = ray->r[i];
			ray->refrac[ray->nRefrac - 1].z = ray->z[i];
		}
	
		prod = ( ray->r[i+1]-ray->r[i] )*( ray->r[i]-ray->r[i-1] );
		if ( prod < 0 ){
			ray->iReturn = TRUE;
		}
	}
	
	
	//check last set of coordinates for a min max values of range. (not that the last return conditio has already been tested)
	ray-> rMin = min( ray->rMin, ray->r[ray->nCoords-1] );
	ray-> rMax = max( ray->rMax, ray->r[ray->nCoords-1] );
	i++;
	prod = ( ray->r[i+1]-ray->r[i] )*( ray->r[i]-ray->r[i-1] );
	if ( prod < 0 ){
		ray->iReturn = TRUE;
	}
	prod = 0.0;
	
	//clip allocated memory for refractions
	ray->refrac = reallocPoint(ray->refrac, ray->nRefrac);
	
	//free memory
	free(yOld);
	free(fOld);
	free(yNew);
	free(fNew);
	DEBUG(1,"out\n");
}
