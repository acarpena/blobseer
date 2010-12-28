
#SHELL = /bin/sh


#-----------------------------------------------------------------------------
#                      NETCDF SECTION
#  (uncomment the following four lines if you want netcdf output capability)
#   (also, make sure the paths to netcdf files are correct for your machine)
#              (NOTE: Don't change lines 3 and 4!)
#
#OUTPUTINC = -I/usr/local/netcdf/include
#OUTPUTLIB = -L/usr/local/netcdf/lib
#OUTPUTOPT = -DNETCDF
#LINKOPTS  = -lnetcdf
#-----------------------------------------------------------------------------
#                         HDF SECTION
#  (uncomment the following four lines if you want hdf output capability)
#   (also, make sure the paths to hdf files are correct for your machine)
#              (NOTE: Don't change lines 3 and 4!)
# Note: szip may or may not be compiled into HDF4. You may need to
# remove -lsz. CM1 uses gzip compression.
#
#OUTPUTINC = -I/usr/local/hdf/include
#OUTPUTLIB = -L/usr/local/hdf/lib
#OUTPUTOPT = -DHDFOUT
#LINKOPTS  = -lmfhdf -ldf -ljpeg -lz -lsz
#-----------------------------------------------------------------------------

OUTPUTOPT = -DSBSOUT
OUTPUTINC = -I/opt/hdf5-1.8.3-serial64/include -I/opt/szip64/include
OUTPUTLIB = -L/opt/hdf5-1.8.3-serial64/lib -L/opt/szip64/lib
FC   = mpxlf_r
CC   = mpcc_r
OPTS = -lxlf -lm -qfree=f90 -O3 -qstrict -qnozerosize -qmaxmem=-1 -q64 -I../include -I../../src -L../../src -lsbs -lsz -lz -lhdf5
CPP  = cpp -C -P
DHCL = -DHCL
DM   = -DMPI
DMC  = -DMAINC
#F2C  = -lgfortran
#-----------------------------------------------------------------------------
#                     HARDWARE SECTION
#-- Choose the appropriate architecture, and uncomment all lines 
#-- in that section.  See "README.compile" for more information.
#-----------------------------------------------------------------------------
#  Linux, single processor, Portland Group compiler
#FC   = pgf90
#OPTS = -Mfree -I../include -pc 64 -Kieee -Ktrap=none -O2
#CPP  = cpp -C -P -traditional
#-----------------------------------------------------------------------------
#  Linux, OpenMP shared memory, Portland Group compiler
#FC   = pgf90
#OPTS = -Mfree -I../include -pc 64 -Kieee -Ktrap=none -O2 -mp
#CPP  = cpp -C -P -traditional
#-----------------------------------------------------------------------------
#  Linux, single processor, Intel fortran compiler
#FC   = ifort
#OPTS = -I../include -O2 -FR -assume byterecl
#CPP  = cpp -C -P -traditional
#-----------------------------------------------------------------------------
#  Linux, distributed memory, Portland Group compiler (NCAR's lightning)
#FC   = mpif90
#OPTS = -Mfree -I../include -pc 64 -Kieee -Ktrap=none -O2
#CPP  = cpp -C -P -traditional
#DM   = -DMPI
#-----------------------------------------------------------------------------
#  Linux, distributed memory, Intel compiler (eg, SHARCNET's saw)
#FC   = mpif90
#OPTS = -I../include -O2 -ftz
#CPP  = cpp -C -P -traditional
#DM   = -DMPI
#-----------------------------------------------------------------------------
#  Linux, distributed memory, PathScale compiler (eg, SHARCNET's whale)
#FC   = mpif90
#OPTS = -freeform -I../include -O2 -OPT:Olimit=0
#CPP  = cpp -C -P -traditional
#DM   = -DMPI
#-----------------------------------------------------------------------------
#  Linux, single processor, Portland Group compiler, double precision
#FC   = pgf90
#OPTS = -Mfree -I../include -pc 64 -Kieee -Ktrap=none -O2 -r8
#CPP  = cpp -C -P -traditional
#DP   = -DDP
#-----------------------------------------------------------------------------
#  Linux, OpenMP shared memory, Portland Group compiler, double precision
#FC   = pgf90
#OPTS = -Mfree -I../include -pc 64 -Kieee -Ktrap=none -O2 -r8 -mp
#CPP  = cpp -C -P -traditional
#DP   = -DDP
#-----------------------------------------------------------------------------
#  Linux and Mac, using g95 compiler
#FC   = g95
#OPTS = -ffree-form -I../include -O2
#CPP  = cpp -C -P -traditional
#-----------------------------------------------------------------------------
#  IBM POWER6, single processor
#      (e.g., NCAR's bluefire)
#      NOTE: on bluefire, use 'gmake'
#FC   = xlf_r
#OPTS = -qfree=f90 -I../include -O3 -qstrict -qnozerosize -qmaxmem=-1 -q64
#CPP  = /lib/cpp -C -P
#-----------------------------------------------------------------------------
#  IBM POWER6, multiple processors, shared memory using OpenMP
#      (e.g., NCAR's bluefire)
#      NOTE: on bluefire, use 'gmake'
#FC   = xlf_r
#OPTS = -qfree=f90 -I../include -O3 -qstrict -qnozerosize -qmaxmem=-1 -q64 -qsmp=noauto
#CPP  = /lib/cpp -C -P
#-----------------------------------------------------------------------------
#  IBM POWER6, multiple processors, distributed memory using MPI
#      (e.g., NCAR's bluefire)
#      NOTE: on bluefire, use 'gmake'
#FC   = mpxlf_r
#OPTS = -qfree=f90 -I../include -O3 -qstrict -qnozerosize -qmaxmem=-1 -q64
#CPP  = /lib/cpp -C -P
#DM   = -DMPI
#-----------------------------------------------------------------------------
#  IBM POWER6, multiple processors, hybrid ... both OpenMP and MPI
#      (e.g., NCAR's bluefire)
#      NOTE: on bluefire, use 'gmake'
#FC   = mpxlf_r
#OPTS = -qfree=f90 -I../include -O3 -qstrict -qnozerosize -qmaxmem=-1 -q64 -qsmp=noauto
#CPP  = /lib/cpp -C -P
#DM   = -DMPI
#-----------------------------------------------------------------------------
#  SGI, distributed memory (NCSA's cobalt)
#FC   = ifort
#OPTS = -lmpi -O2 -FR -assume byterecl -ftz -I../include -I/usr/apps/hdf/include
#CPP  = cpp -C -P -traditional
#DM   = -DMPI
#-----------------------------------------------------------------------------
#-- You shouldn't need to change anything below here
#-----------------------------------------------------------------------------

