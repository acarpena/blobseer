
! Leigh Orf <leigh.orf@cmich.edu>
! Original writeout routine modified to accomodate HDF format.
! Two output formats are supported:
! 1. Scaled short (16 bit) integers, gzip compressed (recommended)
! 2. 32 bit floating point data, gzip compressed
! The scaled shorts will save you lots of disk space and are easy to convert
! back into floats. However precision is lost. Should be fine for visualization.
!
! Routines for reading and converting this format are found at
! http://hdftools.sourceforge.net

      subroutine writeout_mult_hdf(model_time,lossy_compress,qname,sigma, &
                          xf,xh,yf,yh,zf,zh,pi0,rho0,th0,qv0,u0,v0,  &
                          zs,rain,thflux,qvflux,cdu,cdv,ce,sws,dum1,dum2,   &
                          rho,prs,dbz,ua,dumu,va,dumv,wa,dumw,ppi,tha,      &
                          qa,kmh,kmv,khh,khv,tkea,pta)
      implicit none

      include 'input.incl'
      include 'constants.incl'
#ifdef HDFOUT
      include 'hdf.f90'
!!!      include 'mffunc.f90'
      include 'dffunc.f90'
#endif
#ifdef MPI
      include 'mpif.h'
      integer comm
#endif

      integer :: nwrite
      real :: model_time
      character*3, dimension(maxq) :: qname
      character*50, dimension(maxq) :: qnamedesc
      real, dimension(kb:ke) :: sigma
! ORF all georeferenced data is stored
! Assumes no terrain (see zfcol)
      real, dimension(ib:ie)   :: xh
      real, dimension(ib:ie+1) :: xf
      real, dimension(jb:je)   :: yh
      real, dimension(jb:je+1) :: yf
      real, dimension(ib:ie,jb:je,kb:ke) :: zh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: zf
      real, dimension(ni,nj,nk) :: ds
      real, dimension(ni,nj) :: ds2
      real, dimension(ni+1,nj,nk) :: du
      real, dimension(ni,nj+1,nk) :: dv
      real, dimension(ni,nj,nk+1) :: dw
      real, dimension(ib:ie,jb:je,kb:ke) :: pi0,rho0,th0,qv0
      real, dimension(1:nk) :: zhcol,zfcol,u0col,v0col,th0col,pi0col,rho0col,pres0col
      real, dimension(1:ni) :: xfcol,xhcol
      real, dimension(1:nj) :: yfcol,yhcol
      real, dimension(itb:ite,jtb:jte) :: zs
      real, dimension(ib:ie,jb:je) :: thflux,qvflux,cdu,cdv,ce
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
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
      character*50  description

      character*80 filename
      character(LEN=20) :: qvarname

! ORF HDF stuff
! ORF begin hdf variables. Some bits shamelessly ripped off from Lou Wicker's ncommas
      logical lossy_compress
      integer sd_id, sds_id, status, dim_id, data_type
      logical, parameter    :: USE_CHUNKS   = .false.  ! chunking is good but since we're writing multiple files ...
      character(LEN=19), parameter :: COMPRESSION_ATTR      = 'COMPRESSION FORMULA'
      character(LEN=31), parameter :: COMPRESSION_FORMULA   = 'INTEGER16 = CAL*[REAL32-OFFSET]'
      character(LEN=21), parameter :: DECOMPRESSION_ATTR    = 'DECOMPRESSION FORMULA'
      character(LEN=31), parameter :: DECOMPRESSION_FORMULA = 'REAL32 = OFFSET+ INTEGER16/CAL]'
      integer ix,iy
      integer rank,dims(3),start(3),edges(3),stride(3)
      integer itime
      integer x0,x1,y0,y1; ! bounding box for each node within total domain
      character*5 ctime
      character*4 cid
      
#ifdef MPI
#     ifdef MAINC
      call get_registered_comm(comm)
#     else
      comm = MPI_COMM_WORLD
