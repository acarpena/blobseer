

      subroutine solve(nstep,nloop2,nrec,prec,nwrite,nrst,           &
                   taptim,rsttim,cloudvar,qname,budname,qbudget,asq,bsq,   &
                   xh,rxh,uh,ruh,xf,rxf,uf,ruf,yh,vh,rvh,yf,vf,rvf,  &
                   sigma,sigmaf,tauh,taus,zh,mh,rmh,tauf,zf,mf,rmf,  &
                   rstat,pi0,rho0,prs0,thv0,th0,qv0,qc0,             &
                   ql0,rr0,rf0,rrf0,                                 &
                   zs,gz,dzdx,dzdy,rain,sws,                         &
                   thflux,qvflux,cdu,cdv,ce,rth0s,                   &
                   radbcw,radbce,radbcs,radbcn,                      &
                   dum1,dum2,dum3,dum4,divx,rho,prs,                 &
                   t11,t12,t13,t22,t23,t33,                          &
                   gx,u0,rru,ua,u3d,uten,uten1,                      &
                   gy,v0,rrv,va,v3d,vten,vten1,                      &
                   rrw,wa,w3d,wten,wten1,ppi,pp3d,ppten,sten,        &
                   tha,th3d,thten,thten1,thterm,tk,qa,q3d,qten,      &
                   kmh,kmv,khh,khv,tkea,tke3d,tketen,pta,pt3d,ptten, &
                   pdata,cfb,cfa,cfc,d1,d2,pdt,deft,rhs,trans,       &
                   reqs_u,reqs_v,reqs_w,reqs_s,reqs_p,reqs_tk,reqs_q,reqs_t, &
                   ww1,ww2,we1,we2,ws1,ws2,wn1,wn2,                  &
                   pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2,                  &
                   uw31,uw32,ue31,ue32,us31,us32,un31,un32,          &
                   vw31,vw32,ve31,ve32,vs31,vs32,vn31,vn32,          &
                   ww31,ww32,we31,we32,ws31,ws32,wn31,wn32,          &
                   sw31,sw32,se31,se32,ss31,ss32,sn31,sn32,          &
                   pw31,pw32,pe31,pe32,ps31,ps32,pn31,pn32,          &
                   tkw1,tkw2,tke1,tke2,tks1,tks2,tkn1,tkn2,          &
                   qw1,qw2,qe1,qe2,qs1,qs2,qn1,qn2,                  &
                   tw1,tw2,te1,te2,ts1,ts2,tn1,tn2,ploc,packet)
      use module_mp_thompson
      use module_mp_graupel
      implicit none

!-----------------------------------------------------------------------
!  Bryan Cloud Model (CM1) release 14  (cm1r14)
!  23 October 2009
!  http://www.mmm.ucar.edu/people/bryan/cm1/
!-----------------------------------------------------------------------

      include 'input.incl'
      include 'constants.incl'
      include 'timestat.incl'
#ifdef MPI
      include 'mpif.h'
      integer comm
#endif

!-----------------------------------------------------------------------
! Arrays and variables passed into solve

      integer :: nstep,nloop2,nrec,prec,nwrite,nrst
      real*8 :: taptim,rsttim
      logical, dimension(maxq) :: cloudvar
      character*3, dimension(maxq) :: qname
      character*6, dimension(maxq) :: budname
      real*8, dimension(nbudget) :: qbudget
      real*8, dimension(numq) :: asq,bsq
      real, dimension(ib:ie) :: xh,rxh,uh,ruh
      real, dimension(ib:ie+1) :: xf,rxf,uf,ruf
      real, dimension(jb:je) :: yh,vh,rvh
      real, dimension(jb:je+1) :: yf,vf,rvf
      real, dimension(kb:ke) :: sigma
      real, dimension(kb:ke+1) :: sigmaf
      real, dimension(ib:ie,jb:je,kb:ke) :: tauh,taus,zh,mh,rmh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: tauf,zf,mf,rmf
      real, dimension(stat_out) :: rstat
      real, dimension(ib:ie,jb:je,kb:ke) :: pi0,rho0,prs0,thv0,th0,qv0,qc0
      real, dimension(ib:ie,jb:je,kb:ke) :: ql0,rr0,rf0,rrf0
      real, dimension(itb:ite,jtb:jte) :: zs,gz,dzdx,dzdy
      real, dimension(itb:ite+1,jtb:jte,ktb:kte) :: gx
      real, dimension(itb:ite,jtb:jte+1,ktb:kte) :: gy
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
      real, dimension(ib:ie,jb:je) :: thflux,qvflux,cdu,cdv,ce,rth0s
      real, dimension(jb:je,kb:ke) :: radbcw,radbce
      real, dimension(ib:ie,kb:ke) :: radbcs,radbcn
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,dum3,dum4,divx,rho,prs
      real, dimension(ib:ie,jb:je,kb:ke) :: t11,t12,t13,t22,t23,t33
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,rru,ua,u3d,uten,uten1
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,rrv,va,v3d,vten,vten1
      real, dimension(ib:ie,jb:je,kb:ke+1) :: rrw,wa,w3d,wten,wten1
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,pp3d,ppten,sten
      real, dimension(ib:ie,jb:je,kb:ke) :: tha,th3d,thten,thten1,thterm,tk
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa,q3d,qten
      real, dimension(ibc:iec,jbc:jec,kbc:kec) :: kmh,kmv,khh,khv
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea,tke3d,tketen
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta,pt3d,ptten
      real, dimension(npvals,nparcels) :: pdata
      real, dimension(ipb:ipe,jpb:jpe,kpb:kpe) :: cfb
      real, dimension(kpb:kpe) :: cfa,cfc,d1,d2
      complex, dimension(ipb:ipe,jpb:jpe,kpb:kpe) :: pdt,deft
      complex, dimension(ipb:ipe,jpb:jpe) :: rhs,trans
      integer, dimension(rmp) :: reqs_u,reqs_v,reqs_w,reqs_s,reqs_p,reqs_tk
      integer, dimension(rmp,numq) :: reqs_q
      integer, dimension(rmp,npt) :: reqs_t
      real, dimension(jmp,kmp-1) :: ww1,ww2,we1,we2
      real, dimension(imp,kmp-1) :: ws1,ws2,wn1,wn2
      real, dimension(jmp,kmp) :: pw1,pw2,pe1,pe2
      real, dimension(imp,kmp) :: ps1,ps2,pn1,pn2
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
      real, dimension(cmp,jmp,kmt)   :: tkw1,tkw2,tke1,tke2
      real, dimension(imp,cmp,kmt)   :: tks1,tks2,tkn1,tkn2
      real, dimension(cmp,jmp,kmp,numq) :: qw1,qw2,qe1,qe2
      real, dimension(imp,cmp,kmp,numq) :: qs1,qs2,qn1,qn2
      real, dimension(cmp,jmp,kmp,npt) :: tw1,tw2,te1,te2
      real, dimension(imp,cmp,kmp,npt) :: ts1,ts2,tn1,tn2
      real, dimension(3,nparcels) :: ploc
      real, dimension(npvals+1,nparcels) :: packet

!-----------------------------------------------------------------------
! Arrays and variables defined inside solve

      integer i,j,k,n,nrk,icom
      real :: delqv,delpi,delth,delt,fac

      logical simple_comm

      real dttmp,rtime,rdt,tem,thrad,prad,ql
      real*8 afoo,bfoo
      logical :: getdbz
      real*8 :: bud(nk)
      real*8 :: bud2(nj)


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!cc   First, get turbulence coefficients   ccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

#ifdef MPI
      nf=0
      nu=0
      nv=0
      nw=0
      
#     ifdef MAINC
      call get_registered_comm(comm)
#     else
      comm = MPI_COMM_WORLD
#     endif
#endif

      afoo=0.0d0
      bfoo=0.0d0
!--------------------------------------------------------------------
!  surface scheme:

      if(idrag.eq.1.or.isfcflx.eq.1)then
        call getcecd(cdu,cdv,ce,u0,v0,rf0,dum1,dum2,dum3,ua,va)
      endif

      ! get surface drag
      if(idrag.eq.1)then
        call sfcdrag(cdu,cdv,u0,v0,rf0,dum1,dum2,t13,t23,ua,va)
      endif

      ! get surface flux
      if(isfcflx.eq.1)then
        call sfcflux(ruh,xf,rvh,ce,zh,pi0,thv0,th0,u0,v0,thflux,qvflux, &
                     rho,dum1,dum2,dum3,ua,va,ppi,tha,qa(ibm,jbm,kbm,nqv), &
                     qbudget(8))
      endif

!--------------------------------------------------------------------
!                 For turbulence section only:
!  dum1 = Brunt-Vaisala frequency (N_m^2) (nm)
!  dum2 = Vertical deformation terms (S_v^2) (defsq)
!  dum3 = Horizontal deformation terms (S_h^2) (defh)
!
!  Arrays available for temporary storage:
!  dum4,divx,ppten,sten,thterm

      IF(iturb.ge.1)THEN

        call calcnm(mf,pi0,thv0,th0,cloudvar,dum1,dum2,dum3,dum4,divx,   &
                    prs,ppi,tha,qa)

        call calcdef(xh,rxh,uh,xf,rxf,uf,vh,vf,mh,mf,dum2,dum3,   &
                     divx,ppten,ua,va,wa,t11,t12,t13,t22,t23,t33,gx,gy)

      ENDIF

!--------------------------------------------------------------------
!  tke scheme

      IF(iturb.eq.1)THEN
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nkt
        do j=1,nj
        do i=1,ni
          tketen(i,j,k)=0.0
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_misc=time_misc+mytime()

        call turbtke(ruh,rvh,rmh,mf,rmf,th0,thflux,qvflux,rth0s,   &
                     dum1,dum2,dum3,dum4,divx,ppten,sten,thterm,   &
                     kmh,kmv,khh,khv,tkea,tketen,t13,t23,ua,va)

        call turbt(xh,rxh,uh,xf,uf,vh,vf,mh,mf,rho0,rr0,rf0,rrf0,   &
                   dum1,dum2,dum3,dum4,divx,tkea,tketen,kmh,kmv,gx,gy)

        if(idiff.eq.1)then
          if(difforder.eq.2)then
            call diff2w(1,0,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,tkea,tketen)
          elseif(difforder.eq.6)then
            call diff6w(dum1,dum2,dum3,tkea,tketen)
          endif
        endif

!-------------------------------------------------
!  Smagorinsky scheme

      ELSEIF(iturb.eq.2)THEN

        call turbsmag(ruh,rvh,rmh,mf,rmf,th0,thflux,qvflux,rth0s,  &
                      dum1,dum2,dum3,dum4,divx,sten,               &
                      kmh,kmv,khh,khv,t13,t23,ua,va)

