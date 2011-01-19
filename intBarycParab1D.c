/************************************************************************
 *	intBarycParab1D.c		 											*
 * 	(formerly "bpai1d.for")												*
 * 	Perform Barycentric Parabolic interpolation at 1D.					*
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
 * 				x:		vector containing x0, x1, x2					*
 * 				f:		vector containing f0, f1, f2					*
 * 				xi:		scalar (the interpolation point)				*
 * 	Outputs:															*
 * 				fi:		interpolated value of f at xi					*
 * 				fxi:	1st derivative of f at xi						*
 * 				fxxi:	2nd derivative of f at xi						*
 * 																		*
 ************************************************************************/

void intBarycParab1D(double*, double*, double, double*, double*, double*);

void intBarycParab1D(double* x, double* f, double xi, double* fi, double* fxi, double* fxxi){
	double		a1,a2,px1,px2,sx1,sx2;

	px1 = (x[1] -x[0]) * (x[1] -x[2]);
	px2 = (x[2] -x[0]) * (x[2] -x[1]);

	a1 = ( f[1] - f[0] )/px1;
	a2 = ( f[2] - f[0] )/px2;

	px1 = (xi - x[0]) * (xi - x[2]);
	px2 = (xi - x[0]) * (xi - x[1]);

	sx1 = 2*xi - x[0] - x[2];
	sx2 = 2*xi - x[0] - x[1];

	*fi 	= f[0]	+	a1*px1	+a2*px2;
	*fxi	= 			a1*sx1	+a2*sx2;
	*fxxi=			a1*2	+a2*2;
}
 
/*
       
       x1 = x(1)
       x2 = x(2)
       x3 = x(3)

       px(1) = ( x2 - x1 )*( x2 - x3 )
       px(2) = ( x3 - x1 )*( x3 - x2 )
       
       a(1) = ( f(2) - f(1) )/px(1)
       a(2) = ( f(3) - f(1) )/px(2)

       px(1) = ( xi - x1 )*( xi - x3 )
       px(2) = ( xi - x1 )*( xi - x2 )
       
       sx(1) = 2*xi - x1 - x3
       sx(2) = 2*xi - x1 - x2
       
         fi = f(1) + a(1)*px(1) +   a(2)*px(2)
        fxi =        a(1)*sx(1) +   a(2)*sx(2)
       fxxi =      2*a(1)       + 2*a(2)
       
*/