#     endif
#endif

#ifdef HDFOUT

! construct hdf node file name
      irec=1
      filename = '                                                                                '
      itime = int(model_time)
      write(ctime,100)itime
      write(cid,101)myid
      filename = TRIM(output_path) // TRIM(output_basename) // '.' // ctime // '_' // cid // '.hdf'

100   Format(i5.5)
101   Format(i4.4)

      write(outfile,*)
      write(outfile,*) '  myid,filename=',myid,'   ',filename

      sd_id = sfstart(filename, DFACC_CREATE)
      if (sd_id .EQ. FAIL) then
         print *,'writeout_multhdf:  problem opening hdf file'
         print *,'writeout_multhdf:  filename = ', filename
         print *,'writeout_multhdf:  sds_id = ', sd_id
         stop
      endif
      if (lossy_compress) then
         n = 31
         status = sfscatt(sd_id, COMPRESSION_ATTR, DFNT_CHAR8, n, COMPRESSION_FORMULA)
         if (.not. status .eq. 0) write (*,*) myid ,": sfscatt comp attr failed with status",  status
         status = sfscatt(sd_id, DECOMPRESSION_ATTR, DFNT_CHAR8, n, DECOMPRESSION_FORMULA)
         if (.not. status .eq. 0) write (*,*) myid ,": sfscatt decomp comp attr failed with status",  status
      endif

       rank=1
       dims(1)=1
       start(1)=0
       edges(1)=1
       stride(1)=1

       x0 = (myi-1)*ni;
       x1 = myi*ni-1;
       y0 = (myj-1)*nj;
       y1 = myj*nj-1;

       sds_id = sfcreate(sd_id,'time',DFNT_FLOAT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, model_time)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata model_time failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'dx',DFNT_FLOAT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, dx)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata dx failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'dy',DFNT_FLOAT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, dy)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata dy failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'x0',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, x0)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata x0 failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'xf',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, x1)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata xf failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'y0',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, y0)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata y0 failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'yf',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, y1)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata yf failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'myi',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, myi)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata myi failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'myj',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, myj)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata myj failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'numi',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, ni)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata ni failed with status",  status

       status = sfendacc(sds_id)
       sds_id = sfcreate(sd_id,'numj',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, nj)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata nj failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'nodex',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, nodex)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata nodex failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'nodey',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, nodey)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata nodey failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'nx',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, nx)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata nx failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'ny',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, ny)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata ny failed with status",  status
       status = sfendacc(sds_id)

       sds_id = sfcreate(sd_id,'nz',DFNT_INT32,rank,dims)
       status = sfwdata(sds_id, start, stride, edges, nz)
       if (.not. status .eq. 0) write (*,*) myid ,": sfwdata nz failed with status",  status
       status = sfendacc(sds_id)

