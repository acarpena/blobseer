CC = /Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/bin/mpicc
F77 = /Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/bin/mpif77
INCLUDE_DIR = 
# because := is not universally avalible, we have to play games to use the
# user-specificed LDFLAGS and OUR_LIBS env. variables (if set)
OUR_LIBS =      ${LDFLAGS} ${LIBS}
USER_CFLAGS =  -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/openpa/src -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/openpa/src -DUSE_PROCESS_LOCKS  -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/datatype -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/datatype -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/utils/monitor -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/utils/monitor -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/util/wrappers -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/util/wrappers  $(INCLUDE_DIR)
USER_FFLAGS =  -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/openpa/src -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/openpa/src -DUSE_PROCESS_LOCKS  -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/datatype -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/datatype -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/include -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/utils/monitor -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/ch3/channels/nemesis/nemesis/utils/monitor -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpid/common/locks -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/util/wrappers -I/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/util/wrappers  $(INCLUDE_DIR)
CTESTS = file_realms_test io_bounds_test heap_test
FTESTS =  
srcdir=/Users/trungtv/irisawork/hdf5-blobseer/mpich2-1.2.1p1/src/mpi/romio/test-internal

VPATH = .:${srcdir}


all: c-test

c-test: $(CTESTS)

default: all

.c.o:
	$(CC) $(USER_CFLAGS) -c $<

.f.o:
	$(F77) $(USER_FFLAGS) -c $<

.o:
	$(CC) $(USER_CFLAGS) -o  $@ $< $(OUR_LIBS)

clean: 
	rm -f .P* PI* *.o 
	rm -f $(CTESTS) $(FTESTS)
	@-rm -f work.pc work.pcl

file_realms_test: file_realms_test.o

io_bounds_test: io_bounds_test.o

heap_test: heap_test.o
#
# The Fortran tests must be built with the Fortran linker
#testing: 
#	-./runtests $(TESTARGS)

cleanall:
	-@rm -f .P* PI*