!-------------------------------------------------
!  Smagorinsky scheme for axisymmetric simulations

      ELSEIF(iturb.eq.3)THEN

        call turbparam(ruh,rvh,rmh,mf,rmf,th0,thflux,qvflux,rth0s,  &
                         dum1,dum2,dum3,kmh,kmv,khh,khv,t13,t23,ua,va)

      ENDIF

!-------------------------------------------------
!  Get turbulent stresses:

      IF(iturb.ge.1)THEN

        call gettau(xf,rxf,rho0,rf0,kmh,kmv,t11,t12,t13,t22,t23,t33,ua)

      ENDIF

!--------------------------------------------------------------------
!  Dissipative heating term:  NOTE: epsilon is stored in the thterm array

      IF(idiss.eq.1)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          thterm(i,j,k) = 0.0
        enddo
        enddo
        enddo
      ENDIF

      IF(iturb.ge.1.and.idiss.eq.1)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          thten(i,j,k) = dum1(i,j,k)
        enddo
        enddo
        enddo

        call getepsilon(rxh,uh,xf,rxf,uf,yh,vh,yf,vf,mh,mf,rr0,rrf0,   &
                        dum1,dum2,dum3,dum4,divx,ppten,sten,thterm,    &
                        t11,t12,t13,t22,t23,t33,ua,va,wa,gx,gy)

      ENDIF

!--------------------------------------------------------------------


!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CC   Pre-RK calculations   CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

 
!-------------------------------------------------------------------
!  Parcel update

      if(iprcl.eq.1)then
        ! rtime valid at beginning of time step
        rtime=float(nstep-1)*dtl
        call parcel_driver(prec,xh,uh,ruh,yh,vh,rvh,zh,mh,rmh,mf,        &
                           pi0,thv0,th0,dum1,dum2,dum3,dum4,divx,prs,    &
                           ua,va,wa,ppi,thten,tha,qa,khv,pdata,rtime,    &
                           ploc,packet,reqs_p,                           &
                           pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2)
      endif

!--------------------------------------------------------------------
!  radbc
 
      if(irbc.eq.1)then

        if(ibw.eq.1 .or. ibe.eq.1) call radbcew(radbcw,radbce,ua)
 
        if(ibs.eq.1 .or. ibn.eq.1) call radbcns(radbcs,radbcn,va)

      endif

!--------------------------------------------------------------------
!  set ppten to zero

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        ppten(i,j,k)=0.0
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

!--------------------------------------------------------------------
!  U-equation
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni+1
        uten1(i,j,k)=0.
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(idiff.ge.1)then
        if(difforder.eq.2)then
          call diff2u(1,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,ua,uten1)
        elseif(difforder.eq.6)then
          call diff6u(u0,dum1,dum2,dum3,ua,uten1)
        endif
      endif

      if(irdamp.eq.1.or.hrdamp.eq.1)then
        call rdampu(tauh,u0,ua,uten1)
      endif

      if(dns.eq.1)then
        call diff2u(2,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,ua,uten1)
      endif
 
      if(iturb.ge.1)then
        call turbu(xh,xf,rxf,uf,vh,mh,mf,rmf,rho0,rf0,dum1,dum2,dum3,dum4,ua,uten1,wa,t11,t12,t13,t22,kmv,gx,gy)
      endif

!--------------------------------------------------------------------
!  V-equation
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj+1
      do i=1,ni
        vten1(i,j,k)=0.
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(idiff.ge.1)then
        if(difforder.eq.2)then
          call diff2v(1,xh,uh,rxf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,va,vten1)
        elseif(difforder.eq.6)then
          call diff6v(v0,dum1,dum2,dum3,va,vten1)
        endif
      endif

      if(irdamp.eq.1.or.hrdamp.eq.1)then
        call rdampv(tauh,v0,va,vten1)
      endif

      if(dns.eq.1)then
        call diff2v(2,xh,uh,rxf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,va,vten1)
      endif
 
      if(iturb.ge.1)then
        call turbv(xh,rxh,uh,xf,vf,mh,mf,rho0,rf0,dum1,dum2,dum3,dum4,va,vten1,wa,t12,t22,t23,kmv,gx,gy)
      endif
 
!--------------------------------------------------------------------
!  W-equation
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk+1
      do j=1,nj
      do i=1,ni
        wten1(i,j,k)=0.0
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(idiff.ge.1)then
        if(difforder.eq.2)then
          call diff2w(1,1,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,wa,wten1)
        elseif(difforder.eq.6)then
          call diff6w(dum1,dum2,dum3,wa,wten1)
        endif
      endif

      if(irdamp.eq.1.or.hrdamp.eq.1)then
        call rdampw(tauf,wa,wten1)
      endif

      if(dns.eq.1)then
        call diff2w(2,1,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,dum4,thterm,wa,wten1)
      endif
 
      if(iturb.ge.1)then
        call turbw(xh,rxh,uh,xf,vh,mh,mf,rho0,rf0,rrf0,dum1,dum2,dum3,dum4,wa,wten1,t13,t23,t33,kmv,gx,gy)
      endif

!--------------------------------------------------------------------
!  THETA-equation
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        thten1(i,j,k)=0.0
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if(idiff.eq.1)then
        if(difforder.eq.2)then
          call diff2s(1,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,tha,thten1)
        elseif(difforder.eq.6)then
          call diff6s(ql0,dum1,dum2,dum3,tha,thten1)
        endif
      endif

      if(irdamp.eq.1.or.hrdamp.eq.1)then
        call rdamps(taus,th0,tha,thten1)
      endif

!----- cvm (if needed) -----!

      IF( neweqts.ge.1 .and. (idiss.eq.1.or.rterm.eq.1) )THEN
        ! store cvm in dum1:
        ! store ql  in dum2:
        ! store qi  in dum3:
        call getqli(0,qa,dum2,dum3)
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=cv+cvv*qa(i,j,k,nqv)+cpl*dum2(i,j,k)+cpi*dum3(i,j,k)
        enddo
        enddo
        enddo
      ENDIF

!----- store appropriate rho for budget calculations in dum2 -----!

      IF(axisymm.eq.1)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum2(i,j,k) = rho(i,j,k)*pi*(xf(i+1)**2-xf(i)**2)/(dx*dy)
        enddo
        enddo
        enddo
      ELSE
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum2(i,j,k) = rho(i,j,k)
        enddo
        enddo
        enddo
      ENDIF

!---- Dissipative heating term:

      IF(idiss.eq.1)THEN
        ! use thterm array to store epsilon
        if(imoist.eq.1.and.neweqts.ge.1)then
          ! moist, new equations:
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            thten1(i,j,k)=thten1(i,j,k)   &
                        +thterm(i,j,k)/( cpdcv*dum1(i,j,k)*(pi0(i,j,k)+ppi(i,j,k)) )
          enddo
          enddo
          enddo
        else
          ! traditional equations:
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            thten1(i,j,k)=thten1(i,j,k)   &
                        +thterm(i,j,k)/( cp*(pi0(i,j,k)+ppi(i,j,k)) )
          enddo
          enddo
          enddo
        endif
      ENDIF

!---- Rotunno-Emanuel "radiation" term
!---- (currently capped at 2 K/day ... see RE87 p 546)

      IF(rterm.eq.1)THEN
!$omp parallel do default(shared)  &
!$omp private(k)
        do k=1,nk
          bud(k)=0.0d0
        enddo
!$omp parallel do default(shared)  &
!$omp private(i,j,k,thrad,prad)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ! NOTE:  thrad is a POTENTIAL TEMPERATURE tendency
          thrad = -tha(i,j,k)/(12.0*3600.0)
          if( tha(i,j,k).gt. 1.0 ) thrad = -1.0/(12.0*3600.0)
          if( tha(i,j,k).lt.-1.0 ) thrad =  1.0/(12.0*3600.0)
          thten1(i,j,k)=thten1(i,j,k)+thrad
          ! associated pressure tendency:
          prad = (pi0(i,j,k)+ppi(i,j,k))*rddcv*thrad/(th0(i,j,k)+tha(i,j,k))
          ! budget:
          bud(k) = bud(k) + dum1(i,j,k)*dum2(i,j,k)*ruh(i)*rvh(j)*rmh(i,j,k)*( &
                            thrad*(pi0(i,j,k)+ppi(i,j,k))    &
                           + prad*(th0(i,j,k)+tha(i,j,k)) )
        enddo
        enddo
        enddo
        tem = dtl*dx*dy*dz
        do k=1,nk
          qbudget(10) = qbudget(10) + tem*bud(k)
        enddo
      ENDIF
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if( (iturb.ge.1).or.(dns.eq.1) )then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=0,nk+1
        do j=0,nj+1
        do i=0,ni+1
          sten(i,j,k)=th0(i,j,k)+tha(i,j,k)
        enddo
        enddo
        enddo
      endif

      if(dns.eq.1)then
        call diff2s(2,rxh,uh,xf,uf,vh,vf,mh,mf,dum1,dum2,dum3,sten,thten1)
      endif

      ! call to turbs should be last
      if(iturb.ge.1)then
        call turbs(1,xh,rxh,uh,xf,uf,vh,vf,mh,mf,rho0,rr0,rf0,thflux,   &
                   dum1,dum2,dum3,dum4,divx,sten,thten1,khh,khv,gx,gy)
      endif

!-------------------------------------------------------------------
!  contribution to pressure tendency from potential temperature:

      IF(neweqts.ge.1.or.imoist.eq.0)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ppten(i,j,k) = ppten(i,j,k) + thten1(i,j,k)*rddcv   &
                        *(pi0(i,j,k)+ppi(i,j,k))/(th0(i,j,k)+tha(i,j,k))
        enddo
        enddo
        enddo
      ENDIF

