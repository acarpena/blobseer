

      subroutine setup_output(qname,budname,xh,xf,yh,yf,xfref,yfref,zh,zf)
      implicit none

      include 'input.incl'

      character*3, dimension(maxq) :: qname
      character*6, dimension(maxq) :: budname
      real, dimension(ib:ie) :: xh
      real, dimension(ib:ie+1) :: xf
      real, dimension(jb:je) :: yh
      real, dimension(jb:je+1) :: yf
      real, dimension(-2:nx+4) :: xfref
      real, dimension(-2:ny+4) :: yfref
      real, dimension(ib:ie,jb:je,kb:ke) :: zh
      real, dimension(ib:ie,jb:je,kb:ke+1) :: zf

!-----------------------------------------------------------------------

      integer :: i,j,k,n,flag
      character*8 text1
      character*30 text2
      character*50 fname

!-----------------------------------------------------------------------
! get length of output_path string

    flag=0
    n=0
    do while( flag.eq.0 .and. n.le.70 )
      n=n+1
      if( output_path(n:n).eq.' ' .or. output_path(n:n).eq.'.' ) flag=1
    enddo

    strlen=n-1

!--------------------------------------
! get length of output_basename string

    flag=0
    n=0
    do while( flag.eq.0 .and. n.le.70 )
      n=n+1
      if( output_basename(n:n).eq.' ' .or. output_basename(n:n).eq.'.' ) flag=1
    enddo

    baselen=n-1

!------

    totlen = strlen + baselen

      string = '                                                                      '
    statfile = '                                                                      '
     sstring = '                                                                      '

  if(strlen.gt.0)then
      string(1:strlen) = output_path(1:strlen)
    statfile(1:strlen) = output_path(1:strlen)
  endif

      string(strlen+1:strlen+baselen) = output_basename(1:baselen)
    statfile(strlen+1:strlen+baselen) = output_basename(1:baselen)
     sstring(1:baselen) = output_basename(1:baselen)

    statfile(totlen+1:totlen+1+12) = '_stats.dat  '

    write(outfile,*)
    write(outfile,*) '  writing ctl files ... '
    write(outfile,*)
    write(outfile,*) '  strlen          = ',strlen
    write(outfile,*) '  baselen         = ',baselen
    write(outfile,*) '  totlen          = ',totlen
  if(strlen.gt.0)then
    write(outfile,*) '  output_path     = ',output_path(1:strlen)
  endif
    write(outfile,*) '  output_basename = ',output_basename(1:baselen)
    write(outfile,*) '  statfile        = ',statfile
    write(outfile,*)

!-----------------------------------------------------------------------
!  GrADS descriptor files
!-----------------------------------------------------------------------

  grads_descriptors: IF( output_format.eq.1 )THEN

      IF(myid.eq.0)THEN

!----------------------------

  IF(s_out.ge.1)THEN
    string(totlen+1:totlen+1+12) = '_s.ctl'
    write(outfile,*) string
    open(unit=50,file=string,status='unknown')

  if(output_filetype.eq.1)then
    sstring(baselen+1:baselen+1+12) = '_s.dat'
  elseif(output_filetype.eq.2)then
    sstring(baselen+1:baselen+1+12) = '_%y4_s.dat'
  endif
    write(50,201) sstring
!!!    write(50,222)
    if(output_filetype.eq.2) write(50,221)
    write(50,202)
    write(50,203)
    if(stretch_x.ge.1)then
      write(50,214) nx
      do i=1,nx
        write(50,217) 0.001*0.5*(xfref(i)+xfref(i+1))
      enddo
    else
      write(50,204) nx,xh(1)/1000.0,dx/1000.0
    endif
    if(stretch_y.ge.1)then
      write(50,215) ny
      do j=1,ny
        write(50,217) 0.001*0.5*(yfref(j)+yfref(j+1))
      enddo
    else
      write(50,205) ny,yh(1)/1000.0,dy/1000.0
    endif
    if(stretch_z.eq.0)then
      write(50,206) nz,0.5*dz/1000.0,dz/1000.0
    else
      write(50,216) nz
      do k=1,nz
        write(50,217) 0.001*zh(1,1,k)
      enddo
    endif
  if(output_filetype.eq.1)then
    write(50,207) int(1+timax/tapfrq),max(1,int(tapfrq/60.0))
  elseif(output_filetype.eq.2)then
    write(50,227) int(1+timax/tapfrq)
  endif
    write(50,208) s_out
    if(output_rain   .eq.1) write(50,209) 'rn      ', 0,'accumulated rainfall (cm)     '
    if(output_sws    .eq.1) write(50,209) 'sws     ', 0,'max. sfc wind speed (m/s)     '
    if(nrain.eq.2)then
      if(output_rain   .eq.1) write(50,209) 'rn2     ', 0,'translated rainfall (cm)      '
      if(output_sws    .eq.1) write(50,209) 'sws2    ', 0,'translated max wind (m/s)     '
    endif
    if(output_sfcflx .eq.1) write(50,209) 'thflux  ', 0,'sfc theta flux (K m/s)        '
    if(output_sfcflx .eq.1) write(50,209) 'qvflux  ', 0,'sfc water vapor flux (g/g m/s)'
    if(output_sfcflx .eq.1) write(50,209) 'cd      ', 0,'cd                            '
    if(output_sfcflx .eq.1) write(50,209) 'ce      ', 0,'ce                            '
    if(output_zs     .eq.1) write(50,209) 'zs      ', 0,'terrain height (m)            '
    if(output_zh     .eq.1) write(50,209) 'zh      ',nk,'height on model levels (m)    '
    if(output_th     .eq.1) write(50,209) 'th      ',nk,'potential temp. (K)           '
    if(output_thpert .eq.1) write(50,209) 'thpert  ',nk,'potential temp. pert. (K)     '
    if(output_prs    .eq.1) write(50,209) 'prs     ',nk,'pressure (Pa)                 '
    if(output_prspert.eq.1) write(50,209) 'prspert ',nk,'pressure pert. (Pa)           '
    if(output_pi     .eq.1) write(50,209) 'pi      ',nk,'nondimensional pressure       '
    if(output_pipert .eq.1) write(50,209) 'pipert  ',nk,'nondimensional pressure pert. '
    if(output_rho    .eq.1) write(50,209) 'rho     ',nk,'density (kg/m^3)              '
    if(output_rhopert.eq.1) write(50,209) 'rhopert ',nk,'density pert. (kg/m^3)        '
    if(iptra         .eq.1)then
      do n=1,npt
        text1='pt      '
        write(text1(3:3),155) n