! ORF write out base state vars here. Note: Explicitly assumes NO TERRAIN

      do k=1,nk
          zhcol(k)=zh(1,1,k)
          zfcol(k)=zf(1,1,k)
          u0col(k) = u0(1,1,k)
          v0col(k) = v0(1,1,k)
          th0col(k) = th0(1,1,k)
          pi0col(k) = pi0(1,1,k)
          rho0col(k) = rho0(1,1,k)
          pres0col(k) = p00*(pi0(1,1,k))**cpdrd
      enddo

      do j = 1,nj
          yfcol(j) = yf(j)
          yhcol(j) = yh(j)
      enddo

      do i = 1,ni
          xfcol(i) = xf(i)
          xhcol(i) = xh(i)
      enddo

      call write1d_hdf(sd_id,nk,zhcol,       'zh                  ')
      call write1d_hdf(sd_id,nk,zfcol,       'zf                  ')
      call write1d_hdf(sd_id,nj,yhcol,       'yh                  ')
      call write1d_hdf(sd_id,nj,yfcol,       'yf                  ')
      call write1d_hdf(sd_id,ni,xhcol,       'xh                  ')
      call write1d_hdf(sd_id,ni,xfcol,       'xf                  ')
      call write1d_hdf(sd_id,nk,u0col,       'u0                  ')
      call write1d_hdf(sd_id,nk,v0col,       'v0                  ')
      call write1d_hdf(sd_id,nk,th0col,      'th0                 ')
      call write1d_hdf(sd_id,nk,pi0col,      'pi0                 ')
      call write1d_hdf(sd_id,nk,pres0col,    'pres0               ')
      call write1d_hdf(sd_id,nk,rho0col,     'rho0                ')

      if(output_rain.eq.1) then
        do j=1,nj
        do i=1,ni
          ds2(i,j)=rain(i,j,1)
        enddo
        enddo
        call write2d_hdf(sd_id,ni,nj,ds2,'sfcrain             ')
        if (nrain.eq.2) then
          do j=1,nj
          do i=1,ni
            ds2(i,j)=rain(i,j,2)
          enddo
          enddo
          call write2d_hdf(sd_id,ni,nj,ds2,'sfcrain_trans       ')
        endif
      endif

      if(output_sws.eq.1) then
        do j=1,nj
        do i=1,ni
          ds2(i,j)=sws(i,j,1)
        enddo
        enddo
        call write2d_hdf(sd_id,ni,nj,ds2,'maxsws              ')
        if (nrain.eq.2) then
          do j=1,nj
          do i=1,ni
            ds2(i,j)=sws(i,j,2)
          enddo
          enddo
          call write2d_hdf(sd_id,ni,nj,ds2,'maxsws_trans        ')
        endif
      endif
 
      if(output_zs  .eq.1) call write2d_hdf(sd_id,ni,nj,zs,  'terrainheight       ')