!-------------------------------------------------------------------
!  budget calculations:

      if(isfcflx.eq.1.and.imoist.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(j)
        do j=1,nj
          bud2(j) = 0.0d0
        enddo
!$omp parallel do default(shared)  &
!$omp private(i,j,k,delpi,delth,delqv,delt,n)
        do j=1,nj
        do i=1,ni
          k = 1
          delth = rf0(i,j,1)*rr0(i,j,1)*rdz*mh(i,j,1)*thflux(i,j)
          delqv = rf0(i,j,1)*rr0(i,j,1)*rdz*mh(i,j,1)*qvflux(i,j)
          delpi = rddcv*(pi0(i,j,1)+ppi(i,j,1))*(           &
                                delqv/(eps+qa(i,j,1,nqv))   &
                               +delth/(th0(i,j,1)+tha(i,j,1))  )
          delt = (pi0(i,j,k)+ppi(i,j,k))*delth   &
                +(th0(i,j,k)+tha(i,j,k))*delpi
          bud2(j) = bud2(j) + dum2(i,j,k)*ruh(i)*rvh(j)*rmh(i,j,k)*(        &
                  cv*delt                                                   &
                + cvv*qa(i,j,k,nqv)*delt                                    &
                + cvv*(pi0(i,j,k)+ppi(i,j,k))*(th0(i,j,k)+tha(i,j,k))*delqv &
                + g*zh(i,j,k)*delqv   )
          do n=nql1,nql2
            bud2(j) = bud2(j) + dum2(i,j,k)*ruh(i)*rvh(j)*rmh(i,j,k)*cpl*qa(i,j,k,n)*delt
          enddo
          if(iice.eq.1)then
            do n=nqs1,nqs2
              bud2(j) = bud2(j) + dum2(i,j,k)*ruh(i)*rvh(j)*rmh(i,j,k)*cpi*qa(i,j,k,n)*delt
            enddo
          endif
        enddo
        enddo
        tem = dtl*dx*dy*dz
        do j=1,nj
          qbudget(9) = qbudget(9) + tem*bud2(j)
        enddo
        if(timestats.ge.1) time_misc=time_misc+mytime()
      endif

!-------------------------------------------------------------------
!  Passive Tracers

      if(iptra.eq.1)then
        do n=1,npt
          call gettenq(0,xh,rxh,uh,xf,uf,vh,vf,mh,mf,ql0,rho0,rr0,rf0,qvflux,   &
                       dum1,dum2,dum3,dum4,divx,                           &
                       pta(ib,jb,kb,n),ptten(ib,jb,kb,n),khh,khv,gx,gy)
        enddo
      endif

!-------------------------------------------------------------------
!  Moisture

      if(imoist.eq.1)then
        DO n=1,numq
          if(n.eq.nqv)then
            call gettenq(1,xh,rxh,uh,xf,uf,vh,vf,mh,mf,qv0,rho0,rr0,rf0,qvflux,   &
                         dum1,dum2,dum3,dum4,divx,                      &
                         qa(ib,jb,kb,n),qten(ib,jb,kb,n),khh,khv,gx,gy)
            IF(neweqts.ge.1)THEN
              ! contribution to pressure tendency from water vapor:
!$omp parallel do default(shared)   &
!$omp private(i,j,k)
              do k=1,nk
              do j=1,nj
              do i=1,ni
                ppten(i,j,k)=ppten(i,j,k)+qten(i,j,k,n)   &
                            *rddcv*(pi0(i,j,k)+ppi(i,j,k))/(eps+qa(i,j,k,n))
              enddo
              enddo
              enddo
              if(timestats.ge.1) time_misc=time_misc+mytime()
            ENDIF
          else
            call gettenq(0,xh,rxh,uh,xf,uf,vh,vf,mh,mf,ql0,rho0,rr0,rf0,qvflux,   &
                         dum1,dum2,dum3,dum4,divx,                      &
                         qa(ib,jb,kb,n),qten(ib,jb,kb,n),khh,khv,gx,gy)
          endif
        ENDDO
      endif

!-------------------------------------------------------------------


!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CC   Begin RK section   CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

!--------------------------------------------------------------------
! RK2 begin

      DO NRK=1,3

        dttmp=dtl/float(4-nrk)

        IF(.not.terrain_flag)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=2,nk
          do j=0,nj+1
          do i=0,ni+1
            rrw(i,j,k)=rf0(i,j,k)*w3d(i,j,k)
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_advs=time_advs+mytime()

        ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=2,nk
          do j=1,nj
          do i=1,ni
            rrw(i,j,k)=rf0(i,j,k)*( w3d(i,j,k)*gz(i,j)                 &
                 +0.25*(sigma(k)-zt)/(zt-zs(i,j))*(                    &
                       ( (u3d(i+1,j,k-1)+u3d(i  ,j,k-1))               &
                        +(u3d(i+1,j,k  )+u3d(i  ,j,k  )) )*dzdx(i,j)   &
                      +( (v3d(i,j+1,k-1)+v3d(i,j  ,k-1))               &
                        +(v3d(i,j+1,k  )+v3d(i,j  ,k  )) )*dzdy(i,j) ) )
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_advs=time_advs+mytime()

          call bcw(rrw,0)
#ifdef MPI
          call comm_1w_start(rrw,ww1,ww2,we1,we2,   &
                                 ws1,ws2,wn1,wn2,reqs_w)
#endif

        ENDIF

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=0,nj+1
        do i=0,ni+2
          rru(i,j,k)=0.5*(rho0(i-1,j,k)+rho0(i,j,k))*u3d(i,j,k)
        enddo
        enddo
        enddo

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=0,nj+2
        do i=0,ni+1
          rrv(i,j,k)=0.5*(rho0(i,j-1,k)+rho0(i,j,k))*v3d(i,j,k)
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_advs=time_advs+mytime()

#ifdef MPI
        if(terrain_flag)then
          call comm_1w_end(rrw,ww1,ww2,we1,we2,   &
                               ws1,ws2,wn1,wn2,reqs_w)
        endif
#endif

        IF(.not.terrain_flag)THEN

!------------
        IF(axisymm.eq.0)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=0,ni+1
            divx(i,j,k)=(rru(i+1,j,k)-rru(i,j,k))*rdx*uh(i)        &
                       +(rrv(i,j+1,k)-rrv(i,j,k))*rdy*vh(j)        &
                       +(rrw(i,j,k+1)-rrw(i,j,k))*rdz*mh(i,j,k)
          enddo
          enddo
          enddo

        ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=0,ni+1
            divx(i,j,k)=(xf(i+1)*rru(i+1,j,k)-xf(i)*rru(i,j,k))*rdx*uh(i)*rxh(i)   &
                       +(rrw(i,j,k+1)-rrw(i,j,k))*rdz*mh(i,j,k)
          enddo
          enddo
          enddo

        ENDIF
!------------

        ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=0,ni+1
            divx(i,j,k)=(rru(i+1,j,k)-rru(i,j,k))*rdx*uh(i)        &
                       +(rrv(i,j+1,k)-rrv(i,j,k))*rdy*vh(j)        &
                       +(rrw(i,j,k+1)-rrw(i,j,k))*rdz*mh(i,j,k)/gz(i,j)
          enddo
          enddo
          enddo

        ENDIF
        if(timestats.ge.1) time_divx=time_divx+mytime()

!--------------------------------------------------------------------
!  TKE advection
 
        if(iturb.eq.1)then
          call integtkerk(xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,           &
                          rho0,rr0,rf0,rrf0,                              &
                          dum1,dum2,dum3,dum4,divx,t12,rru,rrv,rrw,       &
                          wten,tkea,tke3d,tketen,dttmp,nrk                &
#ifdef MPI
              ,tkw1,tkw2,tke1,tke2,tks1,tks2,tkn1,tkn2,reqs_tk            &
#endif
                          )
        endif

!--------------------------------------------------------------------
!  Passive Tracers

        if(iptra.eq.1)then
          DO n=1,npt
            call integqrk(2,bfoo,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,        &
                          rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,t12,     &
                          rru,rrv,rrw,sten,pta(ib,jb,kb,n),pt3d(ib,jb,kb,n),  &
                          ptten(ib,jb,kb,n),dttmp,nrk                         &
#ifdef MPI
                     ,tw1(1,1,1,n),tw2(1,1,1,n),te1(1,1,1,n),te2(1,1,1,n)     &
                     ,ts1(1,1,1,n),ts2(1,1,1,n),tn1(1,1,1,n),tn2(1,1,1,n)     &
                     ,reqs_t(1,n)                                             &
#endif
                          )
          ENDDO
        endif

!--------------------------------------------------------------------
!  Calculate misc. variables
!
!    These arrays store variables that are used later in the
!    SOUND subroutine.  Do not modify t11 or t22 until after sound!
!    dum1 = vapor
!    dum2 = all liquid
!    dum3 = all solid

        IF(imoist.eq.1)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=0,ni+1
            dum1(i,j,k)=max(q3d(i,j,k,nqv),0.0)
          enddo
          enddo
          enddo

          call getqli(1,q3d,dum2,dum3)

        ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=0,ni+1
            dum1(i,j,k)=0.0
            dum2(i,j,k)=0.0
            dum3(i,j,k)=0.0
          enddo
          enddo
          enddo

        ENDIF

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=0,nj+1
        do i=0,ni+1
          t11(i,j,k)=(th0(i,j,k)+th3d(i,j,k))*(1.0+reps*dum1(i,j,k))     &
                     /(1.0+dum1(i,j,k)+max(0.0,dum2(i,j,k))+max(0.0,dum3(i,j,k)))
        enddo
        enddo
        enddo

        IF(thsmall.eq.1.and.imoist.eq.1)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            t12(i,j,k)=dum1(i,j,k)
            t13(i,j,k)=max(0.0,dum2(i,j,k))+max(0.0,dum3(i,j,k))
          enddo
          enddo
          enddo
        ENDIF

        if(timestats.ge.1) time_buoyan=time_buoyan+mytime()

!--------------------------------------------------------------------
!  THETA-equation
 
        icom=1
        if(imoist.eq.1.and.neweqts.eq.2) icom=0

        call integthrk(icom,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,          &
                       th0,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,          &
                       divx,t23,t22,gx,rru,u3d,gy,rrv,v3d,rrw,w3d,         &
                       tha,th3d,thten1,thten,thterm,tk,q3d,dttmp,nrk       &
#ifdef MPI
         ,sw31,sw32,se31,se32,ss31,ss32,sn31,sn32,reqs_s                   &
#endif
                       )

!--------------------------------------------------------------------
! Moisture

        if(imoist.eq.1)then
          DO n=1,numq
            icom=1
            if(neweqts.eq.2.and.(n.eq.nqv.or.n.eq.2.or.n.eq.4)) icom=0
            call integqrk(icom,bsq(n),xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,mf,   &
                          rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,t23,     &
                          rru,rrv,rrw,sten,qa(ib,jb,kb,n),q3d(ib,jb,kb,n),    &
                          qten(ib,jb,kb,n),dttmp,nrk                          &
#ifdef MPI
                     ,qw1(1,1,1,n),qw2(1,1,1,n),qe1(1,1,1,n),qe2(1,1,1,n)     &
                     ,qs1(1,1,1,n),qs2(1,1,1,n),qn1(1,1,1,n),qn2(1,1,1,n)     &
                     ,reqs_q(1,n)                                             &
#endif
                          )
          ENDDO
        endif

!--------------------------------------------------------------------
!  Pressure equation

      IF(psolver.le.3)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          sten(i,j,k)=ppten(i,j,k)
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_misc=time_misc+mytime()

        if(hadvorder.eq.5)then
          call adv5s(0,bfoo,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,         &
                     rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,t23,   &
                     rru,rrv,rrw,ppi,pp3d,sten,0,dttmp)
        elseif(hadvorder.eq.6)then
          call adv6s(0,bfoo,xh,rxh,uh,ruh,xf,vh,rvh,gz,mh,rmh,         &
                     rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,t23,   &
                     rru,rrv,rrw,ppi,pp3d,sten,0,dttmp)
        endif

        IF(imoist.eq.1.and.neweqts.eq.2)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            t33(i,j,k)=ppi(i,j,k)+dttmp*sten(i,j,k)
            t23(i,j,k)=t33(i,j,k)
          enddo
          enddo
          enddo

          IF(thsmall.eq.1)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
            do k=1,nk
            do j=1,nj
            do i=1,ni
              th3d(i,j,k)=tha(i,j,k)+dttmp*thten(i,j,k)
            enddo
            enddo
            enddo
          ENDIF
          if(timestats.ge.1) time_satadj=time_satadj+mytime()

          call calcprs(pi0,prs,t33)
          call calcrho(pi0,th0,rho,prs,t33,th3d,q3d)
          IF(nrk.ge.3)THEN
            IF(axisymm.eq.0)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
              do k=1,nk
              do j=1,nj
              do i=1,ni
                dum3(i,j,k)=rho(i,j,k)
              enddo
              enddo
              enddo
            ELSE
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
              do k=1,nk
              do j=1,nj
              do i=1,ni
                dum3(i,j,k) = rho(i,j,k)*pi*(xf(i+1)**2-xf(i)**2)/(dx*dy)
              enddo
              enddo
              enddo
            ENDIF
          ENDIF
          if(ptype.eq.1.or.ptype.eq.3.or.ptype.eq.5.or.ptype.eq.6)then
            call satadj(nrk,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,   &
                        rho,dum3,t33,prs,th3d,q3d)
          elseif(ptype.eq.2)then
            call satadj_ice(nrk,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,   &
                            rho,dum3,t33,prs,th3d,                      &
                q3d(ib,jb,kb,1),q3d(ib,jb,kb,2),q3d(ib,jb,kb,3),   &
                q3d(ib,jb,kb,4),q3d(ib,jb,kb,5),q3d(ib,jb,kb,6))
          endif

          IF(thsmall.eq.1)THEN
            rdt=1.0/dttmp
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
            do k=1,nk
            do j=1,nj
            do i=1,ni
              thten(i,j,k)=(th3d(i,j,k)-tha(i,j,k))*rdt
            enddo
            enddo
            enddo
            if(timestats.ge.1) time_satadj=time_satadj+mytime()
          ENDIF

          if(nrk.lt.3)then
            if(thsmall.eq.0)then
              call bcs(th3d)
#ifdef MPI
              call comm_3s_start(th3d,sw31,sw32,se31,se32,   &
                                      ss31,ss32,sn31,sn32,reqs_s)
#endif
            endif
            call bcs(q3d(ib,jb,kb,nqv))
#ifdef MPI
            call comm_3s_start(q3d(ib,jb,kb,nqv),                            &
              qw1(1,1,1,nqv),qw2(1,1,1,nqv),qe1(1,1,1,nqv),qe2(1,1,1,nqv),   &
              qs1(1,1,1,nqv),qs2(1,1,1,nqv),qn1(1,1,1,nqv),qn2(1,1,1,nqv),   &
              reqs_q(1,nqv))
#endif
            call bcs(q3d(ib,jb,kb,2))
#ifdef MPI
            call comm_3s_start(q3d(ib,jb,kb,2),                            &
              qw1(1,1,1,2),qw2(1,1,1,2),qe1(1,1,1,2),qe2(1,1,1,2),   &
              qs1(1,1,1,2),qs2(1,1,1,2),qn1(1,1,1,2),qn2(1,1,1,2),   &
              reqs_q(1,2))
#endif
          endif
          if(nrk.lt.3.and.iice.eq.1)then
            call bcs(q3d(ib,jb,kb,4))
#ifdef MPI
            call comm_3s_start(q3d(ib,jb,kb,4),                            &
              qw1(1,1,1,4),qw2(1,1,1,4),qe1(1,1,1,4),qe2(1,1,1,4),   &
              qs1(1,1,1,4),qs2(1,1,1,4),qn1(1,1,1,4),qn2(1,1,1,4),   &
              reqs_q(1,4))
#endif
          endif

          rdt=1.0/dttmp

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            sten(i,j,k)=sten(i,j,k)+(t33(i,j,k)-t23(i,j,k))*rdt
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_satadj=time_satadj+mytime()

        ENDIF

      ENDIF

!--------------------------------------------------------------------
!  U-equation

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni+1
          uten(i,j,k)=uten1(i,j,k)
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_misc=time_misc+mytime()
 
        if(icor.eq.1)then

        if(pertcor.eq.1)then

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj+1
          do i=0,ni+1
            dum1(i,j,k)=v3d(i,j,k)-v0(i,j,k)
          enddo
          enddo
          enddo

        else

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj+1
          do i=0,ni+1
            dum1(i,j,k)=v3d(i,j,k)
          enddo
          enddo
          enddo

        endif

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni+1
            uten(i,j,k)=uten(i,j,k)+fcor*             &
             0.25*( (dum1(i  ,j,k)+dum1(i  ,j+1,k))   &
                   +(dum1(i-1,j,k)+dum1(i-1,j+1,k)) )
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_cor=time_cor+mytime()

        endif

        if(axisymm.eq.1)then

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=2,ni+1
            uten(i,j,k)=uten(i,j,k)+0.5*(   &
                 ( v3d(i-1,j,k)**2)*rxh(i-1)+(v3d(i,j,k)**2)*rxh(i) )
          enddo
          enddo
          enddo

        endif

        if(hadvorder.eq.5)then
          call adv5u(xf,rxf,uf,vh,gz,mh,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                     rru,u3d,uten,rrv,rrw)
        elseif(hadvorder.eq.6)then
          call adv6u(xf,rxf,uf,vh,gz,mh,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                     rru,u3d,uten,rrv,rrw)
        endif

!--------------------------------------------------------------------
!  V-equation
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj+1
        do i=1,ni
          vten(i,j,k)=vten1(i,j,k)
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_misc=time_misc+mytime()
 
        if(icor.eq.1)then

!--------------
          IF(axisymm.eq.0)THEN

        if(pertcor.eq.1)then

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=1,ni+1
            dum1(i,j,k)=u3d(i,j,k)-u0(i,j,k)
          enddo
          enddo
          enddo

        else

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=0,nj+1
          do i=1,ni+1
            dum1(i,j,k)=u3d(i,j,k)
          enddo
          enddo
          enddo

        endif

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj+1
          do i=1,ni
            vten(i,j,k)=vten(i,j,k)-fcor*             &
             0.25*( (dum1(i,j  ,k)+dum1(i+1,j  ,k))   &
                   +(dum1(i,j-1,k)+dum1(i+1,j-1,k)) )
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_cor=time_cor+mytime()

          ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            vten(i,j,k)=vten(i,j,k)-fcor*0.5*(xf(i)*u3d(i,j,k)+xf(i+1)*u3d(i+1,j,k))*rxh(i)
          enddo
          enddo
          enddo

          ENDIF
!--------------

        endif

        if(axisymm.eq.1)then

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            vten(i,j,k)=vten(i,j,k)-(v3d(i,j,k)*rxh(i))*0.5*(xf(i)*u3d(i,j,k)+xf(i+1)*u3d(i+1,j,k))*rxh(i)
          enddo
          enddo
          enddo

        endif

        if(hadvorder.eq.5)then
          call adv5v(xh,rxh,uh,xf,vf,gz,mh,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                     rru,rrv,v3d,vten,rrw)
        elseif(hadvorder.eq.6)then
          call adv6v(xh,rxh,uh,xf,vf,gz,mh,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                     rru,rrv,v3d,vten,rrw)
        endif

!--------------------------------------------------------------------
!  W-equation

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk+1
        do j=1,nj
        do i=1,ni
          wten(i,j,k)=wten1(i,j,k)
        enddo
        enddo
        enddo
        if(timestats.ge.1) time_misc=time_misc+mytime()
 
        if( (thsmall.eq.0) .or. (thsmall.eq.1.and.imoist.eq.1) )   &
        call buoyan(thv0,qv0,qc0,dum2,wten,t11,t12,t13)

        if(hadvorder.eq.5)then
          call adv5w(xh,rxh,uh,xf,vh,gz,mf,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                     rru,rrv,rrw,w3d,wten)
        elseif(hadvorder.eq.6)then
          call adv6w(xh,rxh,uh,xf,vh,gz,mf,rho0,rr0,rf0,rrf0,dum1,dum2,dum3,dum4,divx,  &
                     rru,rrv,rrw,w3d,wten)
        endif


#ifdef MPI
!--------------------------------------------------------------------
!  Finish comm

        if(iturb.eq.1)then
          call comm_3t_end(tke3d,tkw1,tkw2,tke1,tke2,   &
                                 tks1,tks2,tkn1,tkn2,reqs_tk)
        endif
        if(iptra.eq.1)then
          do n=1,npt
            call comm_3s_end(pt3d(ib,jb,kb,n),                           &
                  tw1(1,1,1,n),tw2(1,1,1,n),te1(1,1,1,n),te2(1,1,1,n),   &
                  ts1(1,1,1,n),ts2(1,1,1,n),tn1(1,1,1,n),tn2(1,1,1,n),   &
                  reqs_t(1,n))
          enddo
        endif
        IF(nrk.lt.3.or.imoist.eq.0)THEN
          icom=1
          if(imoist.eq.1.and.neweqts.eq.2) icom=0
          if(icom.eq.1.and.thsmall.eq.0)then
            call comm_3s_end(th3d,sw31,sw32,se31,se32,   &
                                  ss31,ss32,sn31,sn32,reqs_s)
          endif
        ENDIF
        IF(nrk.lt.3.and.imoist.eq.1)THEN
          DO n=1,numq
            icom=1
            if(neweqts.eq.2.and.(n.eq.nqv.or.n.eq.2.or.n.eq.4)) icom=0
            if(icom.eq.1)then
              call comm_3s_end(q3d(ib,jb,kb,n),                            &
                    qw1(1,1,1,n),qw2(1,1,1,n),qe1(1,1,1,n),qe2(1,1,1,n),   &
                    qs1(1,1,1,n),qs2(1,1,1,n),qn1(1,1,1,n),qn2(1,1,1,n),   &
                    reqs_q(1,n))
            endif
          ENDDO
        ENDIF
        icom=1
        if(imoist.eq.1.and.neweqts.eq.2) icom=0
        IF(nrk.lt.3)THEN
          icom=1
          if(imoist.eq.1.and.neweqts.eq.2) icom=0
          if(icom.eq.0.and.thsmall.eq.0)then
            call comm_3s_end(th3d,sw31,sw32,se31,se32,   &
                                  ss31,ss32,sn31,sn32,reqs_s)
          endif
          if(imoist.eq.1)then
            DO n=1,numq
              icom=1
              if(neweqts.eq.2.and.(n.eq.nqv.or.n.eq.2.or.n.eq.4)) icom=0
              if(icom.eq.0)then
                call comm_3s_end(q3d(ib,jb,kb,n),                            &
                      qw1(1,1,1,n),qw2(1,1,1,n),qe1(1,1,1,n),qe2(1,1,1,n),   &
                      qs1(1,1,1,n),qs2(1,1,1,n),qn1(1,1,1,n),qn2(1,1,1,n),   &
                      reqs_q(1,n))
              endif
            ENDDO
          endif
        ENDIF
        icom=1
        if(imoist.eq.1.and.neweqts.eq.2) icom=0

#endif
!--------------------------------------------------------------------

        IF(axisymm.eq.1)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            v3d(i,j,k)=va(i,j,k)+dttmp*vten(i,j,k)
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_misc=time_misc+mytime()

          call bcv(v3d)

        ENDIF

!--------------------------------------------------------------------
!  call sound

        IF(psolver.eq.1)THEN

          call soundns(xh,rxh,uh,xf,uf,vh,vf,mh,mf,pi0,thv0,       &
                       radbcw,radbce,radbcs,radbcn,                &
                       divx,ua,u3d,uten,va,v3d,vten,wa,w3d,wten,   &
                       ppi,pp3d,sten,t11,t22,dttmp,nrk,            &
                       reqs_u,reqs_v,reqs_w,reqs_s,reqs_p,         &
                       uw31,uw32,ue31,ue32,us31,us32,un31,un32,    &
                       vw31,vw32,ve31,ve32,vs31,vs32,vn31,vn32,    &
                       ww31,ww32,we31,we32,ws31,ws32,wn31,wn32,    &
                       sw31,sw32,se31,se32,ss31,ss32,sn31,sn32,    &
                       pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2)

        ELSEIF(psolver.eq.2)THEN

          call sounde(xh,rxh,uh,ruh,xf,uf,vh,rvh,vf,mh,rmh,mf,rmf,   &
                     pi0,thv0,rho0,rr0,rf0,th0,dzdx,dzdy,            &
                     radbcw,radbce,radbcs,radbcn,                    &
                     dum1,dum2,dum3,dum4,divx,                       &
                     gx,ua,u3d,uten,gy,va,v3d,vten,wa,w3d,wten,      &
                     ppi,pp3d,sten,tha,th3d,thten,thterm,tk,         &
                     t11,t22,nrk,                                    &
                     reqs_u,reqs_v,reqs_w,reqs_s,reqs_p,             &
                     uw31,uw32,ue31,ue32,us31,us32,un31,un32,        &
                     vw31,vw32,ve31,ve32,vs31,vs32,vn31,vn32,        &
                     ww31,ww32,we31,we32,ws31,ws32,wn31,wn32,        &
                     sw31,sw32,se31,se32,ss31,ss32,sn31,sn32,        &
                     pw31,pw32,pe31,pe32,ps31,ps32,pn31,pn32,        &
                     pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2)

        ELSEIF(psolver.eq.3)THEN

          call sound(xh,rxh,uh,ruh,xf,uf,vh,rvh,vf,mh,rmh,mf,rmf,    &
                     pi0,thv0,rho0,rr0,rf0,th0,dzdx,dzdy,            &
                     radbcw,radbce,radbcs,radbcn,                    &
                     dum1,dum2,dum3,dum4,t12,t13,t23,t33,            &
                     gx,ua,u3d,uten,gy,va,v3d,vten,wa,w3d,wten,      &
                     ppi,pp3d,sten,tha,th3d,thten,thterm,tk,         &
                     t11,t22,nrk,                                    &
                     reqs_u,reqs_v,reqs_w,reqs_s,reqs_p,             &
                     uw31,uw32,ue31,ue32,us31,us32,un31,un32,        &
                     vw31,vw32,ve31,ve32,vs31,vs32,vn31,vn32,        &
                     ww31,ww32,we31,we32,ws31,ws32,wn31,wn32,        &
                     sw31,sw32,se31,se32,ss31,ss32,sn31,sn32,        &
                     pw31,pw32,pe31,pe32,ps31,ps32,pn31,pn32,        &
                     pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2)

        ELSEIF(psolver.eq.4.or.psolver.eq.5)THEN

          call anelp(uh,uf,vh,vf,mh,rmh,mf,rmf,pi0,thv0,rho0,prs0,rf0,   &
                     radbcw,radbce,radbcs,radbcn,dum1,divx,              &
                     ua,u3d,uten,va,v3d,vten,wa,w3d,wten,                &
                     ppi,pp3d,t11,cfb,cfa,cfc,d1,d2,pdt,deft,rhs,trans,dttmp,nrk)

        ENDIF

!--------------------------------------------------------------------
!  radbc

        if(irbc.eq.4)then

          if(ibw.eq.1 .or. ibe.eq.1)then
            call radbcew4(ruf,radbcw,radbce,ua,u3d,dttmp)
          endif

          if(ibs.eq.1 .or. ibn.eq.1)then
            call radbcns4(rvf,radbcs,radbcn,va,v3d,dttmp)
          endif

        endif

!--------------------------------------------------------------------
! RK loop end

      ENDDO


!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CC   End of RK section   CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
!CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


!--------------------------------------------------------------------
!  Get pressure

    IF(psolver.eq.4.or.psolver.eq.5)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        prs(i,j,k)=prs0(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

    ELSE

      call calcprs(pi0,prs,pp3d)

    ENDIF

!--------------------------------------------------------------------
!  Get density

    IF(psolver.eq.4.or.psolver.eq.5)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=1,nk
      do j=1,nj
      do i=1,ni
        rho(i,j,k)=rho0(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_misc=time_misc+mytime()

    ELSE

      call calcrho(pi0,th0,rho,prs,pp3d,th3d,q3d)

    ENDIF

!-----------------------------------------------------------------
!  Explicit microphysics

      IF(imoist.eq.1)THEN

        getdbz = .false.
        IF(output_dbz.eq.1)THEN
          rtime=float(nstep)*dtl
          if( (rtime.ge.sngl(taptim)).or.stopit )then
            getdbz = .true.
          endif
          if(getdbz)then
            write(outfile,*) '  Getting dbz ... '
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
            do k=1,nk
            do j=1,nj
            do i=1,ni
              sten(i,j,k)=0.0
            enddo
            enddo
            enddo
          endif
        ENDIF

!-----------------------------------------------------------------------
!  store t in dum1

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=(th0(i,j,k)+th3d(i,j,k))*(pi0(i,j,k)+pp3d(i,j,k))
        enddo
        enddo
        enddo

!-----------------------------------------------------------------------
!  prep for efall calculation:  store cvm in dum2

        IF(efall.eq.1)THEN
          if(neweqts.ge.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
            do k=1,nk
            do j=1,nj
            do i=1,ni
              dum2(i,j,k)=q3d(i,j,k,nqv)
            enddo
            enddo
            enddo
            call getqli(0,q3d,dum3,dum4)
          else
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
            do k=1,nk
            do j=1,nj
            do i=1,ni
              dum2(i,j,k)=0.0
              dum3(i,j,k)=0.0
              dum4(i,j,k)=0.0
            enddo
            enddo
            enddo
          endif
!$omp parallel do default(shared)  &
!$omp private(i,j,k,n,ql)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum2(i,j,k)=cv+cvv*dum2(i,j,k)+cpl*dum3(i,j,k)+cpi*dum4(i,j,k)
          enddo
          enddo
          enddo
        ENDIF

!-----------------------------------------------------------------------
!  store appropriate rho for budget calculations in dum3

        IF(axisymm.eq.0)THEN

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum3(i,j,k)=rho(i,j,k)
          enddo
          enddo
          enddo

        ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum3(i,j,k) = rho(i,j,k)*pi*(xf(i+1)**2-xf(i)**2)/(dx*dy)
          enddo
          enddo
          enddo

        ENDIF

!-----------------------------------------------------------------------
!  NOTES:
!           sten       is used for     dbz
!
!-----------------------------------------------------------------------
!  Kessler scheme
        IF(ptype.eq.1)THEN
          simple_comm = .false.
          call pdefq(    0.0,asq(1),ruh,rvh,rmh,rho,q3d(ib,jb,kb,1))
          call pdefq(1.0e-14,asq(2),ruh,rvh,rmh,rho,q3d(ib,jb,kb,2))
          call pdefq(1.0e-14,asq(3),ruh,rvh,rmh,rho,q3d(ib,jb,kb,3))
          call k_fallout(rho,q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
          call geterain(cpl,lv1,qbudget(7),ruh,rvh,dum1,dum3,q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
          if(efall.ge.1)then
            call getefall(cpl,ruh,rvh,mf,pi0,th0,dum1,dum2,dum3,   &
                          pp3d,th3d,q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
          endif
          call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                       q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
          call kessler(qbudget(3),qbudget(4),qbudget(5),ruh,rvh,rmh,pi0,th0,dum1,   &
                       rho,dum3,pp3d,th3d,prs,                            &
                       q3d(ib,jb,kb,nqv),q3d(ib,jb,kb,2),q3d(ib,jb,kb,3))
          call bcs(q3d(ib,jb,kb,3))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,3),                               &
                     qw1(1,1,1,3),qw2(1,1,1,3),qe1(1,1,1,3),qe2(1,1,1,3),   &
                     qs1(1,1,1,3),qs2(1,1,1,3),qn1(1,1,1,3),qn2(1,1,1,3),   &
                     reqs_q(1,3))
#endif
          call satadj(4,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,   &
                      rho,dum3,pp3d,prs,th3d,q3d)
          call bcs(q3d(ib,jb,kb,1))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,1),                               &
                     qw1(1,1,1,1),qw2(1,1,1,1),qe1(1,1,1,1),qe2(1,1,1,1),   &
                     qs1(1,1,1,1),qs2(1,1,1,1),qn1(1,1,1,1),qn2(1,1,1,1),   &
                     reqs_q(1,1))
#endif
          call bcs(q3d(ib,jb,kb,2))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,2),                               &
                     qw1(1,1,1,2),qw2(1,1,1,2),qe1(1,1,1,2),qe2(1,1,1,2),   &
                     qs1(1,1,1,2),qs2(1,1,1,2),qn1(1,1,1,2),qn2(1,1,1,2),   &
                     reqs_q(1,2))
#endif
          call bcs(th3d)
#ifdef MPI
          call comm_3s_start(th3d,sw31,sw32,se31,se32,   &
                                  ss31,ss32,sn31,sn32,reqs_s)
#endif
          call bcs(pp3d)
#ifdef MPI
          call comm_3s_start(pp3d,pw31,pw32,pe31,pe32,   &
                                  ps31,ps32,pn31,pn32,reqs_p)
#endif
!-----------------------------------------------------------------------
!  Goddard scheme
        ELSEIF(ptype.eq.2)THEN
          simple_comm = .false.
          call pdefq(    0.0,asq(1),ruh,rvh,rmh,rho,q3d(ib,jb,kb,1))
          call pdefq(1.0e-14,asq(2),ruh,rvh,rmh,rho,q3d(ib,jb,kb,2))
          call pdefq(1.0e-14,asq(3),ruh,rvh,rmh,rho,q3d(ib,jb,kb,3))
          call pdefq(1.0e-14,asq(4),ruh,rvh,rmh,rho,q3d(ib,jb,kb,4))
          call pdefq(1.0e-14,asq(5),ruh,rvh,rmh,rho,q3d(ib,jb,kb,5))
          call pdefq(1.0e-14,asq(6),ruh,rvh,rmh,rho,q3d(ib,jb,kb,6))
          call goddard(qbudget(3),qbudget(4),qbudget(5),ruh,rvh,rmh,pi0,th0,             &
                       rho,dum3,prs,pp3d,th3d,                            &
     q3d(ib,jb,kb,1), q3d(ib,jb,kb,2),q3d(ib,jb,kb,3),qten(ib,jb,kb,3),   &
     q3d(ib,jb,kb,4),qten(ib,jb,kb,4),q3d(ib,jb,kb,5),qten(ib,jb,kb,5),   &
     q3d(ib,jb,kb,6),qten(ib,jb,kb,6))
          call satadj_ice(4,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,     &
                          rho,dum3,pp3d,prs,th3d,                     &
              q3d(ib,jb,kb,1),q3d(ib,jb,kb,2),q3d(ib,jb,kb,3),   &
              q3d(ib,jb,kb,4),q3d(ib,jb,kb,5),q3d(ib,jb,kb,6))
          call bcs(q3d(ib,jb,kb,1))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,1),                               &
                     qw1(1,1,1,1),qw2(1,1,1,1),qe1(1,1,1,1),qe2(1,1,1,1),   &
                     qs1(1,1,1,1),qs2(1,1,1,1),qn1(1,1,1,1),qn2(1,1,1,1),   &
                     reqs_q(1,1))