155     format(i1.1)
        write(50,209) text1,nk,'passive tracer                '
      enddo
    endif
    if(output_qv     .eq.1) write(50,209) 'qv      ',nk,'water vapor mixing ratio      '
    if(output_qvpert .eq.1) write(50,209) 'qvpert  ',nk,'qv pert                       '
    if(output_q      .eq.1)then
      do n=1,numq
        if(n.ne.nqv)then
          text1='        '
          text2='                              '
          write(text1(1:3),156) qname(n)
          write(text2(1:3),156) qname(n)
156       format(a3)
          write(50,209) text1,nk,text2
        endif
      enddo
    endif
    if(output_dbz    .eq.1) write(50,209) 'dbz     ',nk,'reflectivity (dBZ)            '
    if(output_uinterp.eq.1) write(50,209) 'uinterp ',nk,'u interp. to scalar points    '
    if(output_vinterp.eq.1) write(50,209) 'vinterp ',nk,'v interp. to scalar points    '
    if(output_winterp.eq.1) write(50,209) 'winterp ',nk,'w interp. to scalar points    '
    if(output_basestate.eq.1) write(50,209) 'pi0     ',nk,'base-state nondim. pressure   '
    if(output_basestate.eq.1) write(50,209) 'th0     ',nk,'base-state potential temp (K) '
    if(output_basestate.eq.1) write(50,209) 'prs0    ',nk,'base-state pressure (Pa)      '
    if(output_basestate.eq.1) write(50,209) 'qv0     ',nk,'base-state qv (kg/kg)         '
    write(50,210)
    close(unit=50)
  ENDIF

!-----------------------------------

  IF(s_out.ge.1 .and. terrain_flag .and. output_interp.eq.1)THEN
    string(totlen+1:totlen+1+12) = '_i.ctl'
    write(outfile,*) string
    open(unit=50,file=string,status='unknown')

  if(output_filetype.eq.1)then
    sstring(baselen+1:baselen+1+12) = '_i.dat'
  elseif(output_filetype.eq.2)then
    sstring(baselen+1:baselen+1+12) = '_%y4_i.dat'
  endif

    write(50,201) sstring
!!!    write(50,222)
    if(output_filetype.eq.2) write(50,221)
    write(50,202)
    write(50,203)
    if(stretch_x.ge.1)then
      write(50,214) nx
      do i=1,nx
        write(50,217) 0.001*0.5*(xfref(i)+xfref(i+1))
      enddo
    else
      write(50,204) nx,xh(1)/1000.0,dx/1000.0
    endif
    if(stretch_y.ge.1)then
      write(50,215) ny
      do j=1,ny
        write(50,217) 0.001*0.5*(yfref(j)+yfref(j+1))
      enddo
    else
      write(50,205) ny,yh(1)/1000.0,dy/1000.0
    endif
    if(stretch_z.eq.0)then
      write(50,206) nz,0.5*dz/1000.0,dz/1000.0
    else
      write(50,216) nz
      do k=1,nz
        write(50,217) 0.001*zh(1,1,k)
      enddo
    endif
  if(output_filetype.eq.1)then
    write(50,207) int(1+timax/tapfrq),max(1,int(tapfrq/60.0))
  elseif(output_filetype.eq.2)then
    write(50,227) int(1+timax/tapfrq)
  endif
    write(50,208) s_out
    if(output_rain   .eq.1) write(50,209) 'rn      ', 0,'accumulated rainfall (cm)     '
    if(output_sws    .eq.1) write(50,209) 'sws     ', 0,'max. sfc wind speed (m/s)     '
    if(nrain.eq.2)then
      if(output_rain   .eq.1) write(50,209) 'rn2     ', 0,'translated rainfall (cm)      '
      if(output_sws    .eq.1) write(50,209) 'sws2    ', 0,'translated max wind (m/s)     '
    endif
    if(output_sfcflx .eq.1) write(50,209) 'thflux  ', 0,'sfc theta flux (K m/s)        '
    if(output_sfcflx .eq.1) write(50,209) 'qvflux  ', 0,'sfc water vapor flux (g/g m/s)'
    if(output_sfcflx .eq.1) write(50,209) 'cd      ', 0,'cd                            '
    if(output_sfcflx .eq.1) write(50,209) 'ce      ', 0,'ce                            '
    if(output_zs     .eq.1) write(50,209) 'zs      ', 0,'terrain height (m)            '
    if(output_zh     .eq.1) write(50,209) 'zh      ',nk,'height on model levels (m)    '
    if(output_th     .eq.1) write(50,209) 'th      ',nk,'potential temp. (K)           '
    if(output_thpert .eq.1) write(50,209) 'thpert  ',nk,'potential temp. pert. (K)     '
    if(output_prs    .eq.1) write(50,209) 'prs     ',nk,'pressure (Pa)                 '
    if(output_prspert.eq.1) write(50,209) 'prspert ',nk,'pressure pert. (Pa)           '
    if(output_pi     .eq.1) write(50,209) 'pi      ',nk,'nondimensional pressure       '
    if(output_pipert .eq.1) write(50,209) 'pipert  ',nk,'nondimensional pressure pert. '
    if(output_rho    .eq.1) write(50,209) 'rho     ',nk,'density (kg/m^3)              '
    if(output_rhopert.eq.1) write(50,209) 'rhopert ',nk,'density pert. (kg/m^3)        '
    if(iptra         .eq.1)then
      do n=1,npt
        text1='pt      '
        write(text1(3:3),155) n
        write(50,209) text1,nk,'passive tracer                '
      enddo
    endif
    if(output_qv     .eq.1) write(50,209) 'qv      ',nk,'water vapor mixing ratio      '
    if(output_qvpert .eq.1) write(50,209) 'qvpert  ',nk,'qv pert                       '
    if(output_q      .eq.1)then
      do n=1,numq
        if(n.ne.nqv)then
          text1='        '
          text2='                              '
          write(text1(1:3),156) qname(n)
          write(text2(1:3),156) qname(n)
          write(50,209) text1,nk,text2
        endif
      enddo
    endif
    if(output_dbz    .eq.1) write(50,209) 'dbz     ',nk,'reflectivity (dBZ)            '
    if(output_uinterp.eq.1) write(50,209) 'uinterp ',nk,'u interp. to scalar points    '
    if(output_vinterp.eq.1) write(50,209) 'vinterp ',nk,'v interp. to scalar points    '
    if(output_winterp.eq.1) write(50,209) 'winterp ',nk,'w interp. to scalar points    '
    if(output_basestate.eq.1) write(50,209) 'pi0     ',nk,'base-state nondim. pressure   '
    if(output_basestate.eq.1) write(50,209) 'th0     ',nk,'base-state potential temp (K) '
    if(output_basestate.eq.1) write(50,209) 'prs0    ',nk,'base-state pressure (Pa)      '
    if(output_basestate.eq.1) write(50,209) 'qv0     ',nk,'base-state qv (kg/kg)         '
    write(50,210)
    close(unit=50)
  ENDIF

