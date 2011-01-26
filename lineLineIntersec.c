/************************************************************************************
 *	lineLineIntersec.c		 														*
 *	Calculate intersection point between two Lines (p and q) each defined by 2		*
 * 	points in space. Note that the intersection point is actual lines defined by	*
 * 	the points, rather than the line segments between the points, and can produce	*
 * 	an intersection point beyond the lengths of the individual line segments.		*
 * 																					*
 * 	Reference:	http://en.wikipedia.org/wiki/Line-line_intersection					*
 * 																					*
 *	originally written in FORTRAN by:												*
 *  					Orlando Camargo Rodriguez:									*
 *						Copyright (C) 2010											*
 * 						Orlando Camargo Rodriguez									*
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
 * 				p1:		Coords to first point of line p.							*
 * 				p2:		Coords to second point of line p.							*
 * 				q1:		Coords to first point of line q.							*
 * 				q2:		Coords to second point of line q.							*
 *																					*
 * 	Outputs:																		*
 * 				i:		Indicates wether the lines intersect.						*
 * 				isect:	Coordc of intersection point.								*
 * 																					*
 ***********************************************************************************/

#include "globals.h"

void	lineLineIntersec(point_t*, point_t*, point_t*, point_t*, uint32_t, point_t*);

void	lineLineIntersec(point_t* p1, point_t* p2, point_t* q1, point_t* q2, uint32_t i, point_t* isect){
	double		d;

	*i = 0;
	d = (p1->r - p2->r) * (q1->z - q2->z) - (p1->z - p2->z) * (q1->r - q2->r);

	if( abs(d) > 1e-16){
		*i = 1

		isect->x = ( (p1->r * p2->z - p1->z * p2->r) * (q1->r - q2->r) - (p1->r - p2->r) * (q1->r * q2->z - q1->z * q2->r) )/d;
		isect->z = ( (p1->r * p2->z - p1->z * p2->r) * (q1->z - q2->z) - (p1->z - p2->z) * (q1->r * q2->z - q1->z * q2->r) )/d;
	}
}
/*****************
 * Original code:
	*
	x1 = p1(1)
	y1 = p1(2)
	x2 = p2(1)
	y2 = p2(2)
	x3 = q1(1)
	y3 = q1(2)
	x4 = q2(1)
	y4 = q2(2)
	
	d = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4)
	
	if (abs(d).gt.1e-16) then
		i = 1
		xi = ( (x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4) )/d
		yi = ( (x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4) )/d
	end if
*/
