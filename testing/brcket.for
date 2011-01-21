       subroutine brcket(n,x,xi,i)
c***********************************************************************
c      BRaCKET subroutine 
c      Written by Orlando Camargo Rodriguez
c      orodrig@ualg.pt
c      Universidade do Algarve
c      Physics Department
c      Signal Processing Laboratory
c      Faro, 03/05/2009 at 19:30
c****&******************************************************************

       integer*8 i,n,ia,im,ib

       real*8 x(n)
       real*8 xi
       
c****&******************************************************************

       ia = 1
       ib = n

c=======================================================================

c      If xi is outside the interval [x(1) x(n)] just output zero and
c      return to the calling program:

       if ((xi.lt.x(1)).or.(xi.gt.x(n))) then

       i = 0

       return

       end if

c=======================================================================

c      If xi is inside the interval [x(1) x(n)] just divide it by half
c      until ib - ia = 1:

500    im = (ia+ib)/2

       if ((ib-ia).gt.1) then
       
          if ((xi.ge.x(ia)).and.(xi.lt.x(im))) then
           ib = im
          else
           ia = im
          end if
       
          goto 500
       
       end if

       i = im

       return

c=======================================================================

       end