!-----------------------------------

  IF(u_out.ge.1)THEN
    string(totlen+1:totlen+1+12) = '_u.ctl'
    write(outfile,*) string
    open(unit=50,file=string,status='unknown')

  if(output_filetype.eq.1)then
    sstring(baselen+1:baselen+1+12) = '_u.dat'
  elseif(output_filetype.eq.2)then
    sstring(baselen+1:baselen+1+12) = '_%y4_u.dat'
  endif

    write(50,201) sstring
    if(output_filetype.eq.2) write(50,221)
    write(50,202)
    write(50,203)
    if(stretch_x.ge.1)then
      write(50,214) nx+1
      do i=1,nx+1
        write(50,217) 0.001*xfref(i)
      enddo
    else
      write(50,204) nx+1,xf(1)/1000.0,dx/1000.0
    endif
    if(stretch_y.ge.1)then
      write(50,215) ny
      do j=1,ny
        write(50,217) 0.001*0.5*(yfref(j)+yfref(j+1))
      enddo
    else
      write(50,205) ny,yh(1)/1000.0,dy/1000.0
    endif
    if(stretch_z.eq.0)then
      write(50,206) nz,0.5*dz/1000.0,dz/1000.0
    else
      write(50,216) nz
      do k=1,nz
        write(50,217) 0.001*zh(1,1,k)
      enddo
    endif
  if(output_filetype.eq.1)then
    write(50,207) int(1+timax/tapfrq),max(1,int(tapfrq/60.0))
  elseif(output_filetype.eq.2)then
    write(50,227) int(1+timax/tapfrq)
  endif
    write(50,208) u_out
    if(output_u    .eq.1) write(50,209) 'u       ',nk,'E-W velocity (m/s)            '
    if(output_upert.eq.1) write(50,209) 'upert   ',nk,'u pert. (m/s)                 '
    if(output_basestate.eq.1) write(50,209) 'u0      ',nk,'base-state u (m/s)            '
    write(50,210)
    close(unit=50)
  ENDIF

!-----------------------------------

  IF(v_out.ge.1)THEN
    string(totlen+1:totlen+1+12) = '_v.ctl'
    write(outfile,*) string
    open(unit=50,file=string,status='unknown')

  if(output_filetype.eq.1)then
    sstring(baselen+1:baselen+1+12) = '_v.dat'
  elseif(output_filetype.eq.2)then
    sstring(baselen+1:baselen+1+12) = '_%y4_v.dat'
  endif

    write(50,201) sstring
    if(output_filetype.eq.2) write(50,221)
    write(50,202)
    write(50,203)
    if(stretch_x.ge.1)then
      write(50,214) nx
      do i=1,nx
        write(50,217) 0.001*0.5*(xfref(i)+xfref(i+1))
      enddo
    else
      write(50,204) nx,xh(1)/1000.0,dx/1000.0
    endif
    if(stretch_y.ge.1)then
      write(50,215) ny+1
      do j=1,ny+1
        write(50,217) 0.001*yfref(j)
      enddo
    else
      write(50,205) ny+1,yf(1)/1000.0,dy/1000.0
    endif
    if(stretch_z.eq.0)then
      write(50,206) nz,0.5*dz/1000.0,dz/1000.0
    else
      write(50,216) nz
      do k=1,nz
        write(50,217) 0.001*zh(1,1,k)
      enddo
    endif
  if(output_filetype.eq.1)then
    write(50,207) int(1+timax/tapfrq),max(1,int(tapfrq/60.0))
  elseif(output_filetype.eq.2)then
    write(50,227) int(1+timax/tapfrq)
  endif
    write(50,208) v_out
    if(output_v    .eq.1) write(50,209) 'v       ',nk,'N-S velocity (m/s)            '
    if(output_vpert.eq.1) write(50,209) 'vpert   ',nk,'v pert (m/s)                  '
    if(output_basestate.eq.1) write(50,209) 'v0      ',nk,'base-state v (m/s)            '
    write(50,210)
    close(unit=50)
  ENDIF

!-----------------------------------

  IF(w_out.ge.1)THEN
    string(totlen+1:totlen+1+12) = '_w.ctl'
    write(outfile,*) string
    open(unit=50,file=string,status='unknown')

  if(output_filetype.eq.1)then
    sstring(baselen+1:baselen+1+12) = '_w.dat'
  elseif(output_filetype.eq.2)then
    sstring(baselen+1:baselen+1+12) = '_%y4_w.dat'
  endif

    write(50,201) sstring
    if(output_filetype.eq.2) write(50,221)
    write(50,202)
    write(50,203)
    if(stretch_x.ge.1)then
      write(50,214) nx
      do i=1,nx
        write(50,217) 0.001*0.5*(xfref(i)+xfref(i+1))
      enddo
    else
      write(50,204) nx,xh(1)/1000.0,dx/1000.0
    endif
    if(stretch_y.ge.1)then
      write(50,215) ny
      do j=1,ny
        write(50,217) 0.001*0.5*(yfref(j)+yfref(j+1))
      enddo
    else
      write(50,205) ny,yh(1)/1000.0,dy/1000.0
    endif
    if(stretch_z.eq.0)then
      write(50,206) nz+1,0.0,dz/1000.0
    else
      write(50,216) nz+1
      do k=1,nz+1
        write(50,217) 0.001*zf(1,1,k)
      enddo
    endif
  if(output_filetype.eq.1)then
    write(50,207) int(1+timax/tapfrq),max(1,int(tapfrq/60.0))
  elseif(output_filetype.eq.2)then
    write(50,227) int(1+timax/tapfrq)
  endif
    write(50,208) w_out
    if(output_w  .eq.1) write(50,209) 'w       ',nk+1,'vertical velocity (m/s)       '
    if(output_tke.eq.1) write(50,209) 'tke     ',nk+1,'turb. kinetic energy (m^2/s^2)'
    if(output_km .eq.1) write(50,209) 'kmh     ',nk+1,'turb. coef. for mo. (m^2/s)   '
    if(output_km .eq.1) write(50,209) 'kmv     ',nk+1,'turb. coef. for mo. (m^2/s)   '
    if(output_kh .eq.1) write(50,209) 'khh     ',nk+1,'turb. coef. for scalar (m^2/s)'
    if(output_kh .eq.1) write(50,209) 'khv     ',nk+1,'turb. coef. for scalar (m^2/s)'
    write(50,210)
    close(unit=50)
  ENDIF

    write(outfile,*)

