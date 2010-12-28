

      subroutine gettenq(iflux,xh,rxh,uh,xf,uf,vh,vf,mh,mf,q0,rho0,rr0,rf0,qvflux, &
                         dum1,dum2,dum3,dum4,divx,q,qten,khh,khv,gx,gy)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      real, dimension(ib:ie) :: xh,rxh,uh
      real, dimension(ib:ie+1) :: xf,uf
      real, dimension(jb:je) :: vh
      real, dimension(jb:je+1) :: vf
      integer iflux
      real, dimension(ib:ie,jb:je,kb:ke) :: mh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: mf
      real, dimension(ib:ie,jb:je,kb:ke) :: q0,rho0,rr0,rf0
      real, dimension(ib:ie,jb:je) :: qvflux
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,dum3,dum4,divx,q,qten
      real, dimension(ibc:iec,jbc:jec,kbc:kec) :: khh,khv
      real, dimension(itb:ite+1,jtb:jte,ktb:kte) :: gx
      real, dimension(itb:ite,jtb:jte+1,ktb:kte) :: gy

      integer i,j,k

!------------------------------------

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        qten(i,j,k)=0.0
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(idiff.eq.1)then
        if(difforder.eq.2)then
          call diff2s(1,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,q,qten)
        elseif(difforder.eq.6)then
          call diff6s(q0,dum1,dum2,dum3,q,qten)
        endif
      endif

      ! Call to turbs should be last
      if(iturb.ge.1)then
        call turbs(iflux,xh,rxh,uh,xf,uf,vh,vf,mh,mf,rho0,rr0,rf0,qvflux,   &
                   dum1,dum2,dum3,dum4,divx,q,qten,khh,khv,gx,gy)
      endif

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine integqrk(icom,bsq,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,    &
                          rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,mass,  &
                          rru,rrv,rrw,qten,qa,q3d,                          &
                          qten1,dttmp,nrk                                   &
#ifdef MPI
                          ,qw1,qw2,qe1,qe2,qs1,qs2,qn1,qn2,reqs_q           &
#endif
                          )
      implicit none
 
      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'
 
      integer icom
      real*8 bsq
      real, dimension(ib:ie) :: xh,rxh,uh,ruh
      real, dimension(ib:ie+1) :: xf
      real, dimension(jb:je) :: vh,rvh
      real, dimension(itb:ite,jtb:jte) :: gz
      real, dimension(ib:ie,jb:je,kb:ke) :: mh,rmh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: mf
      real, dimension(ib:ie,jb:je,kb:ke) :: rho0,rr0,rf0,rrf0
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,dum3,dum4,divx,mass
      real, dimension(ib:ie+1,jb:je,kb:ke) :: rru
      real, dimension(ib:ie,jb:je+1,kb:ke) :: rrv
      real, dimension(ib:ie,jb:je,kb:ke+1) :: rrw
      real, dimension(ib:ie,jb:je,kb:ke) :: qten,qa,q3d,qten1
      real :: dttmp
      integer :: nrk
#ifdef MPI
      real, dimension(3,nj,nk) :: qw1,qw2,qe1,qe2
      real, dimension(ni,3,nk) :: qs1,qs2,qn1,qn2
      integer, dimension(8) :: reqs_q
#endif
 
      integer :: i,j,k,bflag,pdef