#endif
          call bcs(q3d(ib,jb,kb,2))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,2),                               &
                     qw1(1,1,1,2),qw2(1,1,1,2),qe1(1,1,1,2),qe2(1,1,1,2),   &
                     qs1(1,1,1,2),qs2(1,1,1,2),qn1(1,1,1,2),qn2(1,1,1,2),   &
                     reqs_q(1,2))
#endif
          call geterain(cpl,lv1,qbudget(7),ruh,rvh,dum1,dum3,q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
          call geterain(cpi,ls1,qbudget(7),ruh,rvh,dum1,dum3,q3d(ib,jb,kb,4),qten(ib,jb,kb,4))
          call geterain(cpi,ls1,qbudget(7),ruh,rvh,dum1,dum3,q3d(ib,jb,kb,5),qten(ib,jb,kb,5))
          call geterain(cpi,ls1,qbudget(7),ruh,rvh,dum1,dum3,q3d(ib,jb,kb,6),qten(ib,jb,kb,6))
          if(efall.ge.1)then
            call getefall(cpl,ruh,rvh,mf,pi0,th0,dum1,dum2,dum3,   &
                          pp3d,th3d,q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
            call getefall(cpi,ruh,rvh,mf,pi0,th0,dum1,dum2,dum3,   &
                          pp3d,th3d,q3d(ib,jb,kb,4),qten(ib,jb,kb,4))
            call getefall(cpi,ruh,rvh,mf,pi0,th0,dum1,dum2,dum3,   &
                          pp3d,th3d,q3d(ib,jb,kb,5),qten(ib,jb,kb,5))
            call getefall(cpi,ruh,rvh,mf,pi0,th0,dum1,dum2,dum3,   &
                          pp3d,th3d,q3d(ib,jb,kb,6),qten(ib,jb,kb,6))
          endif
          call bcs(th3d)
#ifdef MPI
          call comm_3s_start(th3d,sw31,sw32,se31,se32,   &
                                  ss31,ss32,sn31,sn32,reqs_s)
#endif
          call bcs(pp3d)
#ifdef MPI
          call comm_3s_start(pp3d,pw31,pw32,pe31,pe32,   &
                                  ps31,ps32,pn31,pn32,reqs_p)
#endif
          call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                       q3d(ib,jb,kb,3),qten(ib,jb,kb,3))
          call bcs(q3d(ib,jb,kb,3))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,3),                               &
                     qw1(1,1,1,3),qw2(1,1,1,3),qe1(1,1,1,3),qe2(1,1,1,3),   &
                     qs1(1,1,1,3),qs2(1,1,1,3),qn1(1,1,1,3),qn2(1,1,1,3),   &
                     reqs_q(1,3))
