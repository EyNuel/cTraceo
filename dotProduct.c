/************************************************************************************
 *	dotProduct.c																	*
 *	(formerly "dot.for")															*
 *	Calculate the dot product of 2 vectors.											*
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
 * 				u:		A pointer to a vector.										*
 * 				v:		A pointer to a vector.										*
 *																					*
 * 	Outputs:																		*
 * 						the dot product.											*
 * 																					*
 ***********************************************************************************/

double	dotProduct(vector_t*, vector_t*);

double	dotProduct(vector_t* u, vector_t* v){
	return( u->r * v->r + u->z * v->z);
}
