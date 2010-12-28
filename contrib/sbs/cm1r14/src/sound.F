

      subroutine sound(xh,rxh,uh,ruh,xf,uf,vh,rvh,vf,mh,rmh,mf,rmf,     &
                        pi0,thv0,rho0,rr0,rf0,th0,dzdx,dzdy,            &
                        radbcw,radbce,radbcs,radbcn,                    &
                        fwk,ppd,dpdzx,dpdzy,pk,mm,qk,fpk,               &
                        gx,ua,u3d,uten,gy,va,v3d,vten,wa,w3d,wten,      &
                        ppi,pp3d,ppten,tha,th3d,thten,thterm,tk,        &
                        thv,ppterm,nrk,                                 &
                        reqs_u,reqs_v,reqs_w,reqs_s,reqs_p,             &
                        uw31,uw32,ue31,ue32,us31,us32,un31,un32,        &
                        vw31,vw32,ve31,ve32,vs31,vs32,vn31,vn32,        &
                        ww31,ww32,we31,we32,ws31,ws32,wn31,wn32,        &
                        sw31,sw32,se31,se32,ss31,ss32,sn31,sn32,        &
                        pw31,pw32,pe31,pe32,ps31,ps32,pn31,pn32,        &
                        pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'

      real, dimension(ib:ie) :: xh,rxh,uh,ruh
      real, dimension(ib:ie+1) :: xf,uf
      real, dimension(jb:je) :: vh,rvh
      real, dimension(jb:je+1) :: vf
      real, dimension(ib:ie,jb:je,kb:ke) :: mh,rmh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: mf,rmf
      real, dimension(ib:ie,jb:je,kb:ke) :: pi0,thv0,rho0,rr0,rf0,th0
      real, dimension(itb:ite,jtb:jte) :: dzdx,dzdy
      real, dimension(itb:ite+1,jtb:jte,ktb:kte) :: gx
      real, dimension(itb:ite,jtb:jte+1,ktb:kte) :: gy
      real, dimension(jb:je,kb:ke) :: radbcw,radbce
      real, dimension(ib:ie,kb:ke) :: radbcs,radbcn
      real, dimension(ib:ie,jb:je,kb:ke) :: fwk,ppd,dpdzx,dpdzy,pk,mm,qk,fpk
      real, dimension(ib:ie+1,jb:je,kb:ke) :: ua,u3d,uten
      real, dimension(ib:ie,jb:je+1,kb:ke) :: va,v3d,vten
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa,w3d,wten
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,pp3d,ppten
      real, dimension(ib:ie,jb:je,kb:ke) :: tha,th3d,thten,thterm,tk
      real, dimension(ib:ie,jb:je,kb:ke) :: thv,ppterm
      integer nrk
      integer, dimension(rmp) :: reqs_u,reqs_v,reqs_w,reqs_s,reqs_p
      real, dimension(cmp,jmp,kmp)   :: uw31,uw32,ue31,ue32
      real, dimension(imp+1,cmp,kmp) :: us31,us32,un31,un32
      real, dimension(cmp,jmp+1,kmp) :: vw31,vw32,ve31,ve32
      real, dimension(imp,cmp,kmp)   :: vs31,vs32,vn31,vn32
      real, dimension(cmp,jmp,kmp-1) :: ww31,ww32,we31,we32
      real, dimension(imp,cmp,kmp-1) :: ws31,ws32,wn31,wn32
      real, dimension(cmp,jmp,kmp)   :: sw31,sw32,se31,se32
      real, dimension(imp,cmp,kmp)   :: ss31,ss32,sn31,sn32
      real, dimension(cmp,jmp,kmp)   :: pw31,pw32,pe31,pe32
      real, dimension(imp,cmp,kmp)   :: ps31,ps32,pn31,pn32
      real, dimension(jmp,kmp) :: pw1,pw2,pe1,pe2
      real, dimension(imp,kmp) :: ps1,ps2,pn1,pn2

!-----

      integer :: i,j,k,n,nloop
      real :: tem,r1,dts
      real :: aa,bb,cc,dd

!!!      real alph,beta,asq
!!!      parameter(alph=0.60)
!!!      parameter(beta=1.0-alph)
!!!      parameter(asq=alph*alph)

      real :: beta,asq
      real, dimension(ni,nj,nk) :: ftk,bk,sk

#ifdef MPI
      include 'mpif.h'
#endif

!---------------------------------------------------------------------

      beta=1.0-alph
      asq=alph*alph

!---------------------------------------------------------------------
!  Prepare for acoustic steps

      if(nrk.ge.2)then

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=kb,ke
        do j=jb,je
        do i=ib,ie+1
          u3d(i,j,k)=ua(i,j,k)
        enddo
        enddo
        enddo
 
      IF(axisymm.eq.0)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=kb,ke
        do j=jb,je+1
        do i=ib,ie
          v3d(i,j,k)=va(i,j,k)
        enddo
        enddo
        enddo
      ENDIF
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=kb,ke+1
        do j=jb,je
        do i=ib,ie
          w3d(i,j,k)=wa(i,j,k)
        enddo
        enddo
        enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=kb,ke
        do j=jb,je
        do i=ib,ie
          pp3d(i,j,k)=ppi(i,j,k)
        enddo
        enddo
        enddo

        if(timestats.ge.1) time_misc=time_misc+mytime()

      endif

        IF(thsmall.eq.1)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=kb,ke
          do j=jb,je
          do i=ib,ie
            th3d(i,j,k)=tha(i,j,k)
          enddo
          enddo
          enddo
        ENDIF

        IF(neweqts.eq.0)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            sk(i,j,k)=0.0
          enddo
          enddo
          enddo
        ENDIF

!---------------------------------------------------------------------
 
      if(nrk.eq.1)then
        nloop=1
        dts=dtl/3.
      elseif(nrk.eq.2)then
        nloop=0.5*nsound
        dts=dtl/nsound
      elseif(nrk.eq.3)then
        nloop=nsound
        dts=dtl/nsound
      endif

!-----------------------------------------------------------------
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni+1
        uten(i,j,k)=uten(i,j,k)*dts
      enddo
      enddo
      enddo

    IF(axisymm.eq.0)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj+1
      do i=1,ni
        vten(i,j,k)=vten(i,j,k)*dts
      enddo
      enddo
      enddo
    ENDIF

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=2,nk
      do j=1,nj
      do i=1,ni
        wten(i,j,k)=wten(i,j,k)*dts
      enddo
      enddo
      enddo
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        ppten(i,j,k)=ppten(i,j,k)*dts
      enddo
      enddo
      enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        ppterm(i,j,k)=ppterm(i,j,k)*dts
      enddo
      enddo
      enddo

      IF(thsmall.eq.1)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          thten(i,j,k)=thten(i,j,k)*dts
        enddo
        enddo
        enddo
        IF(neweqts.ge.1 .and. imoist.eq.1)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            thterm(i,j,k)=thterm(i,j,k)*dts
          enddo
          enddo
          enddo
        ENDIF
      ENDIF

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=0,nj+1
      do i=0,ni+1
        ppd(i,j,k)=pp3d(i,j,k)
      enddo
      enddo
      enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        pk(i,j,k)=dts*g*0.5/(cp*thv0(i,j,k))
      enddo
      enddo
      enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
      do k=2,nk
      do j=1,nj
      do i=1,ni
        mm(i,j,k)=dts*cp*0.5*rdz*mf(i,j,k)*( thv(i,j,k-1)+thv(i,j,k) )
      enddo
      enddo
      enddo

      IF(thsmall.eq.1)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=2,nk
        do j=1,nj
        do i=1,ni
          bk(i,j,k)=dts*g/(th0(i,j,k-1)+th0(i,j,k))
        enddo
        enddo
        enddo
      ENDIF

      if(timestats.ge.1) time_sound=time_sound+mytime()

!---------------------------------------------------------------------

      DO N=1,NLOOP

!-----

        if(irbc.eq.2)then
 
          if(ibw.eq.1 .or. ibe.eq.1) call radbcew(radbcw,radbce,u3d)
 
          if(ibs.eq.1 .or. ibn.eq.1) call radbcns(radbcs,radbcn,v3d)
 
        endif

!-----

        if(wbc.eq.2.and.ibw.eq.1)then
!$omp parallel do default(shared)   &
!$omp private(j,k)
          do k=1,nk
          do j=1,nj
            u3d(1,j,k)=u3d(1,j,k)-dts*radbcw(j,k)          &
                      *(u3d(2,j,k)-u3d(1,j,k))*rdx*uh(1)   &
                         +uten(1,j,k)
          enddo
          enddo
        endif

        if(ebc.eq.2.and.ibe.eq.1)then
!$omp parallel do default(shared)   &
!$omp private(j,k)
          do k=1,nk
          do j=1,nj
            u3d(ni+1,j,k)=u3d(ni+1,j,k)-dts*radbce(j,k)              &
                         *(u3d(ni+1,j,k)-u3d(ni  ,j,k))*rdx*uh(ni)   &
                         +uten(ni+1,j,k)
          enddo
          enddo
        endif

!-----

        IF(roflux.eq.1)THEN
          call restrict_openbc_we(rvh,rmh,rho0,u3d)
        ENDIF

!-----

        tem=dts*rdx*cp*0.5

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1+ibw,ni+1-ibe
          u3d(i,j,k)=u3d(i,j,k)+uten(i,j,k)                &
                  -(tem*(ppd(i,j,k)-ppd(i-1,j,k))*uf(i)*   &
                        (thv(i,j,k)+thv(i-1,j,k)))
        enddo
        enddo
        enddo

        IF(terrain_flag)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1+ibw,ni+1-ibe
            fwk(i,j,k)=0.5*(ppd(i-1,j,k)+ppd(i,j,k))
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk-1
          do j=1,nj
          do i=1+ibw,ni+1-ibe
            dpdzx(i,j,k)=( fwk(i,j,k+1)-fwk(i,j,k-1) )*rdz2
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j)
          do j=1,nj
          do i=1+ibw,ni+1-ibe
            dpdzx(i,j,1 )=( fwk(i,j,2 )-fwk(i,j,1   ) )*rdz
            dpdzx(i,j,nk)=( fwk(i,j,nk)-fwk(i,j,nk-1) )*rdz
          enddo
          enddo

          tem=dts*cp*0.5

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1+ibw,ni+1-ibe
            u3d(i,j,k)=u3d(i,j,k)                                  &
                  -tem*(thv(i,j,k)+thv(i-1,j,k))*gx(i,j,k)*dpdzx(i,j,k)
          enddo
          enddo
          enddo

        ENDIF

        if(timestats.ge.1) time_sound=time_sound+mytime()

        if(n.eq.nloop)then
          call bcu(u3d)
        endif

