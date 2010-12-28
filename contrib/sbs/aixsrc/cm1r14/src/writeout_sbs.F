!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

#ifdef SBSOUT
      subroutine writeout_sbs(nwrite,terrain,qname,sigma,zh,pi0,prs0,rho0,th0,qv0,u0,v0,  &
                          zs,rain,sws,thflux,qvflux,cdu,cdv,ce,dum1,dum2,     &
                          rho,prs,dbz,ua,dumu,va,dumv,wa,dumw,ppi,tha,        &
                          qa,kmh,kmv,khh,khv,tkea,pta)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'mpif.h'

      logical :: terrain
      integer :: nwrite
      character*3, dimension(maxq) :: qname
      real, dimension(kb:ke) :: sigma
      real, dimension(ib:ie,jb:je,kb:ke) :: zh,pi0,prs0,rho0,th0,qv0
      real, dimension(itb:ite,jtb:jte) :: zs
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
      real, dimension(ib:ie,jb:je) :: thflux,qvflux,cdu,cdv,ce
      real, dimension(ib:ie,jb:je,kb:ke) :: dum1,dum2,rho,prs,dbz
      real, dimension(ib:ie+1,jb:je,kb:ke) :: u0,ua,dumu
      real, dimension(ib:ie,jb:je+1,kb:ke) :: v0,va,dumv
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa,dumw
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa
      real, dimension(ibc:iec,jbc:jec,kbc:kec) :: kmh,kmv,khh,khv
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta

      integer i,j,k,n,irec
      
      integer :: nip1, njp1, nkp1
      
      integer      :: sbs_err, mpi_err
      integer*8    :: sbs_handler
      integer      :: comm

      character*32 :: filename
      call get_registered_comm(comm)
      call get_registered_sbs_handler_f(sbs_handler)

      call sbs_start_step_f(sbs_handler,nwrite-1)
   
	  nip1 = ni + 1
	  njp1 = nj + 1
	  nkp1 = nk + 1
	  
58 format(i4.4)
      
!--------------------------------------------------------------
!  writeout data on scalar-points

      if(output_rain.eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"rain1",rain(ib,jb,1))
      if(output_sws .eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"sws1",sws(ib,jb,1))
      if(nrain.eq.2)then
        if(output_rain.eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"rain2",rain(ib,jb,2))
        if(output_sws .eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"sws2",sws(ib,jb,2))
      endif
      if(output_sfcflx.eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"thflux",thflux)
      if(output_sfcflx.eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"qvflux",qvflux)
      if(output_sfcflx.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j)
        do j=1,nj
        do i=1,ni
          dum1(i,j,1)=0.25*( (cdu(i,j)+cdu(i+1,j))   &
                            +(cdv(i,j)+cdv(i,j+1)) )
        enddo
        enddo
        call write2d_sbs(sbs_handler,ni,nj,irec,"cd",dum1(ib,jb,1))
      endif
      if(output_sfcflx.eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"ce",ce)
      if(output_zs  .eq.1) call write2d_sbs(sbs_handler,ni,nj,irec,"zs",zs)
      dum1=zh
      if(terrain)then
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=(k*dz-0.5*dz)-zs(i,j)
        enddo
        enddo
        enddo
      endif
      if(output_zh  .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"zh",dum1)
      if(output_th  .eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=th0(i,j,k)+tha(i,j,k)
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"th",dum1)
      endif
      dum1=tha
      if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_thpert .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"thpert",dum1)
      dum1=prs
      if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_prs    .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"prs",dum1)
      if(output_prspert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=prs(i,j,k)-p00*(pi0(i,j,k)**cpdrd)
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"prspert",dum1)
      endif
      if(output_pi.eq.1)then  
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=pi0(i,j,k)+ppi(i,j,k)
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"pi",dum1)
      endif
      dum1=ppi
      if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_pipert .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"pipert",dum1)
      dum1=rho
      if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_rho    .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"rho",dum1)
      if(output_rhopert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=rho(i,j,k)-rho0(i,j,k)
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"rhopert",dum1)
      endif
      !! WARNING : some stuff to do for SBS here
      if(iptra.eq.1)then
        do n=1,npt
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum1(i,j,k)=pta(i,j,k,n)
          enddo
          enddo
          enddo
          if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
          call write3d_sbs(sbs_handler,ni,nj,nk,irec,"pta",dum1)
        enddo
      endif
      !! END WARNING
      if(imoist.eq.1)then
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=qa(i,j,k,nqv)
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        if(output_qv    .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"qv",dum1)
        if(output_qvpert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            dum1(i,j,k)=qa(i,j,k,nqv)-qv0(i,j,k)
          enddo
          enddo
          enddo
          if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
          call write3d_sbs(sbs_handler,ni,nj,nk,irec,"qvpert",dum1)
        endif
        ! WARNING : some stuff to add for SBS here
        if(output_q.eq.1)then
          do n=1,numq
            if(n.ne.nqv)then
              do k=1,nk
              do j=1,nj
              do i=1,ni
                dum1(i,j,k)=qa(i,j,k,n)
              enddo
              enddo
              enddo
              if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
              call write3d_sbs(sbs_handler,ni,nj,nk,irec,"q",dum1)
            endif
          enddo
        endif
        ! END WARNING
        dum1=dbz
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        if(output_dbz   .eq.1) call write3d_sbs(sbs_handler,ni,nj,nk,irec,"dbz",dum1)
      endif
      if(output_uinterp.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=0.5*(ua(i,j,k)+ua(i+1,j,k))
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"uinterp",dum1)
      endif
      if(output_vinterp.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=0.5*(va(i,j,k)+va(i,j+1,k))
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"vinterp",dum1)
      endif
      if(output_winterp.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=0.5*(wa(i,j,k)+wa(i,j,k+1))
        enddo
        enddo
        enddo
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"winterp",dum1)
      endif

      if(output_basestate.eq.1)then
        dum1=pi0
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"pi0",dum1)
        dum1=th0
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"th0",dum1)
        dum1=prs0
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"prs0",dum1)
        dum1=qv0
        if(terrain) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d_sbs(sbs_handler,ni,nj,nk,irec,"qv0",dum1)
      endif

