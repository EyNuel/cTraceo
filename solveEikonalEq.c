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
 *	Ported to C for project SENSOCEAN by:											*
 * 						Emanuel Ey													*
 *						emanuel.ey@gmail.com										*
 *						Signal Processing Laboratory								*
 *						Universidade do Algarve										*
 *																					*
 *	Inputs:																			*
 * 				settings	Input information.										*
 * 	Outputs:																		*
 * 				ray:		A structure containing all ray information.				*
 * 							Note that the ray's launching angle (ray->theta) must	*
 *							be previously defined as it is an input value.			*
 * 							See also: "globals.h" for ray structure definition.		*
 * 																					*
 ***********************************************************************************/

#pragma  once
#include "globals.h"
#include "tools.h"
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
	uint32_t		sRefl, bRefl, oRefl;	//counters for number of reflections at _s_urface, _s_ottom and _o_bject
	uint32_t		jRefl;						//TODO huh?!
	uint32_t		numRungeKutta;				//counts the number o RKF45 iterations
	uint32_t		i, j;
	complex double	reflCoeff, reflDecay;
	vector_t		es = {0,0};				//ray's tangent vector
	vector_t		e1 = {0,0};				//ray's normal vector
	vector_t		slowness = {0,0};
	vector_t		junkVector = {0,0};
	vector_t		normal = {0,0};
	vector_t		tauB = {0,0};
	vector_t		tauR = {0,0};
	double*			yOld			= mallocDouble(4);
	double*			fOld 			= mallocDouble(4);
	double*			yNew 			= mallocDouble(4);
	double*			fNew 			= mallocDouble(4);
	double			dsi, ds4, ds5;
	double			stepError;
	double			ri, zi;
	double			altInterpolatedZ, batInterpolatedZ;
	double			thetaRefl;
	point_t			pointA, pointB, pointIsect;
	double			rho1, rho2, cp2, cs2, ap, as, lambda, tempDouble;
	double			dr, dz, dIc;
	double 			prod;
	uintptr_t		initialMemorySize;
	uint32_t		nObjCoords;	//"noj"
	double			ziDown, ziUp;	//"zidn, ziup", interpolated height of upper/lower boundary of an object

	//allocate memory for ray components:
	initialMemorySize = (uintptr_t)(fabs((settings->source.rbox2 - settings->source.rbox1)/settings->source.ds))*MEM_FACTOR;
	reallocRayMembers(ray, initialMemorySize);
	
	//set parameters:
	rho1 = 1.0;			//density of water.
	
	//define initial conditions:
	ray->iKill	= FALSE;
	iUp			= FALSE;
	iDown		= FALSE;
	sRefl		= 0;
	bRefl		= 0;
	oRefl		= 0;
	jRefl		= 0;
	ray->iRefl[0] = jRefl;

	ray->iReturn = FALSE;
	numRungeKutta = 0;
	reflDecay = 1 + 0*I;
	ray->decay[0] = reflDecay;
	ray->phase[0] = 0.0;

	ray->r[0]	= settings->source.rx;
	ray->rMin	= ray->r[0];
	ray->rMax	= ray->r[0];
	ray->z[0]	= settings->source.zx;

	es.r = cos( ray->theta );
	es.z = sin( ray->theta );
	e1.r = -es.z;
	e1.z =  es.r;

	//Calculate initial sound speed and its derivatives:
	csValues( 	settings,
				settings->source.rx,
				settings->source.zx,
				&cx,
				&cc,
				&sigmaI,
				&cri,
				&czi,
				&slowness,
				&crri,
				&czzi,
				&crzi);

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
		
		/**
		 * Check for boundary intersections:
		 ***/
		DEBUG(5,"Verify that the ray is still within the defined coordinates of the surface and the bottom: \n");
		DEBUG(7, "altimetry.r[0]: %lf, ri: %lf, altimetry.r[N]: %lf; \n", settings->altimetry.r[0], ri, settings->altimetry.r[settings->altimetry.numSurfaceCoords -1]);
		DEBUG(7, "batimetry.r[0]: %lf, ri: %lf, batimetry.r[N]: %lf; \n", settings->batimetry.r[0], ri, settings->batimetry.r[settings->batimetry.numSurfaceCoords -1]);
		if (	(ri > settings->altimetry.r[0]) &&
				(ri < settings->altimetry.r[settings->altimetry.numSurfaceCoords -1]) &&
				(ri > settings->batimetry.r[0]) &&
				(ri < settings->batimetry.r[settings->batimetry.numSurfaceCoords -1] ) ){
			DEBUG(7, "Calculate surface and bottom z at current ray position: \n");
			boundaryInterpolation(	&(settings->altimetry), ri, &altInterpolatedZ, &junkVector, &normal);
			boundaryInterpolation(	&(settings->batimetry), ri, &batInterpolatedZ, &junkVector, &normal);
		}else{
			DEBUG(8,"ray killed\n");
			ray->iKill = TRUE;
		}
		DEBUG(9,"altInterpolatedZ: %lf\n", altInterpolatedZ);
		DEBUG(9,"batInterpolatedZ: %lf\n", batInterpolatedZ);
		DEBUG(7, "Check if the ray is still between the boundaries; if not, find the intersection point and calculate the reflection: \n");
		if ((ray->iKill == FALSE ) && (zi <= altInterpolatedZ || zi >= batInterpolatedZ)){
			pointA.r = yOld[0];
			pointA.z = yOld[1];
			pointB.r = yNew[0];
			pointB.z = yNew[1];
			
			DEBUG(7, "Ray above surface? \n");
			if (zi <= altInterpolatedZ){
				DEBUG(5,"ray above surface.\n");
				//determine the coordinates of the ray-boundary intersection:
				rayBoundaryIntersection(&(settings->altimetry), &pointA, &pointB, &pointIsect);
				ri = pointIsect.r;
				zi = pointIsect.z;
				//verify if the intersection point is identical to the first point:
				/*
				if(pointIsect.r == pointA.r && pointIsect.z == pointA.z){
					fatal("points coincide.");
				}
				*/
				
				//get the boundary's normal and tangent vector:
				boundaryInterpolation(	&(settings->altimetry), ri, &altInterpolatedZ, &tauB, &normal);
				ibdry = -1;
				sRefl = sRefl + 1;
				jRefl = 1;
				
				DEBUG(7, "Calculate surface reflection: \n");
				specularReflection(&normal, &es, &tauR, &thetaRefl);
				
				DEBUG(7, "Check if the ray is 'digging in' beyond the surface: \n");
				//before we check if the next step's depth is above the surface, we need to check if it's range is withing the rangebox:
				DEBUG(8, "Testing step: ri+settings->source.ds*tauR.r: %lf\n", ri+settings->source.ds*tauR.r);
				if(ri+settings->source.ds*tauR.r < settings->source.rbox1 || ri+settings->source.ds*tauR.r > settings->source.rbox2){
					DEBUG(5, "Next step is outside of rangeBox => terminate the ray.\n");
					ray->iKill = TRUE;
				}else{
					boundaryInterpolation(	&(settings->altimetry), ri+settings->source.ds*tauR.r, &altInterpolatedZ, &tauB, &normal);
					if ( (zi + settings->source.ds*tauR.z) < altInterpolatedZ){
						DEBUG(5, "Ray is digging in above surface: %lf\n", ray->theta);
						ray->iKill = TRUE;
					}
				}
				
				// calculate the reflection coefficient, depending of interface type:
				DEBUG(7, "Get the reflection coefficient (kill the ray if the surface is an absorver): \n");
				switch(settings->altimetry.surfaceType){
					
					case SURFACE_TYPE__ABSORVENT:	//"A"
						reflCoeff = 0 +0*I;
						ray->iKill = TRUE;
						break;
						
					case SURFACE_TYPE__RIGID:		//"R"
						reflCoeff = 1 +0*I;
						break;
						
					case SURFACE_TYPE__VACUUM:		//"V"
						reflCoeff = -1 +0*I;
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
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &reflCoeff);
								break;
							
							case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:	//"N"
								//Non-Homogeneous interface =>rho, cp, cs, ap, as are variant with range, and thus have to be interpolated
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.rho,
																&(settings->altimetry.surfaceInterpolation),
																ri,
																&rho2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.cp,
																&(settings->altimetry.surfaceInterpolation),
																ri,
																&cp2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.cs,
																&(settings->altimetry.surfaceInterpolation),
																ri,
																&cs2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.ap,
																&(settings->altimetry.surfaceInterpolation),
																ri,
																&ap,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->altimetry.numSurfaceCoords),
																settings->altimetry.r,
																settings->altimetry.as,
																&(settings->altimetry.surfaceInterpolation),
																ri,
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
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &reflCoeff);
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
				//apply the reflection coefficient:
				reflDecay *= reflCoeff;
				
				DEBUG(7, "Kill the ray if the reflection coefficient is too small: \n");
				if ( cabs(reflCoeff) < MIN_REFLECTION_COEFFICIENT ){
					ray->iKill = TRUE;
				}
			//	end of "ray above surface?"
			}else if (zi >= batInterpolatedZ){	//	Ray below bottom?
				DEBUG(5,"ray below bottom.\n");
				DEBUG(8,"ri: %lf, zi: %lf\n", ri, zi);
				rayBoundaryIntersection(&(settings->batimetry), &pointA, &pointB, &pointIsect);
				ri = pointIsect.r;
				zi = pointIsect.z;
				
				DEBUG(8,"ri: %lf, zi: %lf\n", ri, zi);
				boundaryInterpolation(	&(settings->batimetry), ri, &batInterpolatedZ, &tauB, &normal);
				//Invert the normal at the bottom for reflection:
				normal.r = -normal.r;	//NOTE: differs from altimetry
				normal.z = -normal.z;	//NOTE: differs from altimetry
				
				ibdry = 1;			
				bRefl = bRefl + 1;
				jRefl = 1;
				
				DEBUG(7, "Calculate surface reflection: \n");
				specularReflection(&normal, &es, &tauR, &thetaRefl);

				DEBUG(7, "Check if the ray is 'digging in' beyond the bottom: \n");
				//before we check if the next step's depth is below the bottom, we need to check if it's range is withing the rangebox:
				DEBUG(8, "Testing step: ri+settings->source.ds*tauR.r: %lf\n", ri+settings->source.ds*tauR.r);
				if(ri+settings->source.ds*tauR.r < settings->source.rbox1 || ri+settings->source.ds*tauR.r > settings->source.rbox2){
					DEBUG(5, "Next step is outside of rangeBox => terminate the ray.\n");
					ray->iKill = TRUE;
				}else{
					boundaryInterpolation(	&(settings->batimetry), ri+settings->source.ds*tauR.r, &batInterpolatedZ, &tauB, &normal);
					if ( (zi + settings->source.ds*tauR.z) > batInterpolatedZ){
						DEBUG(5, "Ray is digging in below bottom: %lf\n", ray->theta);
						ray->iKill = TRUE;
					}
				}
				
				DEBUG(7, "Get the reflection coefficient (kill the ray if the surface is an absorver): \n");
				switch(settings->batimetry.surfaceType){
					
					case SURFACE_TYPE__ABSORVENT:	//"A"
						reflCoeff = 0 +0*I;
						ray->iKill = TRUE;
						break;
						
					case SURFACE_TYPE__RIGID:		//"R"
						reflCoeff = 1 +0*I;
						break;
						
					case SURFACE_TYPE__VACUUM:		//"V"
						reflCoeff = -1 +0*I;
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
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &reflCoeff);
								break;
							
							case SURFACE_PROPERTY_TYPE__NON_HOMOGENEOUS:	//"N"
								//Non-Homogeneous interface =>rho, cp, cs, ap, as are variant with range, and thus have to be interpolated
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.rho,
																&(settings->batimetry.surfaceInterpolation),
																ri,
																&rho2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.cp,
																&(settings->batimetry.surfaceInterpolation),
																ri,
																&cp2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.cs,
																&(settings->batimetry.surfaceInterpolation),
																ri,
																&cs2,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.ap,
																&(settings->batimetry.surfaceInterpolation),
																ri,
																&ap,
																&junkVector,
																&junkVector
															);
								boundaryInterpolationExplicit(	&(settings->batimetry.numSurfaceCoords),
																settings->batimetry.r,
																settings->batimetry.as,
																&(settings->batimetry.surfaceInterpolation),
																ri,
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
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &reflCoeff);
								break;
							default:
								fatal("Unknown surface properties (neither H or N).\nAborting...");
								break;
							}
						break;
					default:
						fatal("Unknown surface type (neither A,E,R or V).\nAborting...");
						break;
				}//switch(settings->batimetry.surfaceType)
				
				reflDecay *= reflCoeff;
				
				DEBUG(7,"decay: %lf, abs(reflDecay): %lf, reflCoeff: %lf\n", cabs(ray->decay[i]), cabs(reflDecay), cabs(reflCoeff));
				//Kill the ray if the reflection coefficient is too small: 
				if ( cabs(reflCoeff) < MIN_REFLECTION_COEFFICIENT ){
					ray->iKill = TRUE;
					DEBUG(2, "Ray killed ( abs(reflCoeff) < 1e-5 )\n");
				}
			}	//if (zi > batInterpolatedZ) (	Ray below bottom?)

			DEBUG(6, "Update marching solution and function: \n");
			ri = pointIsect.r;
			zi = pointIsect.z;
			csValues( 	settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
			yNew[0] = ri;
			yNew[1] = zi;
			yNew[2] = sigmaI*tauR.r;
			yNew[3] = sigmaI*tauR.z;

			fNew[0] = tauR.r;
			fNew[1] = tauR.z;
			fNew[2] = slowness.r;
			fNew[3] = slowness.z;
		}	//if ((ray->iKill == FALSE ) && (zi < altInterpolatedZ || zi > batInterpolatedZ))
		
		/*************************
		 *  Object reflection:
		 *************************/
		DEBUG(5, "Check for object reflection: \n");
		if (settings->objects.numObjects > 0){
 			for(j=0; j<settings->objects.numObjects; j++){
				nObjCoords = settings->objects.object[j].nCoords;
				
				DEBUG(7, "For each object detect if the ray is inside the object range: \n");
				if (	(ri >=	settings->objects.object[j].r[0] ) &&
						(ri <	settings->objects.object[j].r[nObjCoords-1])){
					DEBUG(7, "Ray in object range.\n");
					
					if ( settings->objects.object[j].zDown[0] != settings->objects.object[j].zUp[0]){
						fatal("Lower and upper object boundaries do not start at the same depth!\nAborting...");
					}
					boundaryInterpolationExplicit(	&nObjCoords,
													settings->objects.object[j].r,
													settings->objects.object[j].zDown,
													&settings->objects.surfaceInterpolation,
													ri,
													&ziDown,
													&junkVector,
													&normal);
					boundaryInterpolationExplicit(	&nObjCoords,
													settings->objects.object[j].r,
													settings->objects.object[j].zUp,
													&settings->objects.surfaceInterpolation,
													ri,
													&ziUp,
													&junkVector,
													&normal);
					DEBUG(5,"ri: %lf, ziDown: %lf, ziUp: %lf\n",ri, ziDown, ziUp);
					//Second point is inside the object?
					if (	(yNew[1] >=	ziDown 	) &&
							(yNew[1] <=	ziUp   )){
						DEBUG(3, "2nd point inside object.\n");
						pointA.r = yOld[0];
						pointA.z = yOld[1];
						pointB.r = yNew[0];
						pointB.z = yNew[1];

						//	Which face was crossed by the ray: upper or lower?
						//	Since we alrady know that the second point is inside the object, we only have to test if the first one is
						//	above or below the object

						//	Case 1: from left to right, beginning inside box & ending in box:
						if (	yOld[0] <	yNew[0]	&&
								yOld[0] >=	settings->objects.object[j].r[0] &&
								yNew[0] <=	settings->objects.object[j].r[nObjCoords-1] ){
							
							DEBUG(5,"Case 1: from left to right, beginning inside box & ending inside box\n");
							DEBUG(7,"ri:%lf\n", yOld[0]);
							boundaryInterpolationExplicit(	&nObjCoords,
															settings->objects.object[j].r,
															settings->objects.object[j].zUp,
															&settings->objects.surfaceInterpolation,
															yOld[0],
															&ziUp,
															&junkVector,
															&normal);
							boundaryInterpolationExplicit(	&nObjCoords,
															settings->objects.object[j].r,
															settings->objects.object[j].zDown,
															&settings->objects.surfaceInterpolation,
															yOld[0],
															&ziDown,
															&junkVector,
															&normal);
							DEBUG(7,"ri: %lf, ziDown: %lf, ziUp: %lf\n",ri, ziDown, ziUp);
							if (yOld[1] < ziDown){
								rayObjectIntersection(&settings->objects, &j, DOWN, &pointA, &pointB, &pointIsect);
								ibdry = -1;
							}else{
								rayObjectIntersection(&settings->objects, &j, UP, &pointA, &pointB, &pointIsect);
								ibdry = 1;
							}

						//	Case 2: from left to right, beginning outside box & ending inside:
						}else if (	yOld[0] <	settings->objects.object[j].r[0] &&
									yNew[0] <	settings->objects.object[j].r[nObjCoords-1]){
							
							DEBUG(5,"Case 2: from left to right, beginning outside box & ending inside:\n");
							//Calculate the coords for point A in relation to the beginning of the box
							//Q: why do this?
							//R: to be able to determine if these coords are above or below the object.
							pointA.z = pointB.z -(pointB.z - pointA.z) / (pointB.r - pointA.r) * (pointB.r - settings->objects.object[j].r[0]);
							pointA.r = settings->objects.object[j].r[0];
							if (pointA.z < settings->objects.object[j].zUp[0]){
								rayObjectIntersection(&settings->objects, &j, DOWN, &pointA, &pointB, &pointIsect);
								ibdry = -1;
							}else{
								rayObjectIntersection(&settings->objects, &j, UP, &pointA, &pointB, &pointIsect);
								ibdry =  1;
							}
							DEBUG(5, "Leaving case 2\n");

						//	Case 3: from right to left, beginning inside of box and ending inside:
						}else if(	yOld[0] >	yNew[0]	&&
									yOld[0] <=	settings->objects.object[j].r[ nObjCoords-1 ] &&
									yNew[0] >=	settings->objects.object[j].r[0]){

							DEBUG(5,"Case 3: from right to left, beginning outside of box and ending inside:\n");
							boundaryInterpolationExplicit(	&nObjCoords,
															settings->objects.object[j].r,
															settings->objects.object[j].zUp,
															&settings->objects.surfaceInterpolation,
															yOld[0],
															&ziUp,
															&junkVector,
															&normal);
							boundaryInterpolationExplicit(	&nObjCoords,
															settings->objects.object[j].r,
															settings->objects.object[j].zDown,
															&settings->objects.surfaceInterpolation,
															yOld[0],
															&ziDown,
															&junkVector,
															&normal);
							DEBUG(7,"ri: %lf, ziDown: %lf, ziUp: %lf\n",ri, ziDown, ziUp);
							if (yOld[1] < ziDown){
								rayObjectIntersection(&settings->objects, &j, DOWN, &pointA, &pointB, &pointIsect);
								ibdry = -1;
							}else{
								rayObjectIntersection(&settings->objects, &j, UP, &pointA, &pointB, &pointIsect);
								ibdry = 1;
							}

						//	Case 4: from right to left, beginning outside of box and ending inside:
						}else if(	yOld[0] >	settings->objects.object[j].r[ nObjCoords-1 ] &&
									yNew[0] >=	settings->objects.object[j].r[0]){

							//Calculate the coords for point A in relation to the end of the box
							//Q: why do this?
							//R: to be able to determine if these coords are above or below the object.
							pointA.z = pointB.z -(pointB.z - pointA.z) / (pointB.r - pointA.r) * (pointB.r - settings->objects.object[j].r[ nObjCoords-1 ]);
							pointA.r = settings->objects.object[j].r[ nObjCoords-1 ];
							if (pointA.z < settings->objects.object[j].zUp[ nObjCoords-1 ]){
								rayObjectIntersection(&settings->objects, &j, DOWN, &pointA, &pointB, &pointIsect);
								ibdry = -1;
							}else{
								rayObjectIntersection(&settings->objects, &j, UP, &pointA, &pointB, &pointIsect);
								ibdry =  1;
							}
							
						//Some weird error this would be:
						}else{
							fatal("Object reflection case: ray beginning neither behind or between object box.\nCheck object coordinates.\nAborting...");
						}

						ri = pointIsect.r;
						zi = pointIsect.z;

						DEBUG(7, "ri: %lf, zi: %lf\n", ri, zi);

						//Face reflection: upper or lower? 
						if (	ibdry == -1 ){	
							boundaryInterpolationExplicit(	&nObjCoords,
															settings->objects.object[j].r,
															settings->objects.object[j].zDown,
															&settings->objects.surfaceInterpolation,
															ri,
															&ziDown,
															&tauB,
															&normal);
							//invert surface normal:
							normal.r = -normal.r;
							normal.z = -normal.z;
						}else if (	ibdry == 1	){
							boundaryInterpolationExplicit(	&nObjCoords,
															settings->objects.object[j].r,
															settings->objects.object[j].zUp,
															&settings->objects.surfaceInterpolation,
															ri,
															&ziUp,
															&tauB,
															&normal);
						}else{
							fatal("Object reflection case: ray neither being reflected on down or up faces.\nCheck object coordinates.\nAborting...");
						}
						oRefl = oRefl + 1;
						jRefl = 1;

						//	Calculate object reflection:
						specularReflection(&normal, &es, &tauR, &thetaRefl);

						//	Object reflection => get the reflection coefficient
						//	(kill the ray is the object is an absorver):

						switch(settings->objects.object[j].surfaceType){
							case SURFACE_TYPE__ABSORVENT:	//"A"
								DEBUG(5, "Object: SURFACE_TYPE__ABSORVENT\n");
								reflCoeff = 0 +0*I;
								ray->iKill = TRUE;
								break;
						
							case SURFACE_TYPE__RIGID:		//"R"
								DEBUG(5, "Object: SURFACE_TYPE__RIGID\n");
								reflCoeff = 1 +0*I;
								break;
								
							case SURFACE_TYPE__VACUUM:		//"V"
								DEBUG(5, "Object: SURFACE_TYPE__VACUUM\n");
								reflCoeff = -1 +0*I;
								break;
								
							case SURFACE_TYPE__ELASTIC:		//"E"
								DEBUG(5, "Object: SURFACE_TYPE__ELASTIC\n");
								rho2= settings->objects.object[j].rho;
								cp2	= settings->objects.object[j].cp;
								cs2	= settings->objects.object[j].cs;
								ap	= settings->objects.object[j].ap;
								as	= settings->objects.object[j].as;
								lambda = cp2 / settings->source.freqx;
								convertUnits(	&ap,
												&lambda,
												&(settings->source.freqx),
												&(settings->objects.object[j].surfaceAttenUnits),
												&tempDouble
											);
								ap		= tempDouble;
								lambda	= cs2 / settings->source.freqx;
								convertUnits(	&as,
												&lambda,
												&(settings->source.freqx),
												&(settings->objects.object[j].surfaceAttenUnits),
												&tempDouble
											);
								as		= tempDouble;
								DEBUG(6, "Calculating reflection coefficient...\n");
								boundaryReflectionCoeff(&rho1, &rho2, &ci, &cp2, &cs2, &ap, &as, &thetaRefl, &reflCoeff);
								DEBUG(6, "Reflection coefficient calculated\n");
								break;
							
							default:
								fatal("Unknown object boundary type (neither A,E,R or V).\nAborting...");
								break;
						}
						
						reflDecay *= reflCoeff;
						DEBUG(7, "Object: Reflection decay calculated: %lf +j* %lf\n", creal(reflDecay), cimag(reflDecay));

						//Kill the ray if the reflection coefficient is too small: 
						if ( cabs(reflCoeff) < MIN_REFLECTION_COEFFICIENT ){
							ray->iKill = TRUE;
						}
						
						//Update marching solution and function:
						es.r = tauR.r;
						es.z = tauR.z;
						e1.r = -es.z;
						e1.z =  es.r;
						DEBUG(7, "Calculating sound speed parameters for next step...\n");
						csValues( 	settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
						DEBUG(7, "Sound speed parameters for next step calculated.\n");
						
						yNew[0] = ri;
						yNew[1] = zi;
						yNew[2] = sigmaI*es.r;
						yNew[3] = sigmaI*es.z;

						fNew[0] = es.r;
						fNew[1] = es.z;
						fNew[2] = slowness.r;
						fNew[3] = slowness.z;
					}
				}
			}
		} 
		
		
		/*	prepare for next loop	*/
		ri			= yNew[0];
		zi			= yNew[1];
		ray->r[i+1] = yNew[0];
		ray->z[i+1] = yNew[1];
		
		es.r = fNew[0];
		es.z = fNew[1];
		csValues( 	settings, ri, zi, &ci, &cc, &sigmaI, &cri, &czi, &slowness, &crri, &czzi, &crzi);
		
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
			ray->phase[i+1] = ray->phase[i] - atan2( cimag(reflCoeff), creal(reflCoeff) );
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
				//when debugging, save the coordinates of the last ray to a separate matfile before exiting.
				mxArray*	pRay	= NULL;
				MATFile*	matfile	= NULL;
				double**	temp2D 	= malloc(2*sizeof(uintptr_t));
				char* 		string	= mallocChar(10);
				
				temp2D[0]	= ray->r;
				temp2D[1]	= ray->z;
				matfile		= matOpen("dyingRay.mat", "w");
				pRay		= mxCreateDoubleMatrix((MWSIZE)2, (MWSIZE)ray->nCoords, mxREAL);
				if(pRay == NULL || matfile == NULL)
					fatal("Memory alocation error.");
				copyDoubleToPtr2D(temp2D, mxGetPr(pRay), ray->nCoords,2);

				sprintf(string, "dyingRay");
				matPutVariable(matfile, (const char*)string, pRay);
				mxDestroyArray(pRay);
				matClose(matfile);
				freeDouble2D(temp2D, 2);
			#endif
			fatal(	"Ray step too small, number of points in ray coordinates exceeds allocated memory.\n"
					"Note that in cases where neither surface nor bottom have attenuation, rays can be endlessly reflected up and down and become \"trapped\".\n"
					"If you need a high number or reflections per ray, you may also try changing MEM_FACTOR (in globals.h) to a higher value and recompile.\n"
					"Aborting...");
			//double the memory allocated for the ray:
			//TODO find bus error that happens on realloc when realloccing to a larger value (toolsMemory.c".)
			//reallocRay(ray, ray->nCoords * 2);		//TODO disabled as this sometimes results in "bus error".
			
		}
	}

	/*	Ray coordinates have been computed. Finalizing */
	//We still need to calculate the exact coords in case the last point is outside [rbox1,rbox2], so we add 1
	ray -> nCoords	= i+1;
	//reallocRayMembers(ray, i+1);

	//save reflection counters to ray struct, (these values are later used in calcAllRayInfo())
	ray->sRefl = sRefl;
	ray->bRefl = bRefl;
	ray->oRefl = oRefl;
	ray->nRefl = sRefl + bRefl + oRefl;
	
	//Cut the ray at box exit:
	dr	= ray->r[ray->nCoords - 1] - ray->r[ray->nCoords-2];
	dz	= ray->z[ray->nCoords - 1] - ray->z[ray->nCoords-2];
	dIc	= ray->ic[ray->nCoords - 1] -ray->ic[ray->nCoords-2];

	if (ray->r[ray->nCoords-1] > settings->source.rbox2){
		//ray has exited rangebox at right edge
		ray->z[ray->nCoords-1]	= ray->z[ray->nCoords-2] + (settings->source.rbox2 - ray->r[ray->nCoords-2])* dz/dr;
		ray->ic[ray->nCoords-1]	= ray->ic[ray->nCoords-2] + (settings->source.rbox2 - ray->r[ray->nCoords-2])* dIc/dr;
		ray->r[ray->nCoords-1]	= settings->source.rbox2;
		
		//adjust memory size of the ray (we don't need more memory than nCoords)
		reallocRayMembers(ray, ray->nCoords);
		
	}else if (ray->r[ray->nCoords-1] < settings->source.rbox1){
		//ray has exited rangebox at left edge
		ray->z[ray->nCoords-1]	= ray->z[ray->nCoords-2] + (settings->source.rbox1-ray->r[ray->nCoords-2])* dz/dr;
		ray->ic[ray->nCoords-1]	= ray->ic[ray->nCoords-2] + (settings->source.rbox1-ray->r[ray->nCoords-2]) * dIc/dr;
		ray->r[ray->nCoords-1]	= settings->source.rbox1;
		
		//adjust memory size of the ray (we don't need more memory than nCoords)
		reallocRayMembers(ray, ray->nCoords);
		
	}else{
		//the last coordinate isn't outside the rangebox, so we clip the extra coordinate
		reallocRayMembers(ray, ray->nCoords-1);
	}

	
	/* Search for refraction points (refraction angles are zero!), rMin, rMax and twisting(returning) of rays:	*/
	//NOTE: We are assuming (safely) that there can't be more refraction points than the ray has coordinates,
	//		so we can skip memory bounds-checking.
	ray->nRefrac = 0;
	for(i=1; i<ray->nCoords-2; i++){
		ray->rMin = min( ray->rMin, ray->r[i] );
		ray->rMax = max( ray->rMax, ray->r[i] );
		prod = ( ray->z[i+1] - ray->z[i] )*( ray->z[i] - ray->z[i-1] );
		
		if ( (prod < 0) && (ray->iRefl[i] != 1) ){
			ray->nRefrac++;
			ray->rRefrac[ray->nRefrac - 1] = ray->r[i];
			ray->zRefrac[ray->nRefrac - 1] = ray->z[i];
		}
		
		prod = ( ray->r[i+1]-ray->r[i] )*( ray->r[i]-ray->r[i-1] );
		if ( prod < 0 ){
			ray->iReturn = TRUE;
		}
	}
	
	
	//check last set of coordinates for min and max values of range. (note that the last return condition has already been tested)
	ray-> rMin = min( ray->rMin, ray->r[ray->nCoords-1] );
	ray-> rMax = max( ray->rMax, ray->r[ray->nCoords-1] );
	i++;
	
	//clip allocated memory for refractions
	ray->rRefrac = reallocDouble(ray->rRefrac, ray->nRefrac);
	ray->zRefrac = reallocDouble(ray->zRefrac, ray->nRefrac);
	
	//free memory
	free(yOld);
	free(fOld);
	free(yNew);
	free(fNew);
	DEBUG(1,"out\n");
}
