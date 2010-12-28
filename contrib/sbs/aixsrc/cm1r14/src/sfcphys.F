!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine getcecd(cdu,cdv,ce,u0,v0,rf0,uu,vv,wspd,u,v)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      real, dimension(ib:ie,jb:je) :: cdu,cdv,ce
      real, dimension(ib:ie,jb:je,kb:ke) :: rf0
      real, dimension(ib:ie,jb:je,kb:ke) :: uu,vv,wspd
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,u
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,v

      integer i,j
      real usfc,vsfc,wsp

      real, parameter :: dcd1  =  0.5e-3
      real, parameter :: dcd2  =  2.4e-3
      real, parameter :: dwsp1 =  5.0
      real, parameter :: dwsp2 = 35.0

!-----------------------------------------------------------------------
! for pertflx=1, the base-state is not included in calculation of wind speed

      IF(pertflx.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j)
        do j=0,nj+1
        do i=0,ni+1
          uu(i,j,1) = u(i,j,1)-u0(i,j,1)
          vv(i,j,1) = v(i,j,1)-v0(i,j,1)
        enddo
        enddo

      ELSE

!$omp parallel do default(shared)   &
!$omp private(i,j)
        do j=0,nj+1
        do i=0,ni+1
          uu(i,j,1) = u(i,j,1)
          vv(i,j,1) = v(i,j,1)
        enddo
        enddo

      ENDIF

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc)
      do j=1,nj
      do i=1,ni
        usfc=0.5*( uu(i,j,1)+uu(i+1,j,1) )
        vsfc=0.5*( vv(i,j,1)+vv(i,j+1,1) )
        wspd(i,j,1)=sqrt(usfc**2+vsfc**2)
      enddo
      enddo

!-----------------------------------------------------------------------
!  Get Cd on u pts:

  IF(idrag.eq.1)THEN

    IF(cecd.eq.1)THEN
      ! constant value:

!$omp parallel do default(shared)   &
!$omp private(i,j)
      do j=1,nj
      do i=1,ni+1
        cdu(i,j) = cnstcd
      enddo
      enddo

    ELSEIF(cecd.eq.2)THEN
      ! Deacon's formula:  see Rotunno and Emanuel (1987, JAS, p. 547)

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc,wsp)
      do j=1,nj
      do i=1,ni+1
        usfc=uu(i,j,1)
        vsfc=0.25*( ( vv(i  ,j,1)+vv(i  ,j+1,1) )   &
                   +( vv(i-1,j,1)+vv(i-1,j+1,1) ) )
        wsp=sqrt(usfc**2+vsfc**2)
        cdu(i,j) = 1.1e-3+(4.0e-5*wsp)
      enddo
      enddo

    ELSEIF(cecd.eq.3)THEN
      ! Based on Donelan (2004, GRL)

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc,wsp)
      do j=1,nj
      do i=1,ni+1
        usfc=uu(i,j,1)
        vsfc=0.25*( ( vv(i  ,j,1)+vv(i  ,j+1,1) )   &
                   +( vv(i-1,j,1)+vv(i-1,j+1,1) ) )
        wsp=sqrt(usfc**2+vsfc**2)
        cdu(i,j) = dcd1+( wsp - dwsp1 )*( dcd2 - dcd1 )/( dwsp2 - dwsp1 )
        cdu(i,j) = min(cdu(i,j),dcd2)
        cdu(i,j) = max(cdu(i,j),dcd1)
      enddo
      enddo

    ELSE

      write(outfile,*)
      write(outfile,*) '  Undefined value for cecd '
      write(outfile,*)
      call stopcm1

    ENDIF

!-----------------------------------------------------------------------
!  Get Cd on v pts:

    IF(cecd.eq.1)THEN
      ! constant value:

!$omp parallel do default(shared)   &
!$omp private(i,j)
      do j=1,nj+1
      do i=1,ni
        cdv(i,j) = cnstcd
      enddo
      enddo

    ELSEIF(cecd.eq.2)THEN
      ! Deacon's formula:  see Rotunno and Emanuel (1987, JAS, p. 547)

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc,wsp)
      do j=1,nj+1
      do i=1,ni
        usfc=0.25*( ( uu(i,j  ,1)+uu(i+1,j  ,1) )   &
                   +( uu(i,j-1,1)+uu(i+1,j-1,1) ) )
        vsfc=vv(i,j,1)
        wsp=sqrt(usfc**2+vsfc**2)
        cdv(i,j) = 1.1e-3+(4.0e-5*wsp)
      enddo
      enddo

    ELSEIF(cecd.eq.3)THEN
      ! Based on Donelan (2004, GRL)

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc,wsp)
      do j=1,nj+1
      do i=1,ni
        usfc=0.25*( ( uu(i,j  ,1)+uu(i+1,j  ,1) )   &
                   +( uu(i,j-1,1)+uu(i+1,j-1,1) ) )
        vsfc=vv(i,j,1)
        wsp=sqrt(usfc**2+vsfc**2)
        cdv(i,j) = dcd1+( wsp - dwsp1 )*( dcd2 - dcd1 )/( dwsp2 - dwsp1 )
        cdv(i,j) = min(cdv(i,j),dcd2)
        cdv(i,j) = max(cdv(i,j),dcd1)
      enddo
      enddo

    ELSE

      write(outfile,*)
      write(outfile,*) '  Undefined value for cecd '
      write(outfile,*)
      call stopcm1

    ENDIF

  ENDIF   ! endif for idrag=1 check

!-----------------------------------------------------------------------
!  Get Ce:

  IF(isfcflx.eq.1)THEN

    IF(cecd.eq.1)THEN
      ! constant value:

