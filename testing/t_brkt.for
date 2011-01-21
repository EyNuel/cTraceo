      program t_brkt

c
c     This code contains some testing cases for bracket.c
c     gfortran -o t_brkt.exe t_brkt.for brcket.for

c     shall contain a sorted vector of numbers:
      real*8	x(8)
      
c     the element who's "bracketing" elements are to be found:
      real*8 	xi
      
      integer*8   i, j, n

c     ====================================================
c     ==== a sequence with an even number of elements ====
      n = 6
      x(1) = 0.1365
      x(2) = 0.1730
      x(3) = 0.2844
      x(4) = 0.5155
      x(5) = 0.6946
      x(6) = 0.8385
      xi = 0.51
c     (returned index should be "4")
      
      write(6,*) '\nx:'
      do i = 1,n
        write(6,*) x(i),' '
      end do
      write(6,*) '\nxi: ',xi,'index: '
      call brcket(n, x, xi, j);
      write(6,*) j,'\n'
      


c     ====================================================
c     ==== a sequence with an odd number of elements ====
      n = 7
      x(1) = 0.1365
      x(2) = 0.1730
      x(3) = 0.2844
      x(4) = 0.5155
      x(5) = 0.6946
      x(6) = 0.8385
      x(7) = 0.9385;
      xi = 0.51
c     (returned index should be "4")
      call brcket(n,x,xi,i)
      
      write(6,*) '\nx:'
      do i = 1,n
        write(6,*) x(i),' '
      end do
      
      write(6,*) '\nxi: ',xi,'index: '
      call brcket(n, x, xi, j);
      write(6,*) j,'\n'

      end