#ifdef MPI
        if(n.eq.nloop)then
          call comm_3u_start(u3d,uw31,uw32,ue31,ue32,   &
                                 us31,us32,un31,un32,reqs_u)
        endif
#endif

!-----

      IF(axisymm.eq.0)THEN

        if(sbc.eq.2.and.ibs.eq.1)then
!$omp parallel do default(shared)   &
!$omp private(i,k)
          do k=1,nk
          do i=1,ni
            v3d(i,1,k)=v3d(i,1,k)-dts*radbcs(i,k)          &
                      *(v3d(i,2,k)-v3d(i,1,k))*rdy*vh(1)   &
                      +vten(i,1,k)
          enddo
          enddo
        endif
 
        if(nbc.eq.2.and.ibn.eq.1)then
!$omp parallel do default(shared)   &
!$omp private(i,k)
          do k=1,nk
          do i=1,ni
            v3d(i,nj+1,k)=v3d(i,nj+1,k)-dts*radbcn(i,k)              &
                         *(v3d(i,nj+1,k)-v3d(i,nj  ,k))*rdy*vh(nj)   &
                         +vten(i,nj+1,k)
          enddo
          enddo
        endif

!-----

        IF(roflux.eq.1)THEN
          call restrict_openbc_sn(ruh,rmh,rho0,v3d)
        ENDIF

