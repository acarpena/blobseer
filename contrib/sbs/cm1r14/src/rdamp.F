

      subroutine rdamps(taus,th0,s,sten)
      implicit none

      include 'input.incl' 
      include 'constants.incl' 
      include 'timestat.incl'

      real, dimension(ib:ie,jb:je,kb:ke) :: taus,th0
      real, dimension(ib:ie,jb:je,kb:ke) :: s,sten

      integer i,j,k

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        sten(i,j,k)=sten(i,j,k)-rdalpha*taus(i,j,k)*s(i,j,k)
      enddo
      enddo
      enddo

      if(timestats.ge.1) time_rdamp=time_rdamp+mytime()

      return
      end


!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      subroutine rdampu(tauh,u0,ua,uten)
      implicit none
 
      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'
 
      real, dimension(ib:ie,jb:je,kb:ke) :: tauh
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,ua,uten
 
      integer i,j,k

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni+1
        uten(i,j,k)=uten(i,j,k)-rdalpha*0.5*(tauh(i-1,j,k)+tauh(i,j,k))*(ua(i,j,k)-u0(i,j,k))
      enddo
      enddo
      enddo
 
      if(timestats.ge.1) time_rdamp=time_rdamp+mytime()
 
      return
      end


!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      subroutine rdampv(tauh,v0,va,vten)
      implicit none
 
      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'
 
      real, dimension(ib:ie,jb:je,kb:ke) :: tauh
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,va,vten
 
      integer i,j,k

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj+1
      do i=1,ni
        vten(i,j,k)=vten(i,j,k)-rdalpha*0.5*(tauh(i,j-1,k)+tauh(i,j,k))*(va(i,j,k)-v0(i,j,k))
      enddo
      enddo
      enddo
 
      if(timestats.ge.1) time_rdamp=time_rdamp+mytime()
 
      return
      end


!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      subroutine rdampw(tauf,wa,wten)
      implicit none
 
      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'
 
      real, dimension(ib:ie,jb:je,kb:ke+1) :: tauf
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa,wten
 
      integer i,j,k

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        wten(i,j,k)=wten(i,j,k)-rdalpha*tauf(i,j,k)*wa(i,j,k)
      enddo
      enddo
      enddo

      if(timestats.ge.1) time_rdamp=time_rdamp+mytime()

      return
      end


