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
 * 				p1:		Vector containing coords to first point of line p.			*
 * 				p2:		Vector containing coords to second point of line p.			*
 * 				q1:		Vector containing coords to first point of line q.			*
 * 				q2:		Vector containing coords to second point of line q.			*
 *																					*
 * 	Outputs:																		*
 * 				i:		Indicates wether the lines intersect.						*
 * 				xi:		x coordinate of intersection.								*
 * 				yi:		y coordinate of intersection.								*
 * 																					*
 ***********************************************************************************/

void	lineLineIntersec(double*, double*, double*, double*, uint32_t, double*, double*);

void	lineLineIntersec(double* p1, double* p2, double* q1, double* q2, uint32_t i, double* xi, double* yi){
	double		d;

	*i = 0;
	d = (p1[0] - p2[0]) * (q1[1] - q2[1]) - (p1[1] - p2[1]) * (q1[0] - q2[0]);

	if( abs(d) > 1e-16){
		i = 1

		xi = ( (p1[0]*p2[1]-p1[1]*p2[0])*(q1[0]-q2[0])-(p1[0]-p2[0])*(q1[0]*q2[1]-q1[1]*q2[0]) )/d
		yi = ( (p1[0]*p2[1]-p1[1]*p2[0])*(q1[1]-q2[1])-(p1[1]-p2[1])*(q1[0]*q2[1]-q1[1]*q2[0]) )/d
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