!-----

        tem=dts*rdy*cp*0.5

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1+ibs,nj+1-ibn
        do i=1,ni
          v3d(i,j,k)=v3d(i,j,k)+vten(i,j,k)                &
                  -(tem*(ppd(i,j,k)-ppd(i,j-1,k))*vf(j)*   &
                        (thv(i,j,k)+thv(i,j-1,k)))
        enddo
        enddo
        enddo

        IF(terrain_flag)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1+ibs,nj+1-ibn
          do i=1,ni
            fwk(i,j,k)=0.5*(ppd(i,j-1,k)+ppd(i,j,k))
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk-1
          do j=1+ibs,nj+1-ibn
          do i=1,ni
            dpdzy(i,j,k)=( fwk(i,j,k+1)-fwk(i,j,k-1) )*rdz2
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j)
          do j=1+ibs,nj+1-ibn
          do i=1,ni
            dpdzy(i,j,1 )=( fwk(i,j,2   )-fwk(i,j,1   ) )*rdz
            dpdzy(i,j,nk)=( fwk(i,j,nk  )-fwk(i,j,nk-1) )*rdz
          enddo
          enddo

          tem=dts*cp*0.5

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1+ibs,nj+1-ibn
          do i=1,ni
            v3d(i,j,k)=v3d(i,j,k)                                  &
                  -tem*(thv(i,j,k)+thv(i,j-1,k))*gy(i,j,k)*dpdzy(i,j,k)
          enddo
          enddo
          enddo

        ENDIF

        if(timestats.ge.1) time_sound=time_sound+mytime()

        if(n.eq.nloop)then
          call bcv(v3d)
        endif