!$omp parallel do default(shared)   &
!$omp private(i,j)
      do j=1,nj
      do i=1,ni
        ce(i,j) = cnstce
      enddo
      enddo

    ELSEIF(cecd.eq.2)THEN
      ! Deacon's formula:  see Rotunno and Emanuel (1987, JAS, p. 547)

!$omp parallel do default(shared)   &
!$omp private(i,j)
      do j=1,nj
      do i=1,ni
        ce(i,j) = 1.1e-3+(4.0e-5*wspd(i,j,1))
      enddo
      enddo

    ELSEIF(cecd.eq.3)THEN
      ! Constant, based on Drennan et al. (2007, JAS, p. 1103)

!$omp parallel do default(shared)   &
!$omp private(i,j)
      do j=1,nj
      do i=1,ni
        ce(i,j) = 1.18e-3
      enddo
      enddo

    ELSE

      write(outfile,*)
      write(outfile,*) '  Undefined value for cecd '
      write(outfile,*)
      call stopcm1

    ENDIF

  ENDIF

!-----------------------------------------------------------------------

      if(timestats.ge.1) time_sfcphys=time_sfcphys+mytime()

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine sfcdrag(cdu,cdv,u0,v0,rf0,uu,vv,t13,t23,u,v)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      real, dimension(ib:ie,jb:je) :: cdu,cdv
      real, dimension(ib:ie,jb:je,kb:ke) :: rf0
      real, dimension(ib:ie,jb:je,kb:ke) :: uu,vv,t13,t23
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,u
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,v

      integer i,j
      real usfc,vsfc,wspd

!-----------------------------------------------------------------------
!  x component:

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc,wspd)
      do j=1,nj
      do i=1,ni+1
        usfc=uu(i,j,1)
        vsfc=0.25*( ( vv(i  ,j,1)+vv(i  ,j+1,1) )   &
                   +( vv(i-1,j,1)+vv(i-1,j+1,1) ) )
        wspd=sqrt(usfc**2+vsfc**2)
        t13(i,j,1) = cdu(i,j)*usfc*wspd
      enddo
      enddo

!-----------------------------------------------------------------------
!  y component:

!$omp parallel do default(shared)   &
!$omp private(i,j,usfc,vsfc,wspd)
      do j=1,nj+1
      do i=1,ni
        usfc=0.25*( ( uu(i,j  ,1)+uu(i+1,j  ,1) )   &
                   +( uu(i,j-1,1)+uu(i+1,j-1,1) ) )
        vsfc=vv(i,j,1)
        wspd=sqrt(usfc**2+vsfc**2)
        t23(i,j,1) = cdv(i,j)*vsfc*wspd
      enddo
      enddo

!-----------------------------------------------------------------------

      if(timestats.ge.1) time_sfcphys=time_sfcphys+mytime()

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine sfcflux(ruh,xf,rvh,ce,zh,pi0,thv0,th0,u0,v0,thflux,qvflux,   &
                         rho,uu,vv,wspd,u,v,ppi,tha,qva,qsfc)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      real, dimension(ib:ie) :: ruh
      real, dimension(ib:ie+1) :: xf
      real, dimension(jb:je) :: rvh
      real, dimension(ib:ie,jb:je) :: ce
      real, dimension(ib:ie,jb:je,kb:ke) :: zh,pi0,thv0,th0
      real, dimension(ib:ie,jb:je) :: thflux,qvflux
      real, dimension(ib:ie,jb:je,kb:ke) :: uu,vv,wspd
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,u
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,v
      real, dimension(ib:ie,jb:je,kb:ke) :: rho,ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem) :: qva
      real*8 :: qsfc

      integer i,j
      real pisfc,usfc,vsfc,qvsat

      real :: pisurf,rhosfc,tem
      real rslf
      real*8, dimension(nj) :: bud1

      pisurf = (psurf*rp00)**rovcp

!-----------------------------------------------------------------------
!  sensible heat flux:

!$omp parallel do default(shared)   &
!$omp private(i,j,pisfc)
      do j=1,nj
      do i=1,ni
        pisfc = pisurf + ppi(i,j,1)
        thflux(i,j)=ce(i,j)*wspd(i,j,1)*(tsurf/pisfc-th0(i,j,1)-tha(i,j,1))
      enddo
      enddo

!-----------------------------------------------------------------------
!  latent heat flux:

    IF(imoist.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,pisfc,qvsat)
      do j=1,nj
      do i=1,ni
        pisfc = pisurf + ppi(i,j,1)
        qvsat=rslf(p00*(pisfc**cpdrd),tsurf)
        qvflux(i,j)=ce(i,j)*wspd(i,j,1)*(qvsat-qva(i,j,1))
      enddo
      enddo

!-----------------------------------------------------------------------
!  some budget calculations (only calculated if imoist=1):

!$omp parallel do default(shared)  &
!$omp private(j)
      do j=1,nj
        bud1(j)=0.0d0
      enddo

!$omp parallel do default(shared)  &
!$omp private(i,j,rhosfc)
      do j=1,nj
      do i=1,ni
        rhosfc=rho(i,j,1)-zh(i,j,1)*(rho(i,j,2)-rho(i,j,1))   &
                                   /( zh(i,j,2)- zh(i,j,1))
        if(axisymm.eq.1) rhosfc=rhosfc*pi*(xf(i+1)**2-xf(i)**2)/(dx*dy)
        bud1(j)=bud1(j)+qvflux(i,j)*ruh(i)*rvh(j)*rhosfc
      enddo
      enddo

      tem = dtl*dx*dy

      do j=1,nj
        qsfc=qsfc+bud1(j)*tem
      enddo

    ENDIF

!-----------------------------------------------------------------------

      if(timestats.ge.1) time_sfcphys=time_sfcphys+mytime()

      return
      end