!-----------------------------------

201   format('dset ^',a70)
202   format('title CM1 output')
221   format('options template')
222   format('byteswapped')
203   format('undef -99999999.')
204   format('xdef ',i6,' linear ',f13.6,1x,f13.6)
214   format('xdef ',i6,' levels ')
205   format('ydef ',i6,' linear ',f13.6,1x,f13.6)
215   format('ydef ',i6,' levels ')
206   format('zdef ',i6,' linear ',f13.6,1x,f13.6)
216   format('zdef ',i6,' levels ')
217   format(2x,f13.6)
207   format('tdef ',i10,' linear 00Z03JUL2000 ',i5,'MN')
227   format('tdef ',i10,' linear 00Z03JUL0001 1YR')
208   format('vars ',i4)
209   format(a8,2x,i6,'  99  ',a30)
210   format('endvars')

211   format(2x,f7.3)

!-----------------------------------------------------------------------
!  write descriptors for stats file:


    string(totlen+1:totlen+1+12) = '_stats.ctl  '
    write(outfile,*) string
    open(unit=50,file=string,status='unknown')

    sstring(baselen+1:baselen+1+12) = '_stats.dat  '

    write(50,301) sstring
    write(50,302)
    write(50,303)
    write(50,304)
    write(50,305)
    write(50,306)
    write(50,307) 1+nint(timax/dtl/statfrq),max(1,int(statfrq*dtl/60.0))
    write(50,308) stat_out
    if(stat_w      .eq.1) write(50,309) 'wmax    ','max vertical velocity (m/s)   '
    if(stat_w      .eq.1) write(50,309) 'wmin    ','min vertical velocity (m/s)   '
    if(stat_u      .eq.1) write(50,309) 'umax    ','max E-W velocity (m/s)        '
    if(stat_u      .eq.1) write(50,309) 'umin    ','min E-W velocity (m/s)        '
    if(stat_u      .eq.1) write(50,309) 'sumax   ','max sfc E-W velocity (m/s)    '
    if(stat_u      .eq.1) write(50,309) 'sumin   ','min sfc E-W velocity (m/s)    '
    if(stat_v      .eq.1) write(50,309) 'vmax    ','max N-S velocity (m/s)        '
    if(stat_v      .eq.1) write(50,309) 'vmin    ','min N-S velocity (m/s)        '
    if(stat_v      .eq.1) write(50,309) 'svmax   ','max sfc N-S velocity (m/s)    '
    if(stat_v      .eq.1) write(50,309) 'svmin   ','min sfc N-S velocity (m/s)    '
    if(stat_rmw    .eq.1) write(50,309) 'rmw     ','radius of maximum V (m)       '
    if(stat_pipert .eq.1) write(50,309) 'ppimax  ','max pi pert.                  '
    if(stat_pipert .eq.1) write(50,309) 'ppimin  ','min pi pert.                  '
    if(stat_prspert.eq.1) write(50,309) 'ppmax   ','max prs pert.(Pa)             '
    if(stat_prspert.eq.1) write(50,309) 'ppmin   ','min prs pert.(Pa)             '
    if(stat_thpert .eq.1) write(50,309) 'thpmax  ','max potential temp. pert. (K) '
    if(stat_thpert .eq.1) write(50,309) 'thpmin  ','min potential temp. pert. (K) '
    if(stat_thpert .eq.1) write(50,309) 'sthpmax ','max sfc pot temp. pert. (K)   '
    if(stat_thpert .eq.1) write(50,309) 'sthpmin ','min sfc pot temp. pert. (K)   '
    if(stat_q      .eq.1)then
      do n=1,numq
        text1='max     '
        text2='max                           '
        write(text1(4:6),156) qname(n)
        write(text2(5:7),156) qname(n)
        write(50,309) text1,text2
        text1='min     '
        text2='min                           '
        write(text1(4:6),156) qname(n)
        write(text2(5:7),156) qname(n)
        write(50,309) text1,text2
      enddo
    endif
    if(stat_tke    .eq.1) write(50,309) 'tkemax  ','max tke (m^2/s^2)             '
    if(stat_tke    .eq.1) write(50,309) 'tkemin  ','min tke (m^2/s^2)             '
    if(stat_km     .eq.1) write(50,309) 'kmhmax  ','max kmh (m^2/s)               '
    if(stat_km     .eq.1) write(50,309) 'kmhmin  ','min kmh (m^2/s)               '
    if(stat_km     .eq.1) write(50,309) 'kmvmax  ','max kmv (m^2/s)               '
    if(stat_km     .eq.1) write(50,309) 'kmvmin  ','min kmv (m^2/s)               '
    if(stat_kh     .eq.1) write(50,309) 'khhmax  ','max khh (m^2/s)               '
    if(stat_kh     .eq.1) write(50,309) 'khhmin  ','min khh (m^2/s)               '
    if(stat_kh     .eq.1) write(50,309) 'khvmax  ','max khv (m^2/s)               '
    if(stat_kh     .eq.1) write(50,309) 'khvmin  ','min khv (m^2/s)               '
    if(stat_div    .eq.1) write(50,309) 'divmax  ','max 3d divergence             '
    if(stat_div    .eq.1) write(50,309) 'divmin  ','min 3d divergence             '
    if(stat_rh     .eq.1) write(50,309) 'rhmax   ','max relative humidity         '
    if(stat_rh     .eq.1) write(50,309) 'rhmin   ','min relative humidity         '
    if(stat_rhi    .eq.1) write(50,309) 'rhimax  ','max relative humidity wrt ice '
    if(stat_rhi    .eq.1) write(50,309) 'rhimin  ','min relative humidity wrt ice '
    if(iptra       .eq.1)then
      do n=1,npt
        text1='maxpt   '
        text2='max pt                        '
        write(text1(6:6),157) n
        write(text2(7:7),157) n
