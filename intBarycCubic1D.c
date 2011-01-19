/************************************************************************
 *	intBarycParab1D.c		 											*
 * 	(formerly "bcui1d.for")												*
 * 	Perform Barycentric Cubic interpolation at 1D.			    		*
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
 * 				x:		vector containing x0, x1, x2, x4				*
 * 				f:		vector containing f0, f1, f2, f4				*
 * 				xi:		scalar (the interpolation point)				*
 * 	Outputs:															*
 * 				fi:		interpolated value of f at xi					*
 * 				fxi:	1st derivative of f at xi						*
 * 				fxxi:	2nd derivative of f at xi						*
 * 																		*
 ************************************************************************/
#include <inttypes.h>

void intBarycCubic1D(double*, double*, double, double*, double*, double*);

void intBarycCubic1D(double* x, double* f, double xi, double* fi, double* fxi, double* fxxi){
	double		a[3],px[3],sx[3], qx[3];
	uint32_t	i;
	
	px[0] = ( x[1] - x[0] )*( x[1] - x[2] )*( x[1] - x[3] );
	px[1] = ( x[2] - x[0] )*( x[2] - x[1] )*( x[2] - x[3] );
	px[2] = ( x[3] - x[0] )*( x[3] - x[1] )*( x[3] - x[2] );

	for(i=0; i<=2; i++){
		a[i] = ( f[i+1] - f[0] )/px[i];
	}

	px[0] = ( xi - x[0] )*( xi - x[2] )*( xi - x[3] );
	px[1] = ( xi - x[0] )*( xi - x[1] )*( xi - x[3] );
	px[2] = ( xi - x[0] )*( xi - x[1] )*( xi - x[2] );
	
	sx[0] = (xi-x[0])*(xi-x[2]) + (xi-x[0])*(xi-x[3]) + (xi-x[2])*(xi-x[3]);
	sx[1] = (xi-x[0])*(xi-x[1]) + (xi-x[0])*(xi-x[3]) + (xi-x[1])*(xi-x[3]);
	sx[2] = (xi-x[0])*(xi-x[1]) + (xi-x[0])*(xi-x[2]) + (xi-x[1])*(xi-x[2]);
	
	qx[0] = 2*( 3*xi -x[0] -x[2] -x[3] );
	qx[1] = 2*( 3*xi -x[0] -x[1] -x[3] );
	qx[2] = 2*( 3*xi -x[0] -x[1] -x[2] );
	
	*fi   = f[0];
	*fxi  = 0.0;
	*fxxi = 0.0;
	
	for(i=0; i<=2; i++){
		*fi   += a[i]*px[i];
		*fxi  += a[i]*sx[i];
		*fxxi += a[i]*qx[i];
	}
}
/*


c***********************************************************************
       
       x1 = x(1)
       x2 = x(2)
       x3 = x(3)
       x4 = x(4)

       px(1) = ( x2 - x1 )*( x2 - x3 )*( x2 - x4 )
       px(2) = ( x3 - x1 )*( x3 - x2 )*( x3 - x4 )
       px(3) = ( x4 - x1 )*( x4 - x2 )*( x4 - x3 )
       
       do i = 1,3
       
          a(i) = ( f(i+1) - f(1) )/px(i)
       
       end do

       px(1) = ( xi - x1 )*( xi - x3 )*( xi - x4 )
       px(2) = ( xi - x1 )*( xi - x2 )*( xi - x4 )
       px(3) = ( xi - x1 )*( xi - x2 )*( xi - x3 )
       
       sx(1) = (xi-x1)*(xi-x3)+(xi-x1)*(xi-x4)+(xi-x3)*(xi-x4)
       sx(2) = (xi-x1)*(xi-x2)+(xi-x1)*(xi-x4)+(xi-x2)*(xi-x4)
       sx(3) = (xi-x1)*(xi-x2)+(xi-x1)*(xi-x3)+(xi-x2)*(xi-x3)
       
       qx(1) = 2*( 3*xi - x1 - x3 - x4 )
       qx(2) = 2*( 3*xi - x1 - x2 - x4 )
       qx(3) = 2*( 3*xi - x1 - x2 - x3 )
       
       fi   = f(1)
       fxi  = 0.0
       fxxi = 0.0
       
       do i = 1,3
       
          fi   = fi   + a(i)*px(i)
          fxi  = fxi  + a(i)*sx(i)
          fxxi = fxxi + a(i)*qx(i)
       
       end do

c***********************************************************************
c      Back to main:
c***********************************************************************        
       
       return 
       
       end
*/
