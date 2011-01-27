/************************************************************************************
 *	specularReflection.c															*
 *	(formerly "reflct.for")															*
 *	Calculate a vector's reflection of a surface.									*
 * 																					*
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
 * 				normal:	A pointer to the surface's normal vector.					*
 * 				tauI:	A pointer to the incident vector.							*
 *																					*
 * 	Outputs:																		*
 * 				tauR:	A pointer to the reflected vector.							*
 * 				theta:	The angle between the surface normal and the reflected		*
 * 						vector														*
 * 																					*
 ***********************************************************************************/

#include "globals.h"
#include "math.h"

void specularReflection(vector_t*, vector_t*, vector_t*, double*);
void specularReflection(vector_t* normal, vector_t* tauI, vector_t* tauR, double* theta){
	double	c = dotProduct(normal, tauI);

	tauR->r = tauI->r - 2*c * normal->r;
	tauR->z = tauI->z - 2*c * normal->z;

	*theta = acos( dotProduct(normal, tauR));
}
