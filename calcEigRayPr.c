       subroutine calepr(ctitle,nthtas)
	  
c***********************************************************************
c      CALEPR.FOR
c      CALculation of Eigenrays by PRoximity
c      Faro, 07/12/2010 at 12:30
c      Written by Orlando Camargo Rodriguez
c      Version interfaced with Matlab Engine
c****&******************************************************************

c      Include global (common) variables:

       include 'global.for'

c      Define local variables:
       
       character*60 ctitle
       character*10 eray
       character*9 cix
       character*8 cviii
       character*7 cvii
       character*6 cvi
       character*5 cv
       character*4 civ
       character*3 ciii
       character*2 cii
       character*1 ci
             
       integer*8 irefl(np),jbdry(np)
       integer*8 iret(51)
       integer*8 i,ifail,ih,imax,j,jj,jjj,k,l,m
       integer*8 nthtas,nret,erays

       integer*8 matOpen, matClose
       integer*8 mxCreateDoubleMatrix, mxCreateString, mxGetPr
       integer*8 mp, ptitle, pthtas, prh, pzh, pray, perays
       integer*8 matPutVariable, matDeleteVariable
       integer*8 status
       
       real*8 raydat(5,np)
       real*8  tbdry(2,np)

       real*8 xl(2),yl(2)

       real*8 thetai,ctheta
       real*8 rh,zh,zray,dz,tauray
       real*8 rerays

       complex*8 decay(np)
       complex*8 zl(2)
       complex*8 aray

c***********************************************************************

       eray = 'ray0000000'

c***********************************************************************
c      Too much launching angles? do not proceed with calculations:

       if (nthtas.gt.nangle) then
       write(6,*) 'EIG: # of launching angles > ',nangle
       write(6,*) 'aborting calculations...'
       stop 
       end if
       
c***********************************************************************
c      Too much hydrophones along range? do not proceed with calculations:

       if (nra.gt.nhyd2) then
       write(6,*) 'EIG: # of  receivers along range > ',nhyd2
       write(6,*) 'aborting calculations...'
       stop 
       end if
       
c***********************************************************************

       erays = 0 

       mp = matOpen('eig.mat', 'w')

       pthtas   = mxCreateDoubleMatrix(1,nthtas,0)
       call     mxCopyReal8ToPtr(thetas, mxGetPr(pthtas), nthtas)
       status   = matPutVariable(mp,'thetas',pthtas)
       call     mxDestroyArray(pthtas)

       ptitle   = mxCreateString('TRACEO: EIGenrays (by proximity)')
       status   = matPutVariable(mp,'caseTitle',ptitle)
       call     mxDestroyArray(ptitle)
       
       prh      = mxCreateDoubleMatrix(1,nra,0)
       call     mxCopyReal8ToPtr(rarray, mxGetPr(prh), nra)
       status	= matPutVariable(mp,'rarray',prh)
       call		mxDestroyArray(prh)
       
       pzh		= mxCreateDoubleMatrix(1,nza,0)
       call		mxCopyReal8ToPtr(zarray, mxGetPr(pzh), nza)
       status	= matPutVariable(mp,'zarray',pzh)
       call 	mxDestroyArray(pzh)

c***********************************************************************
c      Solve the EIKonal and the Dynamic sets of EQuations:
c***********************************************************************

		do i = 1,nthtas
			thetai = -thetas(i)*pi/180.0
			ctheta = abs( cos( thetai ) ) 

c          	Trace a ray as long as it is neither 90 or -90:
			if (ctheta.gt.1.0e-7) then
				call seikeq(thetai,imax,irefl,decay,jbdry,tbdry)
				call sdyneq(thetai,imax,irefl,decay,jbdry,tbdry)

c          		Got the ray? test for proximity to each hydrophone 
c         		(yes, this is slow, can you figure out a better way  
c          		to do it?):
				do j = 1,nra
					rh = rarray(j)
					if ((rh.ge.rmin).and.(rh.lt.rmax)) then
c***********************************************************************
c                 	Check if the ray is returning back or not; if not 
c                 	we can bracket it without problems, otherwise we 
c                 	need to know how many times it passed by the given
c                 	array range: 
					if (iretrn.eq.0) then
						call brcket(imax,r,rh,ih)

						xl(1) =   r( ih )
						xl(2) =   r(ih+1)
						yl(1) =   z( ih )
						yl(2) =   z(ih+1)

						call lini1d(xl,yl,rh,zray,dummy)
						yl(1) = tau( ih )
						yl(2) = tau(ih+1)

						call lini1d(xl,yl,rh,tauray,dummy)
						zl(1) = amp( ih )
						zl(2) = amp(ih+1)

						call clin1d(xl,zl,rh,aray,dummy)
						do jj = 1,nza
							zh = zarray(jj)
							dz = abs(zray-zh)
							if (dz.lt.miss) then
								erays = erays + 1