157     format(i1)
        write(50,309) text1,text2
        text1='minpt   '
        text2='min pt                        '
        write(text1(6:6),157) n
        write(text2(7:7),157) n
        write(50,309) text1,text2
      enddo
    endif
    if(stat_the    .eq.1) write(50,309) 'themax  ','max theta-e below 10 km       '
    if(stat_the    .eq.1) write(50,309) 'themin  ','min theta-e below 10 km       '
    if(stat_the    .eq.1) write(50,309) 'sthemax ','max theta-e at surface        '
    if(stat_the    .eq.1) write(50,309) 'sthemin ','min theta-e at surface        '
    if(stat_cloud  .eq.1) write(50,309) 'qctop   ','max cloud top height (m)      '
    if(stat_cloud  .eq.1) write(50,309) 'qcbot   ','min cloud base height (m)     '
    if(stat_sfcprs .eq.1) write(50,309) 'sprsmax ','max surface pressure (Pa)     '
    if(stat_sfcprs .eq.1) write(50,309) 'sprsmin ','min surface pressure (Pa)     '
    if(stat_wsp    .eq.1) write(50,309) 'wspmax  ','max wind speed (m/s)          '
    if(stat_wsp    .eq.1) write(50,309) 'wspmin  ','min wind speed (m/s)          '
    if(stat_wsp    .eq.1) write(50,309) 'swspmax ','max surface wind speed (m/s)  '
    if(stat_wsp    .eq.1) write(50,309) 'swspmin ','min surface wind speed (m/s)  '
    if(stat_cfl    .eq.1) write(50,309) 'cflmax  ','max Courant number            '
    if(stat_cfl    .eq.1) write(50,309) 'kshmax  ','max horiz K stability factor  '
    if(stat_cfl    .eq.1) write(50,309) 'ksvmax  ','max vert K stability factor   '
    if(stat_vort   .eq.1) write(50,309) 'vortsfc ','max vert. vort. at sfc (1/s)  '
    if(stat_vort   .eq.1) write(50,309) 'vort1km ','max vert. vort. at 1 km (1/s) '
    if(stat_vort   .eq.1) write(50,309) 'vort2km ','max vert. vort. at 2 km (1/s) '
    if(stat_vort   .eq.1) write(50,309) 'vort3km ','max vert. vort. at 3 km (1/s) '
    if(stat_vort   .eq.1) write(50,309) 'vort4km ','max vert. vort. at 4 km (1/s) '
    if(stat_vort   .eq.1) write(50,309) 'vort5km ','max vert. vort. at 5 km (1/s) '
    if(stat_tmass  .eq.1) write(50,309) 'tmass   ','total mass of (dry) air       '
    if(stat_tmois  .eq.1) write(50,309) 'tmois   ','total moisture                '
    if(stat_qmass  .eq.1)then
      do n=1,numq
        IF( (n.eq.nqv) .or.                                 &
            (n.ge.nql1.and.n.le.nql2) .or.                  &
            (n.ge.nqs1.and.n.le.nqs2.and.iice.eq.1) )THEN
          text1='mass    '
          text2='total mass of                 '
          write(text1( 5: 7),156) qname(n)
          write(text2(15:17),156) qname(n)
          write(50,309) text1,text2
        ENDIF
      enddo
    endif
    if(stat_tenerg .eq.1) write(50,309) 'ek      ','total kinetic energy          '
    if(stat_tenerg .eq.1) write(50,309) 'ei      ','total internal energy         '
    if(stat_tenerg .eq.1) write(50,309) 'ep      ','total potential energy        '
    if(stat_tenerg .eq.1) write(50,309) 'le      ','total latent energy (sort of) '
    if(stat_tenerg .eq.1) write(50,309) 'et      ','total energy                  '
    if(stat_mo     .eq.1) write(50,309) 'tmu     ','total E-W momentum            '
    if(stat_mo     .eq.1) write(50,309) 'tmv     ','total N-S momentum            '
    if(stat_mo     .eq.1) write(50,309) 'tmw     ','total vertical momentum       '
    if(stat_tmf    .eq.1) write(50,309) 'tmfu    ','total upward mass flux        '
    if(stat_tmf    .eq.1) write(50,309) 'tmfd    ','total downward mass flux      '
    if(stat_pcn    .eq.1)then
      do n=1,nbudget
        text1='        '
        text2='                              '
        write(text1(1:6),158) budname(n)
        write(text2(1:6),158) budname(n)
158     format(a6)
        write(50,309) text1,text2
      enddo
    endif
    if(stat_qsrc   .eq.1)then
      do n=1,numq
        text1='as      '
        text2='artificial source of          '
        write(text1( 3: 5),156) qname(n)
        write(text2(22:24),156) qname(n)
        write(50,309) text1,text2
      enddo
      do n=1,numq
        text1='bs      '
        text2='bndry source/sink of          '
        write(text1( 3: 5),156) qname(n)
        write(text2(22:24),156) qname(n)
        write(50,309) text1,text2
      enddo
    endif
    write(50,310)

301   format('dset ^',a70)
302   format('undef -9999.')
303   format('title ctl file for stats.dat')
304   format('xdef 1 linear 1 1')
305   format('ydef 1 linear 1 1')
306   format('zdef 1 linear 1 1')
307   format('tdef ',i10,' linear 00Z03JUL2000 ',i5,'MN')
308   format('vars ',i6)
309   format(a8,' 1 99 ',a30)
310   format('endvars')

      close(unit=50)

!-----------------------------------------------------------------------
!  Parcel data file:

      if(iprcl.eq.1.and.myid.eq.0)then

        string(totlen+1:totlen+1+12) = '_pdata.ctl  '
        write(outfile,*) string
        open(unit=50,file=string,status='unknown')

        sstring(baselen+1:baselen+1+12) = '_pdata.dat  '

        write(50,401) sstring
        write(50,402)
        write(50,403)
        write(50,404) nparcels
        write(50,405)
        write(50,406)
        write(50,407) 1+int(timax/prclfrq),max(1,int(prclfrq/60.0))
        write(50,408) npvals - 3
        write(50,409) 'x       ','x (m)                         '
        write(50,409) 'y       ','y (m)                         '
        write(50,409) 'z       ','z (m)                         '
        write(50,409) 'qv      ','water vapor mixing ratio      '
        write(50,409) 'qc      ','cloud water mixing ratio      '
        write(50,409) 'qr      ','rain water mixing ratio       '
        write(50,409) 'nm      ','squared Brunt-Vaisala frqncy  '
        write(50,409) 'u       ','u (m/s)                       '
        write(50,409) 'v       ','v (m/s)                       '
        write(50,409) 'w       ','w (m/s)                       '
        write(50,409) 'kh      ','turb. coef. for scalar (m^2/s)'
        write(50,409) 'the     ','theta-e (K)                   '
        write(50,409) 'b       ','buoyancy (m/s^2)              '
        write(50,409) 'dpdz    ','dpdz tendency (m/s^2)         '
        write(50,410)

