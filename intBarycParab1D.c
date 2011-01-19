/************************************************************************
 *  Linear Interpolation 1D												*
 * 	Perform linear interpolation.										*
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
 ************************************************************************/
/*
        subroutine bpai1d(x,f,xi,fi,fxi,fxxi)
	  
c***********************************************************************
c      Barycentric Parabolic Interpolation 1D 
c      Written by Orlando Camargo Rodriguez
c      orodrig@ualg.pt
c      Universidade do Algarve
c      Physics Department
c      Signal Processing Laboratory
c      Faro, 13/04/2009 at 14:30
c****&******************************************************************
       
       integer*8 i
       real*8 x(3),f(3)
       real*8 a(2),px(2),sx(2)
       real*8 xi,fi,fxi,fxxi
       real*8 x1,x2,x3
       
c***********************************************************************
       
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
       
c***********************************************************************
c     Back to main:
c***********************************************************************        
       
       return 
       
       end
*/