#endif
          call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                       q3d(ib,jb,kb,4),qten(ib,jb,kb,4))
          call bcs(q3d(ib,jb,kb,4))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,4),                               &
                     qw1(1,1,1,4),qw2(1,1,1,4),qe1(1,1,1,4),qe2(1,1,1,4),   &
                     qs1(1,1,1,4),qs2(1,1,1,4),qn1(1,1,1,4),qn2(1,1,1,4),   &
                     reqs_q(1,4))
#endif
          call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                       q3d(ib,jb,kb,5),qten(ib,jb,kb,5))
          call bcs(q3d(ib,jb,kb,5))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,5),                               &
                     qw1(1,1,1,5),qw2(1,1,1,5),qe1(1,1,1,5),qe2(1,1,1,5),   &
                     qs1(1,1,1,5),qs2(1,1,1,5),qn1(1,1,1,5),qn2(1,1,1,5),   &
                     reqs_q(1,5))
#endif
          call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                       q3d(ib,jb,kb,6),qten(ib,jb,kb,6))
          call bcs(q3d(ib,jb,kb,6))
#ifdef MPI
          call comm_3s_start(q3d(ib,jb,kb,6),                               &
                     qw1(1,1,1,6),qw2(1,1,1,6),qe1(1,1,1,6),qe2(1,1,1,6),   &
                     qs1(1,1,1,6),qs2(1,1,1,6),qn1(1,1,1,6),qn2(1,1,1,6),   &
                     reqs_q(1,6))