#ifdef MPI
        if(n.eq.nloop)then
          call comm_3v_start(v3d,vw31,vw32,ve31,ve32,   &
                                 vs31,vs32,vn31,vn32,reqs_v)
        endif
#endif

      ENDIF

        if(n.lt.nloop)then
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ppd(i,j,k)=pp3d(i,j,k)
          enddo
          enddo
          enddo
        endif

!---------------------------------------------------
!  update implicit solver arrays

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          qk(i,j,k)=ppterm(i,j,k)*(pi0(i,j,k)+pp3d(i,j,k))*rdz*mh(i,j,k)
        enddo
        enddo
        enddo

      IF(axisymm.eq.0)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          fpk(i,j,k)=ppten(i,j,k)+beta*(                                 &
            w3d(i,j,k)*(pk(i,j,k)+qk(i,j,k))                                 &
           +w3d(i,j,k+1)*(pk(i,j,k)-qk(i,j,k)) )                             &
           -ppterm(i,j,k)*(pi0(i,j,k)+pp3d(i,j,k))                       &
                             *( (u3d(i+1,j,k)-u3d(i,j,k))*rdx*uh(i)  &
                               +(v3d(i,j+1,k)-v3d(i,j,k))*rdy*vh(j) )
        enddo
        enddo
        enddo

      ELSE

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          fpk(i,j,k)=ppten(i,j,k)+beta*(                                 &
            w3d(i,j,k)*(pk(i,j,k)+qk(i,j,k))                                 &
           +w3d(i,j,k+1)*(pk(i,j,k)-qk(i,j,k)) )                             &
           -ppterm(i,j,k)*(pi0(i,j,k)+pp3d(i,j,k))                       &
                             *( (xf(i+1)*u3d(i+1,j,k)-xf(i)*u3d(i,j,k))*rdx*uh(i)*rxh(i) )
        enddo
        enddo
        enddo

      ENDIF

        IF(thsmall.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ftk(i,j,k)=thten(i,j,k)+beta*dts*rr0(i,j,k)*(    &
            w3d(i,j,k)*tk(i,j,k)+w3d(i,j,k+1)*tk(i,j,k+1)  )
        enddo
        enddo
        enddo

        IF(neweqts.ge.1 .and. imoist.eq.1)THEN

        IF(axisymm.eq.0)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            sk(i,j,k)=thterm(i,j,k)*(th0(i,j,k)+th3d(i,j,k))*rdz*mh(i,j,k)
            ftk(i,j,k)=ftk(i,j,k)+beta*(    &
             +w3d(i,j,k)*sk(i,j,k)                                 &
             -w3d(i,j,k+1)*sk(i,j,k) )                            &
             -thterm(i,j,k)*(th0(i,j,k)+th3d(i,j,k))                       &
                               *( (u3d(i+1,j,k)-u3d(i,j,k))*rdx*uh(i)  &
                                 +(v3d(i,j+1,k)-v3d(i,j,k))*rdy*vh(j) )
          enddo
          enddo
          enddo
        ELSE
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            sk(i,j,k)=thterm(i,j,k)*(th0(i,j,k)+th3d(i,j,k))*rdz*mh(i,j,k)
            ftk(i,j,k)=ftk(i,j,k)+beta*(    &
             +w3d(i,j,k)*sk(i,j,k)                                 &
             -w3d(i,j,k+1)*sk(i,j,k) )                            &
             -thterm(i,j,k)*(th0(i,j,k)+th3d(i,j,k))                       &
                               *( (xf(i+1)*u3d(i+1,j,k)-xf(i)*u3d(i,j,k))*rdx*uh(i)*rxh(i) )
          enddo
          enddo
          enddo
        ENDIF   ! for axisymm

        ENDIF   ! for neweqts

        ENDIF   ! for thsmall

        IF(terrain_flag)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk-1
          do j=1,nj
          do i=1,ni+1
            dpdzx(i,j,k)=gx(i,j,k)*(u3d(i,j,k+1)-u3d(i,j,k-1))*rdz2
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j)
          do j=1,nj
          do i=1,ni+1
            dpdzx(i,j, 1)=gx(i,j,1 )*(u3d(i,j,2 )-u3d(i,j,1   ))*rdz
            dpdzx(i,j,nk)=gx(i,j,nk)*(u3d(i,j,nk)-u3d(i,j,nk-1))*rdz
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk-1
          do j=1,nj+1
          do i=1,ni
            dpdzy(i,j,k)=gy(i,j,k)*(v3d(i,j,k+1)-v3d(i,j,k-1))*rdz2
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j)
          do j=1,nj+1
          do i=1,ni
            dpdzy(i,j, 1)=gy(i,j,1 )*(v3d(i,j,2 )-v3d(i,j,1   ))*rdz
            dpdzy(i,j,nk)=gy(i,j,nk)*(v3d(i,j,nk)-v3d(i,j,nk-1))*rdz
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            fpk(i,j,k)=fpk(i,j,k)-ppterm(i,j,k)*(pi0(i,j,k)+pp3d(i,j,k))    &
                   *0.5*( (dpdzx(i,j,k)+dpdzx(i+1,j,k))             &
                         +(dpdzy(i,j,k)+dpdzy(i,j+1,k)) )
          enddo
          enddo
          enddo

          IF(thsmall.eq.1.and.neweqts.ge.1.and.imoist.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ftk(i,j,k)=ftk(i,j,k)-thterm(i,j,k)*(th0(i,j,k)+th3d(i,j,k))    &
                   *0.5*( (dpdzx(i,j,k)+dpdzx(i+1,j,k))             &
                         +(dpdzy(i,j,k)+dpdzy(i,j+1,k)) )
          enddo
          enddo
          enddo

          ENDIF

        ENDIF

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=2,nk
        do j=1,nj
        do i=1,ni
          fwk(i,j,k)=wten(i,j,k)                            &
             -beta*mm(i,j,k)*(pp3d(i,j,k)-pp3d(i,j,k-1))
        enddo
        enddo
        enddo

        IF(thsmall.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=2,nk
          do j=1,nj
          do i=1,ni
            fwk(i,j,k)=fwk(i,j,k)                             &
               +beta*bk(i,j,k)*(th3d(i,j,k)+th3d(i,j,k-1))
          enddo
          enddo
          enddo

        ENDIF

!--------------------------------------
!  Get boundary conditions for w

        if(terrain_flag)then
          if(timestats.ge.1) time_sound=time_sound+mytime()
          call bcwsfc(dzdx,dzdy,u3d,v3d,w3d)
        else
!$omp parallel do default(shared)  &
!$omp private(i,j)
          do j=1,nj
          do i=1,ni
            w3d(i,j,1)=0.0
          enddo
          enddo
        endif

!---------------------------------------------
!  the implicit solver ....
 
!$omp parallel do default(shared)   &
!$omp private(i,j)
        do j=1,nj
        do i=1,ni
          dpdzx(i,j,1)=0.0
          dpdzy(i,j,1)=w3d(i,j,1)
        enddo
        enddo

      IF(thsmall.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k,aa,bb,cc,dd,r1)
        do j=1,nj
        do k=2,nk
        do i=1,ni
          aa=asq*( mm(i,j,k)*(qk(i,j,k)-pk(i,j,k))   &
                  +bk(i,j,k)*(dts*tk(i,j,k+1)*rr0(i,j,k)-sk(i,j,k)) )
          bb=1.0+asq*( mm(i,j,k)*( qk(i,j,k)+qk(i,j,k-1)                    &
                                  +pk(i,j,k)-pk(i,j,k-1) )                  &
                      -bk(i,j,k)*(dts*tk(i,j,k)*(rr0(i,j,k)+rr0(i,j,k-1))   &
                                  +sk(i,j,k)-sk(i,j,k-1)) )
          cc=asq*( mm(i,j,k)*(pk(i,j,k-1)+qk(i,j,k-1))   &
                  +bk(i,j,k)*(dts*tk(i,j,k-1)*rr0(i,j,k-1)+sk(i,j,k-1)) )
          dd=w3d(i,j,k)+fwk(i,j,k)                                                    &
              -alph*( mm(i,j,k)*(pp3d(i,j,k)+fpk(i,j,k)-pp3d(i,j,k-1)-fpk(i,j,k-1))   &
                     -bk(i,j,k)*(th3d(i,j,k)+ftk(i,j,k)+th3d(i,j,k-1)+ftk(i,j,k-1)) )
          r1 = 1.0/(bb-cc*dpdzx(i,j,k-1))
          dpdzx(i,j,k)=aa*r1
          dpdzy(i,j,k)=(dd+cc*dpdzy(i,j,k-1))*r1
        enddo
        enddo
        enddo

      ELSE

!$omp parallel do default(shared)   &
!$omp private(i,j,k,aa,bb,cc,dd,r1)
        do j=1,nj
        do k=2,nk
        do i=1,ni
          aa=asq*mm(i,j,k)*(qk(i,j,k)-pk(i,j,k))
          bb=1.0+asq*mm(i,j,k)*( qk(i,j,k)+qk(i,j,k-1)    &
                            +pk(i,j,k)-pk(i,j,k-1) )
          cc=asq*mm(i,j,k)*(pk(i,j,k-1)+qk(i,j,k-1))
          dd=w3d(i,j,k)+fwk(i,j,k)    &
              -alph*mm(i,j,k)*(pp3d(i,j,k)+fpk(i,j,k)-pp3d(i,j,k-1)-fpk(i,j,k-1))
          r1 = 1.0/(bb-cc*dpdzx(i,j,k-1))
          dpdzx(i,j,k)=aa*r1
          dpdzy(i,j,k)=(dd+cc*dpdzy(i,j,k-1))*r1
        enddo
        enddo
        enddo

      ENDIF

!$omp parallel do default(shared)   &
!$omp private(i,j)
        do j=1,nj
        do i=1,ni
          w3d(i,j,nk+1)=0.0
        enddo
        enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do j=1,nj
        do k=nk,2,-1
        do i=1,ni
          w3d(i,j,k)=dpdzx(i,j,k)*w3d(i,j,k+1)+dpdzy(i,j,k)
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_sound=time_sound+mytime()

        if(n.eq.nloop) call bcw(w3d,1)

        if(terrain_flag) call bcwsfc(dzdx,dzdy,u3d,v3d,w3d)

#ifdef MPI
        if(n.eq.nloop)then
          call comm_3w_start(w3d,ww31,ww32,we31,we32,   &
                                 ws31,ws32,wn31,wn32,reqs_w)
        endif
#endif

!-------------------------
 
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          pp3d(i,j,k)=pp3d(i,j,k)+fpk(i,j,k)+alph*(    &
               w3d(i,j,k  )*(pk(i,j,k)+qk(i,j,k))          &
              +w3d(i,j,k+1)*(pk(i,j,k)-qk(i,j,k)) )
          if(abs(pp3d(i,j,k)).lt.smeps) pp3d(i,j,k)=0.0
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_sound=time_sound+mytime()

        if(n.eq.nloop .and. nrk.eq.3 .and. imoist.eq.1)then
          tem=0
        else
          if(n.eq.nloop) call bcs(pp3d)
        endif

#ifdef MPI
        if(n.eq.nloop)then
          if(nrk.lt.3.or.imoist.eq.0)then
            call comm_3s_start(pp3d,pw31,pw32,pe31,pe32,   &
                                    ps31,ps32,pn31,pn32,reqs_p)
          endif
        endif
#endif

        IF(n.lt.nloop)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ppd(i,j,k)=pp3d(i,j,k)-ppd(i,j,k)-ppten(i,j,k)
          enddo
          enddo
          enddo

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ppd(i,j,k)=pp3d(i,j,k)+kdiv*ppd(i,j,k)
          enddo
          enddo
          enddo

          if(timestats.ge.1) time_sound=time_sound+mytime()

          call bcs(ppd)

#ifdef MPI
          call comm_1s_start(ppd,pw1,pw2,pe1,pe2,   &
                                 ps1,ps2,pn1,pn2,reqs_p)
#endif
        ENDIF

!--------------------------------------------------------------------

      IF(thsmall.eq.1)THEN

!$omp parallel do default(shared)   &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          th3d(i,j,k)=th3d(i,j,k)+ftk(i,j,k)+alph*dts*rr0(i,j,k)*(    &
            w3d(i,j,k)*tk(i,j,k)+w3d(i,j,k+1)*tk(i,j,k+1)  )
        enddo
        enddo
        enddo

        IF(neweqts.ge.1 .and. imoist.eq.1)THEN
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            th3d(i,j,k)=th3d(i,j,k)+alph*(       &
                +w3d(i,j,k  )*sk(i,j,k)          &
                -w3d(i,j,k+1)*sk(i,j,k) )
          enddo
          enddo
          enddo
        ENDIF
        if(timestats.ge.1) time_sound=time_sound+mytime()

        if(n.eq.nloop)then
          if(nrk.lt.3.or.imoist.eq.0)then
            call bcs(th3d)
#ifdef MPI
            call comm_3s_start(th3d,sw31,sw32,se31,se32,   &
                                    ss31,ss32,sn31,sn32,reqs_s)
#endif
          endif
        endif

      ENDIF

#ifdef MPI
!--------------------------------------------------------------------
!  Finish comm

        if(n.eq.nloop)then
          call comm_3u_end(u3d,uw31,uw32,ue31,ue32,   &
                               us31,us32,un31,un32,reqs_u)
        endif

!-----

        if(n.eq.nloop)then
          call comm_3v_end(v3d,vw31,vw32,ve31,ve32,   &
                               vs31,vs32,vn31,vn32,reqs_v)
        endif

!-----

        if(n.eq.nloop)then
          call comm_3w_end(w3d,ww31,ww32,we31,we32,   &
                               ws31,ws32,wn31,wn32,reqs_w)
        endif

!-----

        if(n.eq.nloop)then
          if(nrk.lt.3.or.imoist.eq.0)then
            call comm_3s_end(pp3d,pw31,pw32,pe31,pe32,   &
                                  ps31,ps32,pn31,pn32,reqs_p)
            if(thsmall.eq.1)then
              call comm_3s_end(th3d,sw31,sw32,se31,se32,   &
                                    ss31,ss32,sn31,sn32,reqs_s)
            endif
          endif
        else
          call comm_1s_end(ppd,pw1,pw2,pe1,pe2,   &
                               ps1,ps2,pn1,pn2,reqs_p)
        endif

#endif

!--------------------------------------------------------------------

      ENDDO

      if(terrain_flag)then
        call bcwsfc(dzdx,dzdy,u3d,v3d,w3d)
        call bc2d(w3d(ib,jb,1))
      endif

!--------------------------------------------------------------------

      return
      end