! Should we add terrain? I have assumed no terrain and reduced zh,zf into a 1D column
!      if(output_zh  .eq.1) call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,dum1,'zh                 ')
      if(output_th  .eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=th0(i,j,k)+tha(i,j,k)
        enddo
        enddo
        enddo
        description = 'Potential temperature (Kelvins)                   '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Th                  ',description)
      endif

      if(output_thpert .eq.1) then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=tha(i,j,k)
        enddo
        enddo
        enddo
        description = 'Perturbation potential temperature (Kelvins)      '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'ThPert              ',description)
      endif

      if(output_prs    .eq.1) then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=prs(i,j,k)
        enddo
        enddo
        enddo
        description = 'Pressure (Pascals)                                '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'P                   ',description)
      endif

      if(output_prspert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=prs(i,j,k)-p00*(pi0(i,j,k)**cpdrd)
        enddo
        enddo
        enddo
        description = 'Perturbation pressure (Pascals)                   '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'PPert               ',description)
      endif
      if(output_pi.eq.1)then  
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=pi0(i,j,k)+ppi(i,j,k)
        enddo
        enddo
        enddo
        description = 'Nondimensional pressure / Exner ()                '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Pi                  ',description)
      endif
      if(output_pipert .eq.1) then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=ppi(i,j,k)
        enddo
        enddo
        enddo
        description = 'Nondimensional pressure / Exner perturbation ()   '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'PiPert              ',description)
      endif

      if(output_rho    .eq.1) then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=rho(i,j,k)
        enddo
        enddo
        enddo
        description = 'Density (kg/m^3)                                  '
      call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Rho                 ',description)
      endif
      if(output_rhopert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=rho(i,j,k)-rho0(i,j,k)
        enddo
        enddo
        enddo
        description = 'Density perturbation (kg/m^3)                     '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'RhoPert             ',description)
      endif
      if(iturb.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=tkea(i,j,k)
        enddo
        enddo
        enddo
        description = 'Turbulent kinetic energy (J/kg)                   '
      call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Tke                 ',description)
      endif

      if(iturb.ge.1)then

        if(output_km  .eq.1) then

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=kmh(i,j,k)
        enddo
        enddo
        enddo
        description = 'Eddy visc coeff for momentum in horiz (m^2/s^2)   '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Kmh                 ',description)

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=kmv(i,j,k)
        enddo
        enddo
        enddo
        description = 'Eddy visc coeff for momentum in vert (m^2/s^2)    '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Kmv                 ',description)
      endif

      if(output_kh  .eq.1) then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=khh(i,j,k)
        enddo
        enddo
        enddo
        description = 'Eddy visc coeff for heat in vert (m^2/s^2)        '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Khh                 ',description)

!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=khv(i,j,k)
        enddo
        enddo
        enddo
        description = 'Eddy visc coeff for heat in horiz (m^2/s^2)       '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Khv                 ',description)
      endif
    endif
! ORF deal with passive tracers later
!      if(iptra.eq.1)then
!        do n=1,npt
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
!          do k=1,nk
!          do j=1,nj
!          do i=1,ni
!            dum1(i,j,k)=pta(i,j,k,n)
!          enddo
!          enddo
!          enddo
!          call write3d_hdf(etc)
!        enddo
!      endif
      if(imoist.eq.1)then
        do k=1,nk
        do j=1,nj
        do i=1,ni
          ds(i,j,k)=qa(i,j,k,nqv)
        enddo
        enddo
        enddo
        description = 'Water vapor mixing ratio (kg/kg)                  '
        if(output_qv    .eq.1) call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Qv                  ',description)
        if(output_qvpert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=qa(i,j,k,nqv)-qv0(i,j,k)
          enddo
          enddo
          enddo
          description = 'Perturbation water vapor mixing ratio (kg/kg)     '
          call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'QvPert              ',description)
        endif
        if(output_q.eq.1)then
! Should do something like this, but it would have to be done in
! param.F where microphysics stuff is done
! For now generic placeholder for microphysics variables
!         qnamedesc(1) = 'Cloud water mixing ratio (kg/kg)                  '
!         qnamedesc(2) = 'Rain water mixing ratio (kg/kg)                   '
!         qnamedesc(3) = 'Cloud ice mixing ratio (kg/kg)                    '
!         qnamedesc(4) = 'Snow mixing ratio (kg/kg)                         '
!         qnamedesc(5) = 'Graupel mixing ratio (kg/kg)                      '
          do n=1,numq
            if(n.ne.nqv)then
              do k=1,nk
              do j=1,nj
              do i=1,ni
                ds(i,j,k)=qa(i,j,k,n)
              enddo
              enddo
              enddo
          qvarname='                    '
          write(qvarname(1:3),156) qname(n)
156       Format(a3)
          description = 'Microphysics variable (TODO: properly label)      '
              call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,qvarname,description)
            endif
          enddo
        endif
        if(output_dbz.eq.1) then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
          do k=1,nk
          do j=1,nj
          do i=1,ni
            ds(i,j,k)=dbz(i,j,k)
          enddo
          enddo
          enddo
          description = 'Reflectivity (dBZ)                                '
          call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,ds,'Dbz                 ',description)
        endif
      endif
      if(output_uinterp.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni+1
          du(i,j,k)=0.5*(ua(i,j,k)+ua(i+1,j,k))
        enddo
        enddo
        enddo
        description = 'U interpolated to scalar points (m/s)             '
        call write3d_hdf(lossy_compress,sd_id,ni+1,nj,nk,du,'Uinterp             ',description)
      endif
      if(output_vinterp.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj+1
        do i=1,ni
          dv(i,j,k)=0.5*(va(i,j,k)+va(i,j+1,k))
        enddo
        enddo
        enddo
        description = 'V interpolated to scalar points (m/s)             '
        call write3d_hdf(lossy_compress,sd_id,ni,nj+1,nk,dv,'Vinterp             ',description)
      endif
      if(output_winterp.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk+1
        do j=1,nj
        do i=1,ni
          dw(i,j,k)=0.5*(wa(i,j,k)+wa(i,j,k+1))
        enddo
        enddo
        enddo
        description = 'W interpolated to scalar points (m/s)             '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk,dw,'Winterp             ',description)
      endif

!--------------------------------------------------------------
!  writeout data on u-points

      irec=1+(nwrite-1)*nk*u_out
#ifdef MPI
      irec=1
#endif

! Note velocity data is staggered and requires 1 extra point

       if(output_u    .eq.1) then
        do k=1,nk
        do j=1,nj
        do i=1,ni+1
          du(i,j,k)=ua(i,j,k)
        enddo
        enddo
        enddo
        description = 'U on native C grid (m/s)                          '
        call write3d_hdf(lossy_compress,sd_id,ni+1,nj,nk,du,'U                   ',description)
      endif


      if(output_upert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj
        do i=1,ni+1
          du(i,j,k)=ua(i,j,k)-u0(i,j,k)
        enddo
        enddo
        enddo
        description = 'Perturbation u on native C grid (m/s)             '
        call write3d_hdf(lossy_compress,sd_id,ni+1,nj,nk,du,'Upert               ',description)
      endif

!--------------------------------------------------------------
!  writeout data on v-points

      irec=1+(nwrite-1)*nk*v_out
#ifdef MPI
      irec=1
#endif

      if(output_v    .eq.1) then
        do k=1,nk
        do j=1,nj+1
        do i=1,ni
          dv(i,j,k)=va(i,j,k)
        enddo
        enddo
        enddo
        description = 'V on native C grid (m/s)                          '
        call write3d_hdf(lossy_compress,sd_id,ni,nj+1,nk,dv,'V                   ',description)
      endif


      if(output_vpert.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j,k)
        do k=1,nk
        do j=1,nj+1
        do i=1,ni
          dv(i,j,k)=va(i,j,k)-v0(i,j,k)
        enddo
        enddo
        enddo
        description = 'Perturbation v on native C grid (m/s)             '
        call write3d_hdf(lossy_compress,sd_id,ni,nj+1,nk,dv,'Vpert               ',description)
      endif

!--------------------------------------------------------------
!  writeout data on w-points

      irec=1+(nwrite-1)*(nk+1)*w_out
#ifdef MPI
      irec=1
#endif

      if(output_w    .eq.1) then
        do k=1,nk+1
        do j=1,nj
        do i=1,ni
          dw(i,j,k)=wa(i,j,k)
        enddo
        enddo
        enddo
          
        description = 'W on native C grid (m/s)                          '
        call write3d_hdf(lossy_compress,sd_id,ni,nj,nk+1,dw,'W                   ',description)
      endif

!--------------------------------------------------------------

      write(outfile,*)
      write(outfile,*) 'Done Writing Data to File: ',filename
      write(outfile,*)

#ifdef MPI
      call MPI_BARRIER (comm,ierr)
#endif
      status = sfend(sd_id)
!      call f77flush()

      return
      end

! ORF routine to write out 1d floating point arrays to HDF file.

      subroutine write1d_hdf(sd_id,numk,var,varname)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'hdf.f90'
!!!      include 'mffunc.f90'
      include 'dffunc.f90'
#ifdef MPI
      include 'mpif.h'
      integer comm
#endif


      integer :: sds_id,sd_id,numk,sourcerank,rowx,coly,tag,status
      character(LEN=20) :: varname
      real, dimension(1:numk) :: var
      integer rank, start(1), edges(1), dims(1), stride(1)
      integer iz

#ifdef MPI
#     ifdef MAINC
      call get_registered_comm(comm)
#     else
      comm = MPI_COMM_WORLD
#     endif
#endif

      rank=1
      dims(1)=numk
      sds_id = sfcreate(sd_id,varname,DFNT_FLOAT32,rank,dims)
      if (sds_id .eq. FAIL) print *,'sfcreate failed in write1d for myid, varname ',myid,varname

      start(1)=0
      stride(1)=1
      edges(1)=numk
      status = sfwdata(sds_id, start, stride, edges, var)
      if (status .eq. FAIL) print *,'sfwdata in write1d failed at node',myid
      status = sfendacc(sds_id)
      if (status .eq. FAIL) print *,'sfendacc in write1d failed at node',myid

      return
      end

! ORF routine to write out 2d floating point arrays to HDF file.

      subroutine write2d_hdf(sd_id,numi,numj,var,varname)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'hdf.f90'
!!!      include 'mffunc.f90'
      include 'dffunc.f90'
#ifdef MPI
      include 'mpif.h'
      integer comm
#endif


      integer :: sds_id,sd_id,numi,numj,sourcerank,rowx,coly,tag,status
      character(LEN=20) :: varname
!     real, dimension(-2:numi+3,-2:numj+3) :: var
      real, dimension(1:numi,1:numj) :: var
!     real, dimension(1:numi,1:numj) :: dumxy
      integer rank, start(2), edges(2), dims(2), stride(2)

      integer i,j,ix,iy

#ifdef MPI
#     ifdef MAINC
      call get_registered_comm(comm)
#     else
      comm = MPI_COMM_WORLD
#     endif
#endif

      rank=2
      dims(1)=numi
      dims(2)=numj
      sds_id = sfcreate(sd_id,varname,DFNT_FLOAT32,rank,dims)
      if (sds_id .eq. FAIL) print *,'sfcreate failed in write2d for myid, varname ',myid,varname

!      do ix=1,numi
!           do iy=1,numj
!              dumxy(ix,iy)=var(ix,iy)
!           end do
!      end do

      start(1)=0
      start(2)=0
      stride(1)=1
      stride(2)=1
      edges(1)=numi
      edges(2)=numj
!     status = sfwdata(sds_id, start, stride, edges, dumxy)
      status = sfwdata(sds_id, start, stride, edges, var)
      if (status .eq. FAIL) print *,'sfwdata var failed at node',myid
      status = sfendacc(sds_id)
      if (status .eq. FAIL) print *,'sfendacc failed at node',myid

      return
      end

! ORF routine to write out 3d floating point arrays to HDF file.
! Data may be stored either lossy or losslessly. Both formats have gzip
! compression applied (which is lossless).

      subroutine write3d_hdf(lossy_compress,sd_id,numi,numj,numk,var,varname,description)
      implicit none

      include 'input.incl'
      include 'constants.incl'
      include 'hdf.f90'
!!!      include 'mffunc.f90'
      include 'dffunc.f90'
#ifdef MPI
      include 'mpif.h'
      integer comm
#endif


      logical lossy_compress
      integer :: sds_id,sd_id,numi,numj,numk,sourcerank,rowx,coly,tag,status
      character(LEN=20) :: varname
      real, dimension(1:numi,1:numj,1:numk) :: var
      integer(kind=2), dimension(1:numi,1:numj,1:numk) :: dumxyzi
      integer rank, start(3), edges(3), dims(3), stride(3)

      integer i,j,k,ix,iy,iz
      integer nsend

      integer(kind=4), parameter :: twoscale = 2**14-1
      real(kind=8) cal, offset, cal_error, offset_error
      real fmax, fmin, globalfmax, globalfmin
      integer, parameter    :: COMP_CODE    = 4       ! gzip compression
      integer, dimension(1) :: comp_prn(1)  = 4       ! can range from 1-9, 4 is a good comprimise between speed and compression
      character*50 description
      
#ifdef MPI
#     ifdef MAINC
      call get_registered_comm(comm)
#     else
      comm = MPI_COMM_WORLD
#     endif
#endif

      rank=3
      dims(1)=numi
      dims(2)=numj
      dims(3)=numk

      if (lossy_compress) then
          sds_id = sfcreate(sd_id, varname, DFNT_INT16, rank, dims) ! 4 byte scaled ints, lossy
          IF (sds_id .eq. FAIL) print *,'sfcreate failed in write3d_hdf variable ', varname
          status = sfscompress(sds_id, COMP_CODE, comp_prn) !lossless gzip compression
          IF (status .eq. FAIL) print *,'sfscompress failed in write3d_hdf variable', varname
      else
          sds_id = sfcreate(sd_id, varname, DFNT_FLOAT32, rank, dims) ! 8 bytes floats, lossless
          IF (sds_id .eq. FAIL) print *,'sfcreate failed in write3d_hdf variable ', varname
          status = sfscompress(sds_id, COMP_CODE, comp_prn) !lossless gzip compression
          IF (status .eq. FAIL) print *,'sfscompress failed in write3d_hdf variable', varname
      endif

! Find max and mins of data

      fmax = -1.0E20
      fmin =  1.0E20
      do iz=1,numk
         do iy=1,numj
            do ix=1,numi
                if (var(ix,iy,iz) < fmin) fmin = var(ix,iy,iz)
                if (var(ix,iy,iz) > fmax) fmax = var(ix,iy,iz)
            end do
          end do
       end do

#ifdef MPI
       call MPI_Allreduce ( fmax, globalfmax, 1, MPI_REAL, MPI_MAX, comm,ierr)
       call MPI_Allreduce ( fmin, globalfmin, 1, MPI_REAL, MPI_MIN, comm,ierr)
#endif
       if((globalfmax - globalfmin) .lt. 1.0e-10) globalfmax=globalfmin+1.0e-10

       cal          = twoscale / (globalfmax-globalfmin)
       offset       = globalfmin
       cal_error    = dble(0.0)
       offset_error = dble(0.0)

       if (lossy_compress) then
           status = sfscal(sds_id, cal, cal_error, offset, offset_error, DFNT_FLOAT32)
           IF (status .eq. FAIL) print *,'sfscal failed in write3d_hdf variable ', varname
        endif

! ORF write local and global max & mins attributes to 3d variable
       status = sfsnatt(sds_id,'localmax',DFNT_FLOAT32,1,fmax)
           IF (status .eq. FAIL) print *,'sfsnatt failed for ', varname
       status = sfsnatt(sds_id,'localmin',DFNT_FLOAT32,1,fmin)
           IF (status .eq. FAIL) print *,'sfsnatt failed for ', varname
       status = sfsnatt(sds_id,'globalmax',DFNT_FLOAT32,1,globalfmax)
           IF (status .eq. FAIL) print *,'sfsnatt failed for ', varname
       status = sfsnatt(sds_id,'globalmin',DFNT_FLOAT32,1,globalfmin)
           IF (status .eq. FAIL) print *,'sfsnatt failed for ', varname
        status = sfscatt(sds_id,'description',DFNT_CHAR8,50,description)
            IF (status .eq. FAIL) print *,'sfscatt failed for ', varname

      start(1)=0
      start(2)=0
      start(3)=0
      stride(1)=1
      stride(2)=1
      stride(3)=1
      edges(1)=numi
      edges(2)=numj
      edges(3)=numk
     
      if (lossy_compress) then

          do iz = 1,numk
             do iy = 1,numj
                do ix = 1,numi
                    dumxyzi(ix,iy,iz) = cal*(var(ix,iy,iz) - offset)
                enddo
             enddo
          enddo

          status = sfwdata(sds_id, start, stride, edges, dumxyzi)
          IF (status .eq. FAIL) print *,'sfwdata failed for lossy compressed variable ', varname

      else

          status = sfwdata(sds_id, start, stride, edges, var)
          IF (status .eq. FAIL) print *,'sfwdata failed for lossless floating point variable ', varname

      endif

      status = sfendacc(sds_id)
      if (status .eq. FAIL) print *,'sfendacc failed at node',myid
#endif

      return
      end
