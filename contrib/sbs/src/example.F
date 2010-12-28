        subroutine example(handler,comm)
        include "mpif.h"
       
        integer comm, ierr
        integer*8 handler
        integer i,j,k
        integer*8 step
        integer mydim, mytype
        real, dimension(20,20,10) :: mydata
        integer*8, dimension(3) :: offsets, lsizes
        character varname*11
        
        offsets(1) = 0
        offsets(2) = 0
        offsets(3) = 0
    
        lsizes(1) = 20
        lsizes(2) = 20
        lsizes(3) = 10
    
        do i = 1,20
        do j = 1,20
        do k = 1,10
          mydata(i,j,k) = 1.5
        end do
        end do
        end do
    
        varname = 'temperature'
     
        do i = 1,3
          call sbs_start_step(handler,step)
          call sbs_write(handler,varname,mydata,4,3,offsets,lsizes,ierr)
          call sbs_end_step(handler,step)
          call sleep(3)
       end do
    
       end
