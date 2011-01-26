/************************************************************************************
 *	boundaryInterpolation.c															*
 *	(formerly "bdryi.for")															*
 *	Interpolate depth of unknown boundary points.									*
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
 * 				ri:			range of interpolation point.							*
 *																					*
 * 	Outputs:																		*
 * 				zi:			interpolated depth at interpolation point.				*
 * 				taub:		surface's tangent vector at intersection.				*
 * 				normal:		surface's normal vector at intersection point.			*
 * 																					*
 ***********************************************************************************/

#include "globals.h"
#include "interpolation.h"

void boundaryInterpolation(interface_t*, double*, double*, vector_t*, vector_t*);

/*
	   subroutine bdryi(n,tabr,tabz,itype,ri,zi,taub,normal)
 	   real*8 tabr(n),tabz(n)
	   real*8 xc(4),yc(4)
	   real*8 xp(3),yp(3)
	   real*8 xl(2),yl(2)
	   real*8 taub(2),normal(2)
	   real*8 ri,zi,zr,zrr
 */

void boundaryInterpolation(interface_t* interface, double* ri, double* zi, vector_t* taub, vector_t* normal){

	double*		zri =	mallocDouble(1);	//1st derivative of z at ri
	double*		zrri =	mallocDouble(1);	//2nd derivative of z at ri
	uintptr_t	i;
	
	switch(interface->surfaceInterpolation){
		case SURFACE_INTERPOLATION__FLAT:
			//lini1d(xl,yl,ri,zi,zri)
			intLinear1D(&(interface->r[0]), &(interface->z[0]),ri,zi,zri);
			break;
			
		case SURFACE_INTERPOLATION__SLOPED:
			//lini1d(xl,yl,ri,zi,zri)
			intLinear1D(&(interface->r[0]), &(interface->z[0]),ri,zi,zri);
			break;
			
		case SURFACE_INTERPOLATION__2P:
			bracket(interface->numSurfaceCoords, &(interface->r[0]), ri, &i);
			intLinear1D(&(interface->r[i]), &(interface->z[i]),ri,zi,zri);
			break;
			
		case SURFACE_INTERPOLATION__3P:
			if (*ri <= interface->r[1]){
				i = 0;
			}else if( ri >= interface->r[interface->numSurfaceCoords - 2]){
				i = interface->numSurfaceCoords-3;
			}else{
				bracket(interface->numSurfaceCoords, &(interface->r[0]), ri, &i);
			}
			intBarycParab1D(&(interface->r[i]), &(interface->z[i]), ri, zi, zri, zrri);
			break;
			
		case SURFACE_INTERPOLATION__4P:
			if (*ri <= interface->r[1]){
				i = 1;
			}else if(*ri >= interface->r[interface->numSurfaceCoords - 2]){
				i = interface->numSurfaceCoords - 3;
			}else{
				bracket(interface->numSurfaceCoords, &(interface->r[0]), ri, &i);
			}
			intBarycCubic1D(&(interface->r[i-1]), &(interface->z[i-1]), ri, zi, zri, zrri);
	}

	taub->r = cos( atan(zri));
	taub->z = sin( atan(zri));

	//Avoid rounding errors:
	if ( abs(taub->r) == 1.0 ){
		taub->z = 0;
	}else if ( abs(taub->z) == 1.0 ){
	   taub->r = 0;
	}
	
	normal->r = -taub->z; 
	normal->z =  taub->r;
}


