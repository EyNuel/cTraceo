/************************************************************************************
 *	rayBoundaryIntersection.c														*
 *	(formerly "raybi.for")															*
 *	Determine the intersection point between a ray and a boundary					*
 * 	Used for surface, bottom as well as objects.									*
 *																					*
 *	originally written in FORTRAN by:												*
 *						Orlando Camargo Rodriguez:									*
 *						Copyright (C) 2010											*
 *						Orlando Camargo Rodriguez									*
 *						orodrig@ualg.pt												*
 *						Universidade do Algarve										*
 *						Physics Department											*
 *						Signal Processing Laboratory								*
 *																					*
 *	Ported to C for project SENSOCEAN by:											*
 * 						Emanuel Ey													*
 *						emanuel.ey@gmail.com										*
 *						Signal Processing Laboratory								*
 *						Universidade do Algarve										*
 *																					*
 *	Inputs:																			*
 * 				interface:	A pointer of type interface_t, containing the			*
 * 							parameters of the interface.							*
 * 				a:			Coords (r, z) of 1st point.								*
 * 				b:			Coords (r, z) of 2nd point (on the opposite side of the	*
 * 							boundary)												*
 *																					*
 * 	Outputs:																		*
 * 				isect:		Coords (r, z) of intesection point.						*
 * 																					*
 ***********************************************************************************/
#pragma once
#include "globals.h"
#include "tools.c"
#include "lineLineIntersec.c"
#include "linearSpaced.c"

#define		DOWN	-1
#define		UP		1

void	rayObjectIntersection(objects_t*, uint32_t*, int32_t, point_t*, point_t*, point_t*);
void	rayBoundaryIntersection(interface_t*, point_t*, point_t*, point_t*);

void	rayObjectIntersection(objects_t* objects, uint32_t* j, int32_t boundary, point_t* a, point_t* b, point_t* isect){
	/*
	 * maps an object to rayBoundaryIntersection()
	 */
	interface_t		tempInterface;
	
	tempInterface.numSurfaceCoords		= objects->object[*j].nCoords;
	tempInterface.r 					= objects->object[*j].r;
	switch(boundary){
		case DOWN:
			tempInterface.z = objects->object[*j].zDown;
			break;
		case UP:
			tempInterface.z = objects->object[*j].zUp;
			break;
		default:
			fatal("rayObjectIntersection(): unknown boundary");
			break;
	}
	tempInterface.surfaceInterpolation	= objects->surfaceInterpolation;
	rayBoundaryIntersection(&tempInterface, a, b, isect);
}
	

void	rayBoundaryIntersection(interface_t* interface, point_t* a, point_t* b, point_t* isect){
	DEBUG(5,"in\n");
	uint32_t	i,n;
	double		rl[101];
	double		zl[101];
	double		dz[101];
	point_t		q1;
	point_t		q2;
	vector_t	taub;
	vector_t	normal;
	
	switch(interface->surfaceInterpolation){
		case SURFACE_INTERPOLATION__FLAT:
			isect->z = interface->z[0];
			isect->r = (b->r - a->r) / (b->z - a->z) * (isect->z - a->z) + a->r;
			break;
		
		case SURFACE_INTERPOLATION__SLOPED:
			q1.r = interface->r[0];
			q1.z = interface->z[0];
			q2.r = interface->r[1];
			q2.z = interface->z[1];
			lineLineIntersec(a,b,&q1,&q2,&i,isect);
			break;
			
		default:
			DEBUG(7,"Neither a flat nor sloped boundary.\n");
			n = 101;
			linearSpaced(n, a->r, b->r, rl);
			linearSpaced(n, a->z, b->z, zl);
			
//TODO: tremendous potential for optimization here. A "paralel for" might do wonders
			//get first dz
			boundaryInterpolation(interface, rl[0], &isect->z, &taub, &normal);
			dz[0] = zl[0] - isect->z;
			for(i=1; i<n; i++){
				//ri = rl[i];
				//bdryi(nr,tabr,tabz,itype,ri,zi,taub,normal)
				boundaryInterpolation(interface, rl[i], &isect->z, &taub, &normal);
				dz[i] = zl[i] - isect->z;
				DEBUG(7,"dz0: %lf, dz[%u]: %lf | rl[%u]: %lf, rz[%u]: %lf\n", dz[0], i, dz[i], i, rl[i], i, zl[i]);
				if( (dz[0] * dz[i]) <= 0){
					//intersection point has been found, exit for-loop.
					DEBUG(7,"Intersection point found at (%lf, %lf)\n", isect->r, isect->z);
					break;
				}
			}
			DEBUG(7,"Intersection point found at (%lf, %lf)\n", isect->r, isect->z);
			//isect->r = rl[i-1] - dz[i-1] * (rl[i] - rl[i-1]) / (dz[i] - dz[i-1]);
			isect->r = rl[i-1] - dz[i-1] * (rl[i] - rl[i-1]) / (dz[i] - dz[i-1]);
			isect->z = (isect->r - a->r) / (b->r - a->r) * (b->z - a->z) + a->z;
			//prevent rounding errors:
			if (fabs(isect->z) < 1.0e-12){
				isect->z = 0.0;
			}
			break;
//TODO double check the last 20~ lines
	}
	DEBUG(5,"out\n");
}
