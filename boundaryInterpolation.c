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



void boundaryInterpolationExplicit(uint32_t*, double*, double*, uint32_t*, double*, double*, vector_t*, vector_t*);
void boundaryInterpolation(interface_t*, double*, double*, vector_t*, vector_t*);

void boundaryInterpolationExplicit(uint32_t* numSurfaceCoords, double* r, double* z, uint32_t* surfaceInterpolation, double* ri, double* zi, vector_t* taub, vector_t* normal){
	DEBUG(5,"in\n");
		
	double		zri = 0;	//1st derivative of z at ri
	double		zrri;	//2nd derivative of z at ri
	uintptr_t	i;
	
	switch(*surfaceInterpolation){
		case SURFACE_INTERPOLATION__FLAT:
			//lini1d(xl,yl,ri,zi,zri)
			intLinear1D( &(r[0]), &(z[0]),ri,zi,&zri);
			break;
			
		case SURFACE_INTERPOLATION__SLOPED:
			//lini1d(xl,yl,ri,zi,zri)
			intLinear1D( &(r[0]), &(z[0]),ri,zi,&zri);
			break;
			
		case SURFACE_INTERPOLATION__2P:
			bracket(*numSurfaceCoords, r, ri, &i);
			intLinear1D( &(r[i]), &(z[i]),ri,zi,&zri);
			break;
			
		case SURFACE_INTERPOLATION__3P:
			if (*ri <= r[1]){
				i = 0;
			}else if( *ri >= r[*numSurfaceCoords - 2]){
				i = *numSurfaceCoords-3;
			}else{
				bracket(*numSurfaceCoords, &(r[0]), ri, &i);
			}
			intBarycParab1D( &(r[i]), &(z[i]), ri, zi, &zri, &zrri);
			break;
			
		case SURFACE_INTERPOLATION__4P:
			if (*ri <= r[1]){
				i = 1;
			}else if(*ri >= r[*numSurfaceCoords - 2]){
				i = *numSurfaceCoords - 3;
			}else{
				bracket( *numSurfaceCoords, &(r[0]), ri, &i);
			}
			intBarycCubic1D( &(r[i-1]), &(z[i-1]), ri, zi, &zri, &zrri);
	}

	taub->r = cos( atan(zri));
	taub->z = sin( atan(zri));

	//Avoid rounding errors:
	if ( fabs(taub->r) == 1.0 ){
		taub->z = 0;
	}else if ( fabs(taub->z) == 1.0 ){
	   taub->r = 0;
	}
	
	normal->r = -taub->z; 
	normal->z =  taub->r;
	DEBUG(7,"out\n");
}

void boundaryInterpolation(interface_t* interface, double* ri, double* zi, vector_t* taub, vector_t* normal){
	boundaryInterpolationExplicit( 	&(interface->numSurfaceCoords),
									interface->r,
									interface->z,
									&(interface->surfaceInterpolation),
									ri, zi, taub, normal);
}