!--------------------------------------------------------------

      bflag=0
      if(stat_qsrc.eq.1 .and. nrk.eq.3) bflag=1

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        qten(i,j,k)=qten1(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()


      if(nrk.eq.3)then
        pdef = 1
      else
        pdef = 0
      endif


    IF( (advweno.eq.1) .or. (advweno.eq.2.and.nrk.eq.3) )THEN
        call wenos(bflag,bsq,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,       &
                   rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,   &
                   divx,rru,rrv,rrw,q3d,qten)
    ELSE
      if(hadvorder.eq.5.or.advweno.ge.1)then
        call adv5s(bflag,bsq,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,       &
                   rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,mass,   &
                   rru,rrv,rrw,qa,q3d,qten,pdef,dttmp)
      elseif(hadvorder.eq.6)then
        call adv6s(bflag,bsq,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,       &
                   rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,mass,   &
                   rru,rrv,rrw,qa,q3d,qten,pdef,dttmp)
      endif
    ENDIF


!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        q3d(i,j,k)=qa(i,j,k)+dttmp*qten(i,j,k)
      enddo
      enddo
      enddo

!-------------------------------------------------------------------
!  positivity of moisture has been moved to SOLVE, via calling
!  subroutine PDEFQ.   10/03/03
!
!      if(nrk.eq.3)then
!!$omp parallel do default(shared)   &
!!$omp private(i,j,k)
!        do k=1,nk
!        do j=1,nj
!        do i=1,ni
!          if(q3d(i,j,k).lt.rmax) q3d(i,j,k)=0.
!        enddo
!        enddo
!        enddo
!      endif
!      if(timestats.ge.1) time_integ=time_integ+mytime()
!-------------------------------------------------------------------


      if( (nrk.lt.3.and.icom.eq.1).or.(icom.eq.2) )then
        call bcs(q3d)
#ifdef MPI
        call comm_3s_start(q3d,qw1,qw2,qe1,qe2,    &
                               qs1,qs2,qn1,qn2,reqs_q)
#endif
      endif


!--------------------------------------------------------------

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
 
 
      subroutine integtkerk(xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,           &
                            rho0,rr0,rf0,rrf0,                              &
                            dum1,dum2,dum3,dum4,divx,mass,rru,rrv,rrw,      &
                            tketen,tkea,tke3d,tketen1,dttmp,nrk             &
#ifdef MPI
                  ,tkw1,tkw2,tke1,tke2,tks1,tks2,tkn1,tkn2,reqs_tk          &
#endif
                            )
      implicit none
 
      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      real, dimension(ib:ie) :: xh,rxh,uh,ruh
      real, dimension(ib:ie+1) :: xf
      real, dimension(jb:je) :: vh,rvh
      real, dimension(itb:ite,jtb:jte) :: gz
      real, dimension(ib:ie,jb:je,kb:ke) :: mh,rmh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: mf
      real, dimension(ib:ie,jb:je,kb:ke) :: rho0,rr0,rf0,rrf0
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,dum3,dum4,divx,mass
      real, dimension(ib:ie+1,jb:je,kb:ke) :: rru
      real, dimension(ib:ie,jb:je+1,kb:ke) :: rrv
      real, dimension(ib:ie,jb:je,kb:ke+1) :: rrw,tketen
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea,tke3d,tketen1
      real :: dttmp
      integer :: nrk
#ifdef MPI
      real, dimension(cmp,jmp,kmt) :: tkw1,tkw2,tke1,tke2
      real, dimension(imp,cmp,kmt) :: tks1,tks2,tkn1,tkn2
      integer, dimension(rmp) :: reqs_tk
#endif
 
      integer :: i,j,k
      real*8 bfoo
 
!--------------------------------------------------------------

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nkt
      do j=1,nj
      do i=1,ni
        tketen(i,j,k)=tketen1(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(hadvorder.eq.5)then
        call adv5w(xh,rxh,uh,xf,vh,gz,mf,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                   rru,rrv,rrw,tke3d,tketen)
      elseif(hadvorder.eq.6)then
        call adv6w(xh,rxh,uh,xf,vh,gz,mf,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                   rru,rrv,rrw,tke3d,tketen)
      endif
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nkt
      do j=1,nj
      do i=1,ni
        tke3d(i,j,k)=tkea(i,j,k)+dttmp*tketen(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(nrk.eq.3)then
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nkt
        do j=1,nj
        do i=1,ni
          if(tke3d(i,j,k).lt.1.0e-6) tke3d(i,j,k)=0.0
        enddo
        enddo
        enddo
      endif
      if(timestats.ge.1) time_integ=time_integ+mytime()


      call bct(tke3d)

#ifdef MPI
      call comm_3t_start(tke3d,tkw1,tkw2,tke1,tke2,   &
                               tks1,tks2,tkn1,tkn2,reqs_tk)
#endif


!--------------------------------------------------------------
 
      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine integthrk(icom,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,         &
                           th0,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,         &
                           divx,mass,ppterm,gx,rru,u3d,gy,rrv,v3d,rrw,w3d,    &
                           tha,th3d,thten1,thten,thterm,tk,q3d,dttmp,nrk      &
#ifdef MPI
         ,thw1,thw2,the1,the2,ths1,ths2,thn1,thn2,reqs_th                     &
#endif
                           )
      implicit none
 
      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      integer icom
      real, dimension(ib:ie) :: xh,rxh,uh,ruh
      real, dimension(ib:ie+1) :: xf
      real, dimension(jb:je) :: vh,rvh
      real, dimension(itb:ite,jtb:jte) :: gz
      real, dimension(itb:ite+1,jtb:jte,ktb:kte) :: gx
      real, dimension(itb:ite,jtb:jte+1,ktb:kte) :: gy
      real, dimension(ib:ie,jb:je,kb:ke) :: mh,rmh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: mf
      real, dimension(ib:ie,jb:je,kb:ke) :: th0,rho0,rr0,rf0,rrf0
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,dum3,dum4,divx,mass,ppterm
      real, dimension(ib:ie+1,jb:je,kb:ke) :: rru,u3d
      real, dimension(ib:ie,jb:je+1,kb:ke) :: rrv,v3d
      real, dimension(ib:ie,jb:je,kb:ke+1) :: rrw,w3d
      real, dimension(ib:ie,jb:je,kb:ke) :: tha,th3d,thten1,thten,thterm,tk
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: q3d
      real :: dttmp
      integer :: nrk
#ifdef MPI
      real, dimension(3,nj,nk) :: thw1,thw2,the1,the2
      real, dimension(ni,3,nk) :: ths1,ths2,thn1,thn2
      integer, dimension(8) :: reqs_th
#endif

      integer :: i,j,k,n
      real*8 bfoo

!-------------------------------------------


!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        thten(i,j,k)=thten1(i,j,k)
      enddo
      enddo
      enddo

    IF(thsmall.eq.0)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie
        ppterm(i,j,k)=th0(i,j,k)+th3d(i,j,k)
      enddo
      enddo
      enddo

    ELSE

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie
        if(abs(th3d(i,j,k)).lt.smeps)then
          ppterm(i,j,k)=0.0
        else
          ppterm(i,j,k)=th3d(i,j,k)
        endif
      enddo
      enddo
      enddo

    ENDIF
      if(timestats.ge.1) time_misc=time_misc+mytime()
 

    IF( (advweno.eq.1) .or. (advweno.eq.2.and.nrk.eq.3) )THEN
        call wenos(0,bfoo,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,          &
                   rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,   &
                   divx,rru,rrv,rrw,ppterm,thten)
    ELSE
      if(hadvorder.eq.5.or.advweno.ge.1)then
        call adv5s(0,bfoo,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,          &
                   rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,mass,   &
                   rru,rrv,rrw,tha,ppterm,thten,0,dttmp)
      elseif(hadvorder.eq.6)then
        call adv6s(0,bfoo,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,          &
                   rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,mass,   &
                   rru,rrv,rrw,tha,ppterm,thten,0,dttmp)
      endif
    ENDIF


!--------------
!  additional terms for theta equation
!       dum2 is cpml
!       dum3 is cvml
!

      IF(neweqts.ge.1 .and. imoist.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum4(i,j,k)=0.0
        enddo
        enddo
        enddo
 
        DO n=nql1,nql2
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum4(i,j,k)=dum4(i,j,k)+cpl*q3d(i,j,k,n)
          enddo
          enddo
          enddo
        ENDDO

        if(iice.eq.1)then
          DO n=nqs1,nqs2
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
            do k=1,nk
            do j=1,nj
            do i=1,ni
              dum4(i,j,k)=dum4(i,j,k)+cpi*q3d(i,j,k,n)
            enddo
            enddo
            enddo
          ENDDO
        endif
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum2(i,j,k)=cp+cpv*q3d(i,j,k,nqv)+dum4(i,j,k)
        enddo
        enddo
        enddo
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum3(i,j,k)=cv+cvv*q3d(i,j,k,nqv)+dum4(i,j,k)
        enddo
        enddo
        enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          thterm(i,j,k)=( rd+rv*q3d(i,j,k,nqv)-rovcp*dum2(i,j,k) )/dum3(i,j,k)
        enddo
        enddo
        enddo

      IF(thsmall.eq.0)THEN

      IF(axisymm.eq.0)THEN
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          thten(i,j,k)=thten(i,j,k)-ppterm(i,j,k)*(              &
                      (u3d(i+1,j,k)-u3d(i,j,k))*rdx*uh(i)      &
                     +(v3d(i,j+1,k)-v3d(i,j,k))*rdy*vh(j)      &
                     +(w3d(i,j,k+1)-w3d(i,j,k))*rdz*mh(i,j,k) )    &
                    *thterm(i,j,k)
        enddo
        enddo
        enddo

      ELSE

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          thten(i,j,k)=thten(i,j,k)-ppterm(i,j,k)*(              &
                      (xf(i+1)*u3d(i+1,j,k)-xf(i)*u3d(i,j,k))*rdx*uh(i)*rxh(i)      &
                     +(w3d(i,j,k+1)-w3d(i,j,k))*rdz*mh(i,j,k) )    &
                    *thterm(i,j,k)
        enddo
        enddo
        enddo

      ENDIF

        IF(terrain_flag)THEN

          ! dum1 = dudz
          ! dum4 = dvdz

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk-1
          do j=1,nj
          do i=1,ni+1
            dum1(i,j,k)=gx(i,j,k)*(u3d(i,j,k+1)-u3d(i,j,k-1))*rdz2
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j)
          do j=1,nj
          do i=1,ni+1
            dum1(i,j,1 )=gx(i,j,1 )*(u3d(i,j,2 )-u3d(i,j,1   ))*rdz
            dum1(i,j,nk)=gx(i,j,nk)*(u3d(i,j,nk)-u3d(i,j,nk-1))*rdz
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk-1
          do j=1,nj+1
          do i=1,ni
            dum4(i,j,k)=gy(i,j,k)*(v3d(i,j,k+1)-v3d(i,j,k-1))*rdz2
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j)
          do j=1,nj+1
          do i=1,ni
            dum4(i,j,1 )=gy(i,j,1 )*(v3d(i,j,2 )-v3d(i,j,1   ))*rdz
            dum4(i,j,nk)=gy(i,j,nk)*(v3d(i,j,nk)-v3d(i,j,nk-1))*rdz
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            thten(i,j,k)=thten(i,j,k)-ppterm(i,j,k)*0.5*(      &
                           (dum1(i,j,k)+dum1(i+1,j,k))       &
                          +(dum4(i,j,k)+dum4(i,j+1,k))  )    &
                      *thterm(i,j,k)
          enddo
          enddo
          enddo

        ENDIF   ! for terrain

      ENDIF   ! for thsmall=0

      ENDIF   ! for neweqts=1
        if(timestats.ge.1) time_misc=time_misc+mytime()
 
 
!--------------

    IF(thsmall.eq.0)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        th3d(i,j,k)=tha(i,j,k)+dttmp*thten(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_integ=time_integ+mytime()

 
      if( (nrk.lt.3.or.imoist.eq.0) .and. icom.eq.1 )then
        call bcs(th3d)
#ifdef MPI
        call comm_3s_start(th3d,thw1,thw2,the1,the2,   &
                                ths1,ths2,thn1,thn2,reqs_th)
#endif
      endif

    ENDIF

!--------------
 
    IF(psolver.le.3)THEN

      if(neweqts.ge.1 .and. imoist.eq.1)then
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ppterm(i,j,k)=rovcp*dum2(i,j,k)/dum3(i,j,k)
        enddo
        enddo
        enddo
      else
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ppterm(i,j,k)=rddcv
        enddo
        enddo
        enddo
      endif

      if(timestats.ge.1) time_misc=time_misc+mytime()

    ENDIF

!--------------------------------------------------------------

      return
      end