!--------------------------------------------------------------
!  writeout data on u-points

      irec=1+(nwrite-1)*nk*u_out
      if(output_filetype.eq.2) irec=1
#ifdef MPI
      irec=1
#endif

      if(output_u    .eq.1) call write3d_sbs(sbs_handler,ni+1,nj,nk,irec,"u",ua)

      if(output_upert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni+1
          dumu(i,j,k)=ua(i,j,k)-u0(i,j,k)
        enddo
        enddo
        enddo
        call write3d_sbs(sbs_handler,ni+1,nj,nk,irec,"upert",dumu)
      endif

      if(output_basestate.eq.1) call write3d_sbs(sbs_handler,ni+1,nj,nk,irec,"u0",u0)

!--------------------------------------------------------------
!  writeout data on v-points

      irec=1+(nwrite-1)*nk*v_out
      if(output_filetype.eq.2) irec=1
#ifdef MPI
      irec=1
#endif

      if(output_v    .eq.1) call write3d_sbs(sbs_handler,ni,nj+1,nk,irec,"v",va)

      if(output_vpert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj+1
        do i=1,ni
          dumv(i,j,k)=va(i,j,k)-v0(i,j,k)
        enddo
        enddo
        enddo
        call write3d_sbs(sbs_handler,ni,nj+1,nk,irec,"vpert",dumv)
      endif

      if(output_basestate.eq.1) call write3d_sbs(sbs_handler,ni,nj+1,nk,irec,"v0",v0)

!--------------------------------------------------------------
!  writeout data on w-points

      irec=1+(nwrite-1)*(nk+1)*w_out
      if(output_filetype.eq.2) irec=1
      irec=1

      if(output_w  .eq.1)                call write3d_sbs(sbs_handler,ni,nj,nk+1,irec,"w",wa)
      if(output_tke.eq.1.and.iturb.eq.1) call write3d_sbs(sbs_handler,ni,nj,nk+1,irec,"tke",tkea)
      if(output_km .eq.1.and.iturb.ge.1) call write3d_sbs(sbs_handler,ni,nj,nk+1,irec,"kmh",kmh)
      if(output_km .eq.1.and.iturb.ge.1) call write3d_sbs(sbs_handler,ni,nj,nk+1,irec,"kmv",kmv)
      if(output_kh .eq.1.and.iturb.ge.1) call write3d_sbs(sbs_handler,ni,nj,nk+1,irec,"khh",khh)
      if(output_kh .eq.1.and.iturb.ge.1) call write3d_sbs(sbs_handler,ni,nj,nk+1,irec,"khv",khv)

!--------------------------------------------------------------
	
      call sbs_end_step_f(sbs_handler, nwrite-1)
	  
      write(outfile,*)
      write(outfile,*) 'Done Writing Data to File: nwrite=',nwrite
      write(outfile,*)

#ifdef MPI
      call MPI_BARRIER (comm,ierr)
#endif
      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine write2d_sbs(sbs_handler,numi,numj,irec,varname,var)
      implicit none

      integer :: numi,numj,irec
      character(len=*) :: varname
      real, dimension(-2:numi+3,-2:numj+3) :: var
      real, dimension(numi,numj) :: lvar

      integer i,j
      
      integer*8    :: sbs_handler
      integer      :: sbs_err
      integer*8, dimension(2) :: lsizes, offsets
      integer client_id

      lsizes(1) = numi
      lsizes(2) = numj

      call get_client_id(client_id)

      offsets(1) = numi*(mod(client_id,5))
      offsets(2) = numj*(client_id/5)

      call get_registered_sbs_handler_f(sbs_handler)
        do j=1,numj
        do i=1,numi
          lvar(i,j) = var(i,j)
        enddo
        enddo
	  
	 ! print *, "in write2d_sbs, writing ",varname
         ! print *, "   numi,numj: ",numi," ",numj
      call sbs_write_f(sbs_handler,varname,lvar,4,2,offsets,lsizes,sbs_err)

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine write3d_sbs(sbs_handler,numi,numj,numk,irec,varname,var)
      implicit none

      integer :: numi,numj,numk,irec
      character(len=*) :: varname
      real, dimension(-2:numi+3,-2:numj+3,0:numk+1) :: var
      real, dimension(numi,numj,numk) :: lvar

      integer i,j,k
      
      integer*8    :: sbs_handler
      integer      :: sbs_err
      integer*8, dimension(3) :: offsets, lsizes
      integer client_id

      lsizes(1) = numi
      lsizes(2) = numj
      lsizes(3) = numk

      call get_client_id(client_id)

      offsets(1) = numi*(mod(client_id,5))
      offsets(2) = numj*(client_id/5)
      offsets(3) = 0
      
      call get_registered_sbs_handler_f(sbs_handler)

      	do k=1,numk
        do j=1,numj
        do i=1,numi
          lvar(i,j,k) = var(i,j,k)
        enddo
        enddo
        enddo
      
      !print *, "in write3d_sbs, writing ",varname
      !print *, "   numi,numj,numk: ",numi," ",numj," ",numk
      call sbs_write_f(sbs_handler,varname,lvar,4,3,offsets,lsizes,sbs_err)

      return
      end
      
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	
#endif