SRC   = adv5.F \
	adv6.F \
	anelp.F \
	base.F \
	bc.F \
	cm1.F \
	comm.F \
	diff2.F \
	diff6.F \
	goddard.F \
	init3d.F \
	integ.F \
	kessler.F \
	lfoice.F \
	maxmin.F \
	misclibs.F \
	morrison.F \
	param.F \
	parcel.F \
	poiss.F \
	rdamp.F \
	sfcphys.F \
	singleton.F \
	solve.F \
	sounde.F \
	sound.F \
	soundns.F \
	statpack.F \
        thompson.F \
	turbtke.F \
	weno.F \
	writeout.F \
	writeout_nc.F \
	writeout_hdf.F \
	writeout_sbs.F

OBJS = $(addsuffix .o, $(basename $(SRC)))

FFLAGS  =  $(OPTS)
AR      = ar cru

.SUFFIXES:
.SUFFIXES:      .F .f90 .o

all : cm1

cm1:			$(OBJS)
			$(FC) $(OBJS) $(FFLAGS) $(OUTPUTINC) $(OUTPUTLIB) $(LINKOPTS) -o ../run/cm1.exe
			$(AR) onefile.F $(SRC)
			mv onefile.F ../run

exc:			$(OBJS)
			$(CC) -c cm1c.c $(DM) $(DMC) $(OPTS)
			$(FC) $(OBJS) cm1c.o $(OUTPUTINC) $(OPTS) $(OUTPUTLIB) $(LINKOPTS) $(DMC) $(F2C)  -o ../run/cm1c

.F.o:
			$(CPP) $*.F $(DM) $(DHCL) $(DMC) $(DP) $(OUTPUTOPT) > $*.f90
			$(FC) $(FFLAGS) $(OUTPUTINC) -c $*.f90

code:
			$(AR) onefile.F $(SRC)
			mv onefile.F ../run

clean:
			rm -f *.f90 *.o *.a *.mod

# DEPENDENCIES : only dependencies after this line (don't remove the word DEPENDENCIES)

poiss.o: singleton.o
param.o: thompson.o
param.o: morrison.o
solve.o: thompson.o
solve.o: morrison.o
