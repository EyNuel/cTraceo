/************************************************************************
 *  intLinear1D.c														*
 * 	(formerly "lini1d.for)												*
 * 	Perform linear interpolation at 1D.									*
 * 																		*
 *	originally written in FORTRAN by:									*
 *  					Orlando Camargo Rodriguez:						*
 *						Copyright (C) 2010								*
 * 						Orlando Camargo Rodriguez						*
 *						orodrig@ualg.pt									*
 *						Universidade do Algarve							*
 *						Physics Department								*
 *						Signal Processing Laboratory					*
 *																		*
 *	Ported to C by:		Emanuel Ey										*
 *						emanuel.ey@gmail.com							*
 *						Signal Processing Laboratory					*
 *						Universidade do Algarve							*
 *																		*
 *	Inputs:																*
 * 				x:		vector containing x0, x1						*
 * 				f:		vector containing f0, f1						*
 * 				xi:		scalar (the interpolation point)				*
 * 	Outputs:																*
 * 				fi:		interpolated value of f at xi					*
 * 				fxi:	derivative of f at xi							*
 * 																		*
 ************************************************************************/

void intLinear1D(double*, double*, double*, double*, double*);

void intLinear1D(double* x, double* f, double* xi, double* fi, double* fxi){
	*fxi	= 	( f[1] -f[0]) / ( x[1] -x[0]);
	*fi		=	f[0] +(xi -x[0]) *(*fxi);
}

/*
       subroutine lini1d(x,y,xi,yi,yxi)

c***********************************************************************
c      Linear Interpolation 1D
c      Written by Orlando Camargo Rodriguez
c      orodrig@ualg.pt
c      Universidade do Algarve
c      Physics Department
c      Signal Processing Laboratory
c      Faro, 20/08/2009 at 19:30
c****&******************************************************************

       real*8 x(2),y(2)
       real*8 xi,yi,yxi
       real*8 x1,x2,y1,y2

c***********************************************************************

       x1 = x(1)
       x2 = x(2)

       y1 = y(1)
       y2 = y(2)

       yxi = (y2-y1)/(x2-x1)

       yi  = y1 + (xi-x1)*yxi

c***********************************************************************
c      Back to main:
c***********************************************************************

       return

       end
*/