401     format('dset ^',a70)
402     format('undef -9999.')
403     format('title ctl file for pdata.dat')
404     format('xdef ',i10,' linear 1 1')
405     format('ydef          1 linear 1 1')
406     format('zdef          1 linear 1 1')
407     format('tdef ',i10,' linear 00Z03JUL2000 ',i5,'MN')
408     format('vars ',i6)
409     format(a8,' 1 99 ',a30)
410     format('endvars')

        close(unit=50)

      endif

!-----------------------------------------------------------------------

      ENDIF     ! endif for myid=0

  ENDIF grads_descriptors

      write(outfile,*)

!-----------------------------------------------------------------------

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine writeout(fnum,nwrite,qname,sigma,zh,pi0,prs0,rho0,th0,qv0,u0,v0,  &
                          zs,rain,sws,thflux,qvflux,cdu,cdv,ce,dum1,dum2,     &
                          rho,prs,dbz,ua,dumu,va,dumv,wa,dumw,ppi,tha,        &
                          qa,kmh,kmv,khh,khv,tkea,pta)
      implicit none

      include 'input.incl'
      include 'constants.incl'
#ifdef MPI
      include 'mpif.h'
      integer comm
#endif

      integer :: fnum,nwrite
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
#ifdef MPI
      character*80 sname,uname,vname,wname
#endif

!--------------------------------------------------------------
!  writeout data on scalar-points

#ifdef MPI
#     ifdef MAINC
      call get_registered_comm(comm)
#     else
      comm = MPI_COMM_WORLD
#     endif
#endif

#ifndef MPI
    write(outfile,*)
  IF(output_filetype.eq.1)THEN
    if(s_out.ge.1)then
      if(fnum.eq.51)then
        string(totlen+1:totlen+1+12) = '_s.dat'
      elseif(fnum.eq.71)then
        string(totlen+1:totlen+1+12) = '_i.dat'
      endif
      write(outfile,*) string
      open(unit=fnum,file=string,form='unformatted',access='direct',   &
           recl=(ni*nj*4),status='unknown')
      irec=1+(nwrite-1)*( nrain*output_rain + nrain*output_sws + output_zs + 4*output_sfcflx +   &
                          nk*(s_out-nrain*output_rain-nrain*output_sws-output_zs-4*output_sfcflx) )
    endif
    if(u_out.ge.1)then
      string(totlen+1:totlen+1+12) = '_u.dat'
      write(outfile,*) string
      open(unit=52,file=string,form='unformatted',access='direct',   &
           recl=((ni+1)*nj*4),status='unknown')
    endif
    if(v_out.ge.1)then
      string(totlen+1:totlen+1+12) = '_v.dat'
      write(outfile,*) string
      open(unit=53,file=string,form='unformatted',access='direct',   &
           recl=(ni*(nj+1)*4),status='unknown')
    endif
    if(w_out.ge.1)then
      string(totlen+1:totlen+1+12) = '_w.dat'
      write(outfile,*) string
      open(unit=54,file=string,form='unformatted',access='direct',   &
           recl=(ni*nj*4),status='unknown')
    endif
  ELSEIF(output_filetype.eq.2)THEN
    if(s_out.ge.1)then
      if(fnum.eq.51)then
        string(totlen+1:totlen+1+12) = '_XXXX_s.dat'
      elseif(fnum.eq.71)then
        string(totlen+1:totlen+1+12) = '_XXXX_i.dat'
      endif
      write(string(totlen+2:totlen+5),102) nwrite
102   format(i4.4)
      write(outfile,*) string
      open(unit=fnum,file=string,form='unformatted',access='direct',   &
           recl=(ni*nj*4),status='unknown')
      irec=1
    endif
    if(u_out.ge.1)then
      string(totlen+1:totlen+1+12) = '_XXXX_u.dat'
      write(string(totlen+2:totlen+5),102) nwrite
      write(outfile,*) string
      open(unit=52,file=string,form='unformatted',access='direct',   &
           recl=((ni+1)*nj*4),status='unknown')
    endif
    if(v_out.ge.1)then
      string(totlen+1:totlen+1+12) = '_XXXX_v.dat'
      write(string(totlen+2:totlen+5),102) nwrite
      write(outfile,*) string
      open(unit=53,file=string,form='unformatted',access='direct',   &
           recl=(ni*(nj+1)*4),status='unknown')
    endif
    if(w_out.ge.1)then
      string(totlen+1:totlen+1+12) = '_XXXX_w.dat'
      write(string(totlen+2:totlen+5),102) nwrite
      write(outfile,*) string
      open(unit=54,file=string,form='unformatted',access='direct',   &
           recl=(ni*nj*4),status='unknown')
    endif
  ELSE
    write(outfile,*) '  Invalid option for output_filetype'
    call stopcm1
  ENDIF
#else
      irec=1

      sname = '                                                                                '
      uname = '                                                                                '
      vname = '                                                                                '
      wname = '                                                                                '

    if(strlen.gt.0)then
      sname(1:strlen) = output_path(1:strlen)
      uname(1:strlen) = output_path(1:strlen)
      vname(1:strlen) = output_path(1:strlen)
      wname(1:strlen) = output_path(1:strlen)
    endif

      sname(strlen+1:strlen+baselen) = output_basename(1:baselen)
      uname(strlen+1:strlen+baselen) = output_basename(1:baselen)
      vname(strlen+1:strlen+baselen) = output_basename(1:baselen)
      wname(strlen+1:strlen+baselen) = output_basename(1:baselen)

      if(fnum.eq.51)then
        sname(totlen+1:totlen+1+16) = '_XXXX_YYYY_s.dat'
      elseif(fnum.eq.71)then
        sname(totlen+1:totlen+1+16) = '_XXXX_YYYY_i.dat'
      endif
      uname(totlen+1:totlen+1+16) = '_XXXX_YYYY_u.dat'
      vname(totlen+1:totlen+1+16) = '_XXXX_YYYY_v.dat'
      wname(totlen+1:totlen+1+16) = '_XXXX_YYYY_w.dat'

      write(sname(totlen+2:totlen+ 5),100) myid
      write(sname(totlen+7:totlen+10),100) nwrite

      write(uname(totlen+2:totlen+ 5),100) myid
      write(uname(totlen+7:totlen+10),100) nwrite

      write(vname(totlen+2:totlen+ 5),100) myid
      write(vname(totlen+7:totlen+10),100) nwrite

      write(wname(totlen+2:totlen+ 5),100) myid
      write(wname(totlen+7:totlen+10),100) nwrite

