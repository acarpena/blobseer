CC = mpcc_r
CPP = /lib/cpp -P
AR = ar -X64

CCFLAG = -q64 -O2 -DH5_USE_16_API -D_HDF5 -D_DEBUG -DHAVE_FORTRAN -D_LOCAL
INCDIR = -I. -I/opt/hdf5-1.8.3-serial64/include -I/opt/szip64/include
LIBDIR = -L. -L/opt/hdf5-1.8.3-serial64/lib -L/opt/szip64/lib
LIB = -lpthread -lhdf5 -lsz -lz -lm -lgfortran

OBJS = 		sbs_common.o		\
		sbs_filter.o		\
		sbs_config.o		\
		sbs_writer_adios.o	\
		sbs_writer_phdf5.o	\
		sbs_writer_hdf5.o	\
		sbs_buffer.o		\
		sbs_shmem.o		\
		sbs_cmdq.o		\
		sbs_mailbox.o		\
		sbs_server.o		\
		sbs_clientf.o		\
		sbs_client.o		\
		sbs_controller.o	\
		sbs_variable.o		\
		sbs_types.o


all: lib

lib: $(OBJS)
	$(AR) -r -c -s libsbs.a $(OBJS)

%.o: %.c %.h
	$(CC) $(CCFLAG) -c $< $(INCDIR) -o $@

clean:
	rm -f *.o *.a