c#######################################################################
								if (erays.lt.10) then
									jjj = 4
									write (ci, '(I1)'), erays
									eray(4:4) = ci
								elseif ((erays.ge.10).and.(erays.lt.100)) then
									jjj = 5
									write (cii, '(I2)'), erays
									eray(4:5) = cii
								elseif ((erays.ge.100).and.(erays.lt.1000))  then
									jjj = 6
									write (ciii, '(I3)'), erays
									eray(4:6) = ciii
								elseif ((erays.ge.1000).and.(erays.lt.10000))  then
									jjj = 7
									write (civ, '(I4)'), erays
									eray(4:7) = civ
								elseif ((erays.ge.1000).and.(erays.lt.10000))  then
									jjj = 8
									write (cv, '(I5)'), erays
									eray(4:8) = cv
								else
									jjj = 9
									write (cvi, '(I6)'), erays
									eray(4:9) = cvi
								end if										
c#######################################################################
								r(ih+1) =     rh
								z(ih+1) =   zray
								tau(ih+1) = tauray
								amp(ih+1) =   aray
								
								do k = 1,ih+1
									raydat(1,k) =   r(k)
									raydat(2,k) =   z(k)
									raydat(3,k) = tau(k)
									raydat(4,k) = realpart( amp(k) )
									raydat(5,k) = imagpart( amp(k) )
								end do

								pray	= mxCreateDoubleMatrix(5,ih+1,0)
								call 	mxCopyReal8ToPtr(raydat,mxGetPr(pray),5*(ih+1))
								status 	= matPutVariable(mp,eray(1:jjj),pray)
								call 	mxDestroyArray(pray)
							end if 
						end do 
					else
						call ebrckt(imax,r,rh,nret,iret)
						do l = 1,nret
							xl(1) =   r(iret(l)  )
							xl(2) =   r(iret(l)+1)
							yl(1) =   z(iret(l)  )
							yl(2) =   z(iret(l)+1)
							
							call lini1d(xl,yl,rh,zray,dummy)
							yl(1) = tau(iret(l)  )
							yl(2) = tau(iret(l)+1)
							
							call lini1d(xl,yl,rh,tauray,dummy)
							zl(1) = amp(iret(l)  )
							zl(2) = amp(iret(l)+1)
							
							call clin1d(xl,zl,rh,aray,dummy)
							do jj = 1,nza
								zh = zarray(jj)
								dz = abs(zray-zh)
								if (dz.lt.miss) then
									erays = erays + 1
c#######################################################################
								if (erays.lt.10) then
									jjj = 4
									write (ci, '(I1)'), erays
									eray(4:4) = ci
								elseif ((erays.ge.10).and.(erays.lt.100)) then
									jjj = 5
									write (cii, '(I2)'), erays
									eray(4:5) = cii
								elseif ((erays.ge.100).and.(erays.lt.1000))  then
									jjj = 6
									write (ciii, '(I3)'), erays
									eray(4:6) = ciii
								elseif ((erays.ge.1000).and.(erays.lt.10000))  then
									jjj = 7
									write (civ, '(I4)'), erays
									eray(4:7) = civ
								elseif ((erays.ge.1000).and.(erays.lt.10000))  then
									jjj = 8
									write (cv, '(I5)'), erays
									eray(4:8) = cv
								else
									jjj = 9
									write (cvi, '(I6)'), erays
									eray(4:9) = cvi
								end if
c#######################################################################
								r(ih+1) =	  rh
								z(ih+1) =	zray
								tau(ih+1) = tauray
								amp(ih+1) =	aray
								
								do k = 1,ih+1
									raydat(1,k) =   r(k)
									raydat(2,k) =   z(k)
									raydat(3,k) = tau(k)
									raydat(4,k) = realpart( amp(k) )
									raydat(5,k) = imagpart( amp(k) )
								end do
								
								pray = mxCreateDoubleMatrix(5,ih+1,0)
								call mxCopyReal8ToPtr(raydat,mxGetPr(pray),5*(ih+1))
								status = matPutVariable(mp,eray(1:jjj),pray)
								call mxDestroyArray(pray)
							end if
						end do
					end do
				end if
c***********************************************************************
			end if 
		end do 
	end if
end do

c***********************************************************************
c      The Happy End:
c***********************************************************************

rerays = 1.0*erays 
perays = mxCreateDoubleMatrix(1,1,0)

call mxCopyReal8ToPtr(rerays,mxGetPr(perays),1)
status = matPutVariable(mp,'nerays',perays)
call mxDestroyArray(perays)
status = matClose(mp)

c***********************************************************************
c     Back to main:
c***********************************************************************

return

end
								