100   format(i4.4)

      write(outfile,*)
      write(outfile,*) '  myid,sname=',myid,'   ',sname
      open(unit=fnum,file=sname,                   &
           form='unformatted',access='direct',   &
           recl=(ni*nj*4),status='unknown')

      if(u_out.ge.1)then
        write(outfile,*) '  myid,uname=',myid,'   ',uname
        open(unit=52,file=uname,                   &
             form='unformatted',access='direct',   &
             recl=((ni+1)*nj*4),status='unknown')
      endif

      if(v_out.ge.1)then
        write(outfile,*) '  myid,vname=',myid,'   ',vname
        open(unit=53,file=vname,                   &
             form='unformatted',access='direct',   &
             recl=(ni*(nj+1)*4),status='unknown')
      endif

      if(w_out.ge.1)then
        write(outfile,*) '  myid,wname=',myid,'   ',wname
        open(unit=54,file=wname,                   &
             form='unformatted',access='direct',   &
             recl=(ni*nj*4),status='unknown')
      endif
#endif

      if(output_rain.eq.1) call write2d(fnum,ni,nj,irec,rain(ib,jb,1))
      if(output_sws .eq.1) call write2d(fnum,ni,nj,irec,sws(ib,jb,1))
      if(nrain.eq.2)then
        if(output_rain.eq.1) call write2d(fnum,ni,nj,irec,rain(ib,jb,2))
        if(output_sws .eq.1) call write2d(fnum,ni,nj,irec,sws(ib,jb,2))
      endif
      if(output_sfcflx.eq.1) call write2d(fnum,ni,nj,irec,thflux)
      if(output_sfcflx.eq.1) call write2d(fnum,ni,nj,irec,qvflux)
      if(output_sfcflx.eq.1)then