#endif
          if(getdbz) call calcdbz(rho,q3d(ib,jb,kb,3),q3d(ib,jb,kb,5),q3d(ib,jb,kb,6),sten)
!-----------------------------------------------------------------------
!  Thompson scheme
        ELSEIF(ptype.eq.3)THEN
          simple_comm = .true.
          call pdefq(    0.0,asq(1),ruh,rvh,rmh,rho,q3d(ib,jb,kb,1))
          call pdefq(1.0e-12,asq(2),ruh,rvh,rmh,rho,q3d(ib,jb,kb,2))
          call pdefq(1.0e-12,asq(3),ruh,rvh,rmh,rho,q3d(ib,jb,kb,3))
          call pdefq(1.0e-12,asq(4),ruh,rvh,rmh,rho,q3d(ib,jb,kb,4))
          call pdefq(1.0e-12,asq(5),ruh,rvh,rmh,rho,q3d(ib,jb,kb,5))
          call pdefq(1.0e-12,asq(6),ruh,rvh,rmh,rho,q3d(ib,jb,kb,6))
!!!          call pdefq(    1.0,asq(7),ruh,rvh,rmh,rho,q3d(ib,jb,kb,7))
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum1(i,j,k)=pi0(i,j,k)+pp3d(i,j,k)
            dum2(i,j,k)=dz*rmh(i,j,k)
          enddo
          enddo
          enddo
        IF(neweqts.ge.1)THEN
          ! for mass conservation:
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            qten(i,j,k,1) = q3d(i,j,k,1)
            qten(i,j,k,2) = q3d(i,j,k,2)
            qten(i,j,k,3) = q3d(i,j,k,3)
            qten(i,j,k,4) = q3d(i,j,k,4)
            qten(i,j,k,5) = q3d(i,j,k,5)
            qten(i,j,k,6) = q3d(i,j,k,6)
          enddo
          enddo
          enddo
        ENDIF
          call mp_gt_driver(q3d(ib,jb,kb,1),q3d(ib,jb,kb,2),q3d(ib,jb,kb,3), &
                            q3d(ib,jb,kb,4),q3d(ib,jb,kb,5),q3d(ib,jb,kb,6), &
                            q3d(ib,jb,kb,7),q3d(ib,jb,kb,8),                 &
                            th0,th3d,dum1,prs,dum2,dtl,rain,                 &
                            qbudget(5),qbudget(6),                           &
                            ruh,rvh,rmh,dum3,sten,getdbz)
        IF(neweqts.ge.1)THEN
          ! for mass conservation:
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            if( abs(q3d(i,j,k,1)-qten(i,j,k,1)) + abs(q3d(i,j,k,2)-qten(i,j,k,2))   &
              + abs(q3d(i,j,k,3)-qten(i,j,k,3)) + abs(q3d(i,j,k,4)-qten(i,j,k,4))   &
              + abs(q3d(i,j,k,5)-qten(i,j,k,5)) + abs(q3d(i,j,k,6)-qten(i,j,k,6))   &
                .ge. 1.0e-8 )then
              pp3d(i,j,k)=((rho(i,j,k)*(rd+rv*q3d(i,j,k,nqv))   &
                                      *(th0(i,j,k)+th3d(i,j,k))*rp00)**rddcv)-pi0(i,j,k)
              prs(i,j,k)=p00*((pi0(i,j,k)+pp3d(i,j,k))**cpdrd)
            endif
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_microphy=time_microphy+mytime()
        ENDIF
          call satadj(4,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,   &
                      rho,dum3,pp3d,prs,th3d,q3d)

!-----------------------------------------------------------------------
!  LFOICE scheme

        ELSEIF(ptype.eq.4)THEN
          simple_comm = .true.
          call pdefq(    0.0,asq(1),ruh,rvh,rmh,rho,q3d(ib,jb,kb,1))
          call pdefq(1.0e-14,asq(2),ruh,rvh,rmh,rho,q3d(ib,jb,kb,2))
          call pdefq(1.0e-14,asq(3),ruh,rvh,rmh,rho,q3d(ib,jb,kb,3))
          call pdefq(1.0e-14,asq(4),ruh,rvh,rmh,rho,q3d(ib,jb,kb,4))
          call pdefq(1.0e-14,asq(5),ruh,rvh,rmh,rho,q3d(ib,jb,kb,5))
          call pdefq(1.0e-14,asq(6),ruh,rvh,rmh,rho,q3d(ib,jb,kb,6))
          call lfo_ice_drive(mf, pi0, prs0, pp3d, prs, th0, th3d,    &
                             qv0, rho0, q3d, qten, dum1)
          do n=2,numq
            call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                         q3d(ib,jb,kb,n),qten(ib,jb,kb,n))
          enddo

!-----------------------------------------------------------------------
!  Morrison scheme

        ELSEIF(ptype.eq.5)THEN
          simple_comm = .true.
          call pdefq(    0.0,asq(1),ruh,rvh,rmh,rho,q3d(ib,jb,kb,1))
          call pdefq(1.0e-12,asq(2),ruh,rvh,rmh,rho,q3d(ib,jb,kb,2))
          call pdefq(1.0e-12,asq(3),ruh,rvh,rmh,rho,q3d(ib,jb,kb,3))
          call pdefq(1.0e-12,asq(4),ruh,rvh,rmh,rho,q3d(ib,jb,kb,4))
          call pdefq(1.0e-12,asq(5),ruh,rvh,rmh,rho,q3d(ib,jb,kb,5))
          call pdefq(1.0e-12,asq(6),ruh,rvh,rmh,rho,q3d(ib,jb,kb,6))
!!!          call pdefq(    1.0,asq(7),ruh,rvh,rmh,rho,q3d(ib,jb,kb,7))
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ! dum1 = pi
            ! dum2 = dz
            ! dum4 = theta
            dum1(i,j,k)=pi0(i,j,k)+pp3d(i,j,k)
            dum2(i,j,k)=dz*rmh(i,j,k)
            dum4(i,j,k)=th0(i,j,k)+th3d(i,j,k)
            ! store old theta in thten array:
            thten(i,j,k)=dum4(i,j,k)
          enddo
          enddo
          enddo
        IF(neweqts.ge.1)THEN
          ! for mass conservation:
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            qten(i,j,k,1) = q3d(i,j,k,1)
            qten(i,j,k,2) = q3d(i,j,k,2)
            qten(i,j,k,3) = q3d(i,j,k,3)
            qten(i,j,k,4) = q3d(i,j,k,4)
            qten(i,j,k,5) = q3d(i,j,k,5)
            qten(i,j,k,6) = q3d(i,j,k,6)
          enddo
          enddo
          enddo
        ENDIF
          call mp_graupel(nstep,dum4,                                         &
                          q3d(ib,jb,kb, 1),q3d(ib,jb,kb, 2),q3d(ib,jb,kb, 3), &
                          q3d(ib,jb,kb, 4),q3d(ib,jb,kb, 5),q3d(ib,jb,kb, 6), &
                          q3d(ib,jb,kb, 7),q3d(ib,jb,kb, 8),q3d(ib,jb,kb, 9), &
                          q3d(ib,jb,kb,10),q3d(ib,jb,kb,11),                  &
                          dum1,prs,dtl,dum2,w3d,rain,                         &
                          qbudget(1),qbudget(2),qbudget(5),qbudget(6),        &
                          ruh,rvh,rmh,dum3,sten,getdbz)
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ! add any changes to theta:
            th3d(i,j,k)=th3d(i,j,k)+(dum4(i,j,k)-thten(i,j,k))
          enddo
          enddo
          enddo
        IF(neweqts.ge.1)THEN
          ! for mass conservation:
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            if( abs(q3d(i,j,k,1)-qten(i,j,k,1)) + abs(q3d(i,j,k,2)-qten(i,j,k,2))   &
              + abs(q3d(i,j,k,3)-qten(i,j,k,3)) + abs(q3d(i,j,k,4)-qten(i,j,k,4))   &
              + abs(q3d(i,j,k,5)-qten(i,j,k,5)) + abs(q3d(i,j,k,6)-qten(i,j,k,6))   &
                .ge. 1.0e-8 )then
              pp3d(i,j,k)=((rho(i,j,k)*(rd+rv*q3d(i,j,k,nqv))   &
                                      *(th0(i,j,k)+th3d(i,j,k))*rp00)**rddcv)-pi0(i,j,k)
              prs(i,j,k)=p00*((pi0(i,j,k)+pp3d(i,j,k))**cpdrd)
            endif
          enddo
          enddo
          enddo
          if(timestats.ge.1) time_microphy=time_microphy+mytime()
        ENDIF
          call satadj(4,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,   &
                      rho,dum3,pp3d,prs,th3d,q3d)

!-----------------------------------------------------------------------
!  RE87 approach

        ELSEIF(ptype.eq.6)THEN
          simple_comm = .true.
          call pdefq(    0.0,asq(1),ruh,rvh,rmh,rho,q3d(ib,jb,kb,1))
          call pdefq(1.0e-14,asq(2),ruh,rvh,rmh,rho,q3d(ib,jb,kb,2))
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            if(q3d(i,j,k,2).gt.0.001)then
              qten(i,j,k,2) = v_t
            else
              qten(i,j,k,2) = 0.0
            endif
          enddo
          enddo
          enddo
          call geterain(cpl,lv1,qbudget(7),ruh,rvh,dum1,dum3,q3d(ib,jb,kb,2),qten(ib,jb,kb,2))
          if(efall.ge.1)then
            call getefall(cpl,ruh,rvh,mf,pi0,th0,dum1,dum2,dum3,   &
                          pp3d,th3d,q3d(ib,jb,kb,2),qten(ib,jb,kb,2))
          endif
          call fallout(qbudget(6),ruh,rvh,zh,mh,mf,rain,dum3,rho,   &
                       q3d(ib,jb,kb,2),qten(ib,jb,kb,2))
          call satadj(4,qbudget(1),qbudget(2),ruh,rvh,rmh,pi0,th0,   &
                      rho,dum3,pp3d,prs,th3d,q3d)

!-----------------------------------------------------------------------
!  Milbrandt & Yao scheme
!
!        ELSEIF(ptype.eq.7)THEN
!
!-----------------------------------------------------------------------
!  insert new microphysics schemes here
!-----------------------------------------------------------------------
!        ELSEIF(ptype.eq.8)THEN
!-----------------------------------------------------------------------
! otherwise, stop for undefined ptype
        ELSE
          print *,'  Undefined ptype!'
          call stopcm1
        ENDIF

!-----------------------------------------------------------------------
!Begin:  message passing for simple_comm
      IF(simple_comm)THEN
          call bcs(th3d)
          call bcs(pp3d)
          DO n=1,numq
            call bcs(q3d(ib,jb,kb,n))
          ENDDO
#ifdef MPI
          call comm_3s_start(th3d,sw31,sw32,se31,se32,   &
                                  ss31,ss32,sn31,sn32,reqs_s)
          call comm_3s_start(pp3d,pw31,pw32,pe31,pe32,   &
                                  ps31,ps32,pn31,pn32,reqs_p)
          DO n=1,numq
            call comm_3s_start(q3d(ib,jb,kb,n),                               &
                       qw1(1,1,1,n),qw2(1,1,1,n),qe1(1,1,1,n),qe2(1,1,1,n),   &
                       qs1(1,1,1,n),qs2(1,1,1,n),qn1(1,1,1,n),qn2(1,1,1,n),   &
                       reqs_q(1,n))
          ENDDO
#endif
      ENDIF
!Done:  message passing for simple_comm
!-----------------------------------------------------------------------

      ENDIF