!$omp parallel do default(shared)  &
!$omp private(i,j)
        do j=1,nj
        do i=1,ni
          dum1(i,j,1)=0.25*( (cdu(i,j)+cdu(i+1,j))   &
                            +(cdv(i,j)+cdv(i,j+1)) )
        enddo
        enddo
        call write2d(fnum,ni,nj,irec,dum1(ib,jb,1))
      endif
      if(output_sfcflx.eq.1) call write2d(fnum,ni,nj,irec,ce)
      if(output_zs  .eq.1) call write2d(fnum,ni,nj,irec,zs)
      dum1=zh
      if(fnum.eq.71)then
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=(k*dz-0.5*dz)-zs(i,j)
        enddo
        enddo
        enddo
      endif
      if(output_zh  .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
      endif
      dum1=tha
      if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_thpert .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
      dum1=prs
      if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_prs    .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
      endif
      dum1=ppi
      if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_pipert .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
      dum1=rho
      if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
      if(output_rho    .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
      endif
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
          if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
          call write3d(fnum,ni,nj,nk,irec,dum1)
        enddo
      endif
      if(imoist.eq.1)then
        do k=1,nk
        do j=1,nj
        do i=1,ni
          dum1(i,j,k)=qa(i,j,k,nqv)
        enddo
        enddo
        enddo
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        if(output_qv    .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
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
          if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
          call write3d(fnum,ni,nj,nk,irec,dum1)
        endif
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
              if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
              call write3d(fnum,ni,nj,nk,irec,dum1)
            endif
          enddo
        endif
        dum1=dbz
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        if(output_dbz   .eq.1) call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
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
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
      endif

      if(output_basestate.eq.1)then
        dum1=pi0
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
        dum1=th0
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
        dum1=prs0
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
        dum1=qv0
        if(fnum.eq.71) call zinterp(sigma,zs,zh,dum1,dum2)
        call write3d(fnum,ni,nj,nk,irec,dum1)
      endif

!--------------------------------------------------------------
!  writeout data on u-points

      irec=1+(nwrite-1)*nk*u_out
      if(output_filetype.eq.2) irec=1
#ifdef MPI
      irec=1
#endif

      if(output_u    .eq.1) call write3d(52,ni+1,nj,nk,irec,ua)

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
        call write3d(52,ni+1,nj,nk,irec,dumu)
      endif

      if(output_basestate.eq.1) call write3d(52,ni+1,nj,nk,irec,u0)

!--------------------------------------------------------------
!  writeout data on v-points

      irec=1+(nwrite-1)*nk*v_out
      if(output_filetype.eq.2) irec=1
#ifdef MPI
      irec=1
#endif

      if(output_v    .eq.1) call write3d(53,ni,nj+1,nk,irec,va)

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
        call write3d(53,ni,nj+1,nk,irec,dumv)
      endif

      if(output_basestate.eq.1) call write3d(53,ni,nj+1,nk,irec,v0)

!--------------------------------------------------------------
!  writeout data on w-points

      irec=1+(nwrite-1)*(nk+1)*w_out
      if(output_filetype.eq.2) irec=1
#ifdef MPI
      irec=1
#endif

      if(output_w  .eq.1)                call write3d(54,ni,nj,nk+1,irec,wa)
      if(output_tke.eq.1.and.iturb.eq.1) call write3d(54,ni,nj,nk+1,irec,tkea)
      if(output_km .eq.1.and.iturb.ge.1) call write3d(54,ni,nj,nk+1,irec,kmh)
      if(output_km .eq.1.and.iturb.ge.1) call write3d(54,ni,nj,nk+1,irec,kmv)
      if(output_kh .eq.1.and.iturb.ge.1) call write3d(54,ni,nj,nk+1,irec,khh)
      if(output_kh .eq.1.and.iturb.ge.1) call write3d(54,ni,nj,nk+1,irec,khv)

!--------------------------------------------------------------

      write(outfile,*)
      write(outfile,*) 'Done Writing Data to File: nwrite=',nwrite
      write(outfile,*)

      close(unit=fnum)
      if(u_out.ge.1)then
        close(unit=52)
      endif
      if(v_out.ge.1)then
        close(unit=53)
      endif
      if(w_out.ge.1)then
        close(unit=54)
      endif
#ifdef MPI
      call MPI_BARRIER (comm,ierr)
#endif

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine write2d(fileunit,numi,numj,irec,var)
      implicit none

      integer :: fileunit,numi,numj,irec
      real, dimension(-2:numi+3,-2:numj+3) :: var

      integer i,j

!----------------------------------

#ifdef DP
      write(fileunit,rec=irec) ((sngl(var(i,j)),i=1,numi),j=1,numj)
#else
      write(fileunit,rec=irec) ((var(i,j),i=1,numi),j=1,numj)
#endif
      irec=irec+1

!----------------------------------

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine write3d(fileunit,numi,numj,numk,irec,var)
      implicit none

      integer :: fileunit,numi,numj,numk,irec
      real, dimension(-2:numi+3,-2:numj+3,0:numk+1) :: var

      integer i,j,k

!----------------------------------

      do k=1,numk
#ifdef DP
        write(fileunit,rec=irec) ((sngl(var(i,j,k)),i=1,numi),j=1,numj)
#else
        write(fileunit,rec=irec) ((var(i,j,k),i=1,numi),j=1,numj)
#endif
        irec=irec+1
      enddo

!----------------------------------

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine write_restart(nrec,prec,nwrite,nrst,                    &
                               qbudget,asq,bsq,                          &
                               rain,sws,radbcw,radbce,radbcs,radbcn,     &
                               ua,va,wa,ppi,tha,qa,tkea,pta,pdata,rtime)
      implicit none

      include 'input.incl'
      include 'constants.incl'

      integer nrec,prec,nwrite,nrst
      real*8, dimension(nbudget) :: qbudget
      real*8, dimension(numq) :: asq,bsq
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
      real, dimension(jb:je,kb:ke) :: radbcw,radbce
      real, dimension(ib:ie,kb:ke) :: radbcs,radbcn
      real, dimension(ib:ie+1,jb:je,kb:ke) :: ua
      real, dimension(ib:ie,jb:je+1,kb:ke) :: va
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta
      real, dimension(npvals,nparcels) :: pdata
      real rtime

      character*80 fname

!-----------------------------------------------------------------------

      fname = '                                                                                '
    if(strlen.gt.0)then
      fname(1:strlen) = output_path(1:strlen)
    endif
      fname(strlen+1:strlen+baselen) = output_basename(1:baselen)
      fname(totlen+1:totlen+1+18) = '_rst_XXXX_YYYY.dat'

      write(fname(totlen+ 6:totlen+ 9),101) myid
      write(fname(totlen+11:totlen+14),101) nrst
101   format(i4.4)

      write(outfile,*)
      write(outfile,*) '  Writing to restart file!'
      write(outfile,*) '  fname=',fname
      write(outfile,*)

      open(unit=50,file=fname,form='unformatted',status='unknown')

      write(50) nrec
      write(50) prec
      write(50) nwrite
      write(50) nrst
      write(50) rtime
      write(50) qbudget
      write(50) asq
      write(50) bsq
      write(50) rain
      write(50) sws
      write(50) ua
      write(50) va
      write(50) wa
      write(50) ppi
      write(50) tha
      if(imoist.eq.1) write(50) qa
      if(iturb.eq.1) write(50) tkea
      if(iptra.eq.1) write(50) pta
      if(iprcl.eq.1) write(50) pdata
      if(irbc.eq.4.and.ibw.eq.1) write(50) radbcw
      if(irbc.eq.4.and.ibe.eq.1) write(50) radbce
      if(irbc.eq.4.and.ibs.eq.1) write(50) radbcs
      if(irbc.eq.4.and.ibn.eq.1) write(50) radbcn

      close(unit=50)

      return
      end


!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine read_restart(nloop1,nrec,prec,nwrite,nrst,taptim,rsttim,   &
                              qbudget,asq,bsq,                              &
                              rain,sws,radbcw,radbce,radbcs,radbcn,         &
                              ua,va,wa,ppi,tha,qa,tkea,pta,pdata,rtime)
      implicit none

      include 'input.incl'
      include 'constants.incl'

      integer nloop1,nrec,prec,nwrite,nrst
      real*8 taptim,rsttim
      real*8, dimension(nbudget) :: qbudget
      real*8, dimension(numq) :: asq,bsq
      real, dimension(ib:ie,jb:je,nrain) :: rain,sws
      real, dimension(jb:je,kb:ke) :: radbcw,radbce
      real, dimension(ib:ie,kb:ke) :: radbcs,radbcn
      real, dimension(ib:ie+1,jb:je,kb:ke) :: ua
      real, dimension(ib:ie,jb:je+1,kb:ke) :: va
      real, dimension(ib:ie,jb:je,kb:ke+1) :: wa
      real, dimension(ib:ie,jb:je,kb:ke) :: ppi,tha
      real, dimension(ibm:iem,jbm:jem,kbm:kem,numq) :: qa
      real, dimension(ibt:iet,jbt:jet,kbt:ket) :: tkea
      real, dimension(ibp:iep,jbp:jep,kbp:kep,npt) :: pta
      real, dimension(npvals,nparcels) :: pdata
      real rtime

      character*80 fname

!-----------------------------------------------------------------------

      fname = '                                                                                '
    if(strlen.gt.0)then
      fname(1:strlen) = output_path(1:strlen)
    endif
      fname(strlen+1:strlen+baselen) = output_basename(1:baselen)
      fname(totlen+1:totlen+1+18) = '_rst_XXXX_YYYY.dat'

      write(fname(totlen+ 6:totlen+ 9),101) myid
      write(fname(totlen+11:totlen+14),101) rstnum
101   format(i4.4)

      write(outfile,*)
      write(outfile,*) '  Reading from restart file!'
      write(outfile,*) '  fname=',fname
      write(outfile,*)

      open(unit=50,file=fname,form='unformatted',status='old')

      read(50) nrec
      read(50) prec
      read(50) nwrite
      read(50) nrst
      read(50) rtime
      read(50) qbudget
      read(50) asq
      read(50) bsq
      read(50) rain
      read(50) sws
      read(50) ua
      read(50) va
      read(50) wa
      read(50) ppi
      read(50) tha
      if(imoist.eq.1) read(50) qa
      if(iturb.eq.1) read(50) tkea
      if(iptra.eq.1) read(50) pta
      if(iprcl.eq.1) read(50) pdata
      if(irbc.eq.4.and.ibw.eq.1) read(50) radbcw
      if(irbc.eq.4.and.ibe.eq.1) read(50) radbce
      if(irbc.eq.4.and.ibs.eq.1) read(50) radbcs
      if(irbc.eq.4.and.ibn.eq.1) read(50) radbcn

      close(unit=50)

!---------

      nloop1=nint(rtime/dtl)+1
      taptim=rtime+tapfrq
      rsttim=rtime+rstfrq
      prcltim=rtime+prclfrq
      if( output_format.eq.2 )then
        nrec=nrec-1
      else
        nrec=nrec-stat_out
      endif

      write(outfile,*) nloop1,nrec,taptim,rsttim
      write(outfile,*)

!---------

      return
      end