!-----------------------------------------------------------------
!  Equate the two arrays

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie+1
        ua(i,j,k)=u3d(i,j,k)
      enddo
      enddo
      enddo
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je+1
      do i=ib,ie
        va(i,j,k)=v3d(i,j,k)
      enddo
      enddo
      enddo
 
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke+1
      do j=jb,je
      do i=ib,ie
        wa(i,j,k)=w3d(i,j,k)
      enddo
      enddo
      enddo

      if(iturb.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=kbt,ket
        do j=jbt,jet
        do i=ibt,iet
          tkea(i,j,k)=tke3d(i,j,k)
        enddo
        enddo
        enddo
      endif

      if(iptra.eq.1)then
        do n=1,npt
          call pdefq(0.0,afoo,ruh,rvh,rmh,rho,pt3d(ib,jb,kb,n))
          call bcs(pt3d(ib,jb,kb,n))
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=kb,ke
          do j=jb,je
          do i=ib,ie
            pta(i,j,k,n)=pt3d(i,j,k,n)
          enddo
          enddo
          enddo
        enddo
      endif
      if(timestats.ge.1) time_integ=time_integ+mytime()
 
#ifndef MPI
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie
        ppi(i,j,k)=pp3d(i,j,k)
      enddo
      enddo
      enddo

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie
        tha(i,j,k)=th3d(i,j,k)
      enddo
      enddo
      enddo

      if(imoist.eq.1)then
 
        do n=1,numq

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=kb,ke
          do j=jb,je
          do i=ib,ie
            qa(i,j,k,n)=q3d(i,j,k,n)
          enddo
          enddo
          enddo

        enddo
 
      endif

      if(timestats.ge.1) time_integ=time_integ+mytime()
#endif

!---- finish communication of moisture ----

#ifdef MPI
  IF(imoist.eq.1)THEN
    IF(simple_comm)THEN
      call fincomm(tha,th3d,sw31,sw32,se31,se32,   &
                            ss31,ss32,sn31,sn32,reqs_s)
      call fincomm(ppi,pp3d,pw31,pw32,pe31,pe32,   &
                            ps31,ps32,pn31,pn32,reqs_p)
      do n=1,numq
        call fincomm(qa(ib,jb,kb,n),q3d(ib,jb,kb,n),                        &
                     qw1(1,1,1,n),qw2(1,1,1,n),qe1(1,1,1,n),qe2(1,1,1,n),   &
                     qs1(1,1,1,n),qs2(1,1,1,n),qn1(1,1,1,n),qn2(1,1,1,n),   &
                     reqs_q(1,n))
      enddo
    ELSE
      if(ptype.eq.1)then
        call fincomm(qa(ib,jb,kb,3),q3d(ib,jb,kb,3),                        &
                     qw1(1,1,1,3),qw2(1,1,1,3),qe1(1,1,1,3),qe2(1,1,1,3),   &
                     qs1(1,1,1,3),qs2(1,1,1,3),qn1(1,1,1,3),qn2(1,1,1,3),   &
                     reqs_q(1,3))
        call fincomm(qa(ib,jb,kb,1),q3d(ib,jb,kb,1),                        &
                     qw1(1,1,1,1),qw2(1,1,1,1),qe1(1,1,1,1),qe2(1,1,1,1),   &
                     qs1(1,1,1,1),qs2(1,1,1,1),qn1(1,1,1,1),qn2(1,1,1,1),   &
                     reqs_q(1,1))
        call fincomm(qa(ib,jb,kb,2),q3d(ib,jb,kb,2),                        &
                     qw1(1,1,1,2),qw2(1,1,1,2),qe1(1,1,1,2),qe2(1,1,1,2),   &
                     qs1(1,1,1,2),qs2(1,1,1,2),qn1(1,1,1,2),qn2(1,1,1,2),   &
                     reqs_q(1,2))
        call fincomm(tha,th3d,sw31,sw32,se31,se32,   &
                              ss31,ss32,sn31,sn32,reqs_s)
        call fincomm(ppi,pp3d,pw31,pw32,pe31,pe32,   &
                              ps31,ps32,pn31,pn32,reqs_p)
      elseif(ptype.eq.2)then
        call fincomm(qa(ib,jb,kb,1),q3d(ib,jb,kb,1),                        &
                     qw1(1,1,1,1),qw2(1,1,1,1),qe1(1,1,1,1),qe2(1,1,1,1),   &
                     qs1(1,1,1,1),qs2(1,1,1,1),qn1(1,1,1,1),qn2(1,1,1,1),   &
                     reqs_q(1,1))
        call fincomm(qa(ib,jb,kb,2),q3d(ib,jb,kb,2),                        &
                     qw1(1,1,1,2),qw2(1,1,1,2),qe1(1,1,1,2),qe2(1,1,1,2),   &
                     qs1(1,1,1,2),qs2(1,1,1,2),qn1(1,1,1,2),qn2(1,1,1,2),   &
                     reqs_q(1,2))
        call fincomm(tha,th3d,sw31,sw32,se31,se32,   &
                              ss31,ss32,sn31,sn32,reqs_s)
        call fincomm(ppi,pp3d,pw31,pw32,pe31,pe32,   &
                              ps31,ps32,pn31,pn32,reqs_p)
        call fincomm(qa(ib,jb,kb,3),q3d(ib,jb,kb,3),                        &
                     qw1(1,1,1,3),qw2(1,1,1,3),qe1(1,1,1,3),qe2(1,1,1,3),   &
                     qs1(1,1,1,3),qs2(1,1,1,3),qn1(1,1,1,3),qn2(1,1,1,3),   &
                     reqs_q(1,3))
        call fincomm(qa(ib,jb,kb,4),q3d(ib,jb,kb,4),                        &
                     qw1(1,1,1,4),qw2(1,1,1,4),qe1(1,1,1,4),qe2(1,1,1,4),   &
                     qs1(1,1,1,4),qs2(1,1,1,4),qn1(1,1,1,4),qn2(1,1,1,4),   &
                     reqs_q(1,4))
        call fincomm(qa(ib,jb,kb,5),q3d(ib,jb,kb,5),                        &
                     qw1(1,1,1,5),qw2(1,1,1,5),qe1(1,1,1,5),qe2(1,1,1,5),   &
                     qs1(1,1,1,5),qs2(1,1,1,5),qn1(1,1,1,5),qn2(1,1,1,5),   &
                     reqs_q(1,5))
        call fincomm(qa(ib,jb,kb,6),q3d(ib,jb,kb,6),                        &
                     qw1(1,1,1,6),qw2(1,1,1,6),qe1(1,1,1,6),qe2(1,1,1,6),   &
                     qs1(1,1,1,6),qs2(1,1,1,6),qn1(1,1,1,6),qn2(1,1,1,6),   &
                     reqs_q(1,6))
      endif
    ENDIF

  ELSE

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie
        ppi(i,j,k)=pp3d(i,j,k)
      enddo
      enddo
      enddo

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
      do k=kb,ke
      do j=jb,je
      do i=ib,ie
        tha(i,j,k)=th3d(i,j,k)
      enddo
      enddo
      enddo
      if(timestats.ge.1) time_integ=time_integ+mytime()

  ENDIF
#endif

      if(imove.eq.1.and.imoist.eq.1)then
        call movesfc(uh,vh,rain(ib,jb,2),dum1(ib,jb,1),dum1(ib,jb,2),dum1(ib,jb,3))
      endif

!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!cc   All done   cccccccccccccccccccccccccccccccccccccccccccccccccccc
!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

#ifdef MPI
      call MPI_BARRIER (comm,ierr)
      if(timestats.ge.1) time_mpb=time_mpb+mytime()
#endif

!--------------------------------------------------------------------
!  Get statistics

!$omp parallel do default(shared)  &
!$omp private(i,j,n,tem)
      do j=1,nj
      do i=1,ni
        tem = sqrt( (umove+0.5*(ua(i,j,1)+ua(i+1,j,1)))**2    &
                   +(vmove+0.5*(va(i,j,1)+va(i,j+1,1)))**2 ) 
        do n=1,nrain
          sws(i,j,n)=max(sws(i,j,n),tem)
        enddo
      enddo
      enddo

      if(imove.eq.1)then
        call movesfc(uh,vh,sws(ib,jb,2),dum1(ib,jb,1),dum1(ib,jb,2),dum1(ib,jb,3))
      endif

      if(mod(nstep,statfrq).eq.0)then
        IF(axisymm.eq.0)THEN
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ppten(i,j,k)=rho(i,j,k)
          enddo
          enddo
          enddo
        ELSE
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ppten(i,j,k) = rho(i,j,k)*pi*(xf(i+1)**2-xf(i)**2)/(dx*dy)
          enddo
          enddo
          enddo
        ENDIF
        rtime=float(nstep)*dtl
        call statpack(nrec,rtime,cloudvar,qname,budname,qbudget,asq,bsq, &
                      xh,rxh,uh,ruh,xf,uf,yh,vh,rvh,vf,zh,mh,rmh,mf,    &
                      rstat,pi0,rho0,thv0,th0,qv0,u0,v0,                &
                      dum1,dum2,dum3,dum4,divx,ppten,prs,               &
                      ua,va,wa,ppi,tha,qa,qten,kmh,kmv,khh,khv,tkea,pta)
      endif

!--------------------------------------------------------------------
!  Writeout and stuff

      rtime=float(nstep)*dtl
      if(myid.eq.0)then
        if(timeformat.eq.1)then
          write(6,110) nstep,rtime,' sec '
        elseif(timeformat.eq.2)then
          write(6,110) nstep,rtime/60.0,' min '
        elseif(timeformat.eq.3)then
          write(6,110) nstep,rtime/3600.0,' hour'
        elseif(timeformat.eq.4)then
          write(6,110) nstep,rtime/86400.0,' day '
        else
          write(6,110) nstep,rtime,' sec'
        endif
110     format(2x,i12,4x,f18.6,a5)
      endif
      if(timestats.ge.1) time_misc=time_misc+mytime()

      if( (rtime.ge.sngl(taptim)).or.stopit )then
        nwrite=nwrite+1
      IF(output_format.eq.1)THEN
        call writeout(51,nwrite,qname,sigma,zh,pi0,prs0,rho0,th0,qv0,u0,v0,  &
                      zs,rain,sws,thflux,qvflux,cdu,cdv,ce,dum1,dum2,        &
                      rho,prs,sten,ua,uten,va,vten,wa,wten,ppi,tha,          &
                      qa,kmh,kmv,khh,khv,tkea,pta)
        if(terrain_flag .and. output_interp.eq.1)then
          call writeout(71,nwrite,qname,sigma,zh,pi0,prs0,rho0,th0,qv0,u0,v0,  &
                        zs,rain,sws,thflux,qvflux,cdu,cdv,ce,dum1,dum2,        &
                        rho,prs,sten,ua,uten,va,vten,wa,wten,ppi,tha,          &
                        qa,kmh,kmv,khh,khv,tkea,pta)
        endif
#ifdef NETCDF
      ELSEIF(output_format.eq.2)THEN
        call writeout_cdf(nwrite,qname,sigma,sigmaf,xh,xf,yh,yf,zh,zf, &
                      pi0,prs0,rho0,th0,qv0,u0,v0,                     &
                      zs,rain,sws,thflux,qvflux,cdu,cdv,ce,dum1,dum2,  &
                      rho,prs,sten,ua,uten,va,vten,wa,wten,ppi,tha,    &
                      qa,kmh,kmv,khh,khv,tkea,pta)
#endif
#ifdef HDFOUT

        else if(output_format.eq.3) then
        call writeout_mult_hdf(rtime,.true.,qname,sigma,xf,xh,yf,yh,zf,zh,pi0,rho0,th0,qv0,u0,v0,  &
                      zs,rain,thflux,qvflux,cdu,cdv,ce,sws,dum1,dum2,   &
                      rho,prs,sten,ua,uten,va,vten,wa,wten,ppi,tha,     &
                      qa,kmh,kmv,khh,khv,tkea,pta)
        else if(output_format.eq.4) then
        call writeout_mult_hdf(rtime,.false.,qname,sigma,xf,xh,yf,yh,zf,zh,pi0,rho0,th0,qv0,u0,v0,  &
                      zs,rain,thflux,qvflux,cdu,cdv,ce,sws,dum1,dum2,   &
                      rho,prs,sten,ua,uten,va,vten,wa,wten,ppi,tha,     &
                      qa,kmh,kmv,khh,khv,tkea,pta)
#endif
#ifdef SBSOUT
        else if(output_format.eq.5) then
        call writeout_sbs(nwrite,.false.,qname,sigma,zh,pi0,prs0,rho0,th0,qv0,u0,v0, &
                      zs,rain,thflux,qvflux,cdu,cdv,ce,sws,dum1,dum2,   &
                      rho,prs,sten,ua,uten,va,vten,wa,wten,ppi,tha,     &
                      qa,kmh,kmv,khh,khv,tkea,pta)

#endif
      ENDIF
        taptim=taptim+tapfrq
        if(timestats.ge.1) time_write=time_write+mytime()
      endif

      if(rtime.ge.rsttim .and. rstfrq.gt.0)then
        nrst=nrst+1
        call write_restart(nrec,prec,nwrite,nrst,                    &
                           qbudget,asq,bsq,                          &
                           rain,sws,radbcw,radbce,radbcs,radbcn,     &
                           ua,va,wa,ppi,tha,qa,tkea,pta,pdata,rtime)
        rsttim=rsttim+rstfrq
        if(timestats.ge.1) time_write=time_write+mytime()
      endif

!-------------------------------------------------------------------
!  Parcel update (final time step)

      if( (iprcl.eq.1).and.(nstep.eq.nloop2) )then
        write(outfile,*) '  Calling parcel driver for last step'
        call parcel_driver(prec,xh,uh,ruh,yh,vh,rvh,zh,mh,rmh,mf,        &
                           pi0,thv0,th0,dum1,dum2,dum3,dum4,divx,prs,    &
                           ua,va,wa,ppi,thten,tha,qa,khv,pdata,rtime,    &
                           ploc,packet,reqs_p,                           &
                           pw1,pw2,pe1,pe2,ps1,ps2,pn1,pn2)
      endif

!-------------------------------------------------------------------

      if(stopit)then
        if(myid.eq.0)then
          print *
          print *,' Courant number has exceeded 1.5 '
          print *
          print *,' Stopping model .... '
          print *
        endif
        call stopcm1
      endif

      return
      end


