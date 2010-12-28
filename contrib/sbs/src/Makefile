CC = mpicc
#CCFLAG = -O2 -Wall -D_HDF5 -D_LOCAL -D_DEBUG -D H5_USE_16_API
CCFLAG = -O2 -Wall -D_HDF5 -DHAVE_FORTRAN -D_LOCAL -D H5_USE_16_API
INCDIR = -I. -I$(HOME)/deploy-mpi/include
LIBDIR = -L. -L$(HOME)/deploy-mpi/lib
INC = 
LIB = -lpthread -lhdf5 -lsz -lz -lm -lgfortran #-ladios -lmxml

all: example

example: example.c sbs_common.o sbs_filter.o sbs_config.o sbs_writer_adios.o sbs_writer_phdf5.o sbs_writer_hdf5.o sbs_buffer.o sbs_shmem.o sbs_cmdq.o sbs_mailbox.o sbs_server.o sbs_clientf.o sbs_client.o sbs_controller.o sbs_variable.o sbs_types.o
	$(CC) $(CCFLAG) -o ex example.c sbs_common.o sbs_shmem.o sbs_cmdq.o sbs_mailbox.o sbs_server.o sbs_client.o sbs_filter.o sbs_controller.c sbs_variable.o sbs_types.o sbs_writer_adios.o sbs_writer_phdf5.o sbs_writer_hdf5.o sbs_buffer.o sbs_config.o $(INCDIR) $(INC) $(LIBDIR) $(LIB)

examplef: examplef.c example.F sbs_common.o sbs_filter.o sbs_config.o sbs_writer_hdf5.o sbs_buffer.o sbs_shmem.o sbs_cmdq.o sbs_mailbox.o sbs_server.o sbs_clientf.o sbs_client.o sbs_controller.o sbs_variable.o sbs_types.o
	mpif90 -c example.F
	$(CC) $(CCFLAG) -o exf examplef.c example.o sbs_common.o sbs_shmem.o sbs_cmdq.o sbs_mailbox.o sbs_server.o sbs_client.o sbs_clientf.o sbs_filter.o sbs_controller.c sbs_variable.o sbs_types.o sbs_writer_hdf5.o sbs_buffer.o sbs_config.o $(INCDIR) $(INC) $(LIBDIR) $(LIB)

lib: sbs_common.o sbs_filter.o sbs_config.o sbs_writer_hdf5.o sbs_buffer.o sbs_shmem.o sbs_cmdq.o sbs_mailbox.o sbs_server.o sbs_clientf.o sbs_client.o sbs_controller.o sbs_variable.o sbs_types.o
	ar rcs libsbs.a sbs_common.o sbs_filter.o sbs_config.o sbs_writer_hdf5.o sbs_buffer.o sbs_shmem.o sbs_cmdq.o sbs_mailbox.o sbs_server.o sbs_clientf.o sbs_client.o sbs_controller.o sbs_variable.o sbs_types.o

example2: example2.c sbs_common.o sbs_writer.o sbs_variable.o sbs_types.o
	$(CC) $(CCFLAG) -o ex2 example2.c sbs_common.o sbs_variable.o sbs_types.o sbs_writer_hdf5.o sbs_writer_adios.o $(INCDIR) $(INC) $(LIBDIR) $(LIB)

test_var: test_var.c variable
	$(CC) $(CCFLAG) -o test_var test_var.c sbs_variable.o  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

test_filter: test_filter.c variable filter types
	$(CC) $(CCFLAG) -o test_filter test_filter.c sbs_variable.o sbs_filter.c sbs_types.o

sbs_config.o: sbs_config.c sbs_config.h
	$(CC) $(CCFLAG) -c sbs_config.c $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_types.o: sbs_types.c sbs_types.h
	$(CC) $(CCFLAG) -c sbs_types.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_filter.o: sbs_filter.c sbs_filter.h
	$(CC) $(CCFLAG) -c sbs_filter.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_variable.o: sbs_variable.c sbs_variable.h
	$(CC) $(CCFLAG) -c sbs_variable.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_mailbox.o: sbs_mailbox.c sbs_mailbox.h
	$(CC) $(CCFLAG) -c sbs_mailbox.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_cmdq.o: sbs_cmdq.c sbs_cmdq.h
	$(CC) $(CCFLAG) -c sbs_cmdq.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_controller.o: sbs_controller.c sbs_controller.h
	$(CC) $(CCFLAG) -c sbs_controller.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_server.o: sbs_server.c sbs_server.h
	$(CC) $(CCFLAG) -c sbs_server.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_client.o: sbs_client.c sbs_client.h
	$(CC) $(CCFLAG) -c sbs_client.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_clientf.o: sbs_clientf.c sbs_client.h
	$(CC) $(CCFLAG) -c sbs_clientf.c $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_shmem.o: sbs_shmem.c sbs_shmem.h
	$(CC) $(CCFLAG) -c sbs_shmem.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_mpi_cmd.o: sbs_mpi_cmd.c sbs_mpi_cmd.h
	$(CC) $(CCFLAG) -c sbs_mpi_cmd.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_writer_adios.o: sbs_writer_adios.c sbs_writer_adios.h
	$(CC) $(CCFLAG) -c sbs_writer_adios.c  $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_writer_hdf5.o: sbs_writer_hdf5.c sbs_writer_hdf5.h
	$(CC) $(CCFLAG) -c sbs_writer_hdf5.c $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_writer_phdf5.o: sbs_writer_phdf5.c sbs_writer_phdf5.h
	$(CC) $(CCFLAG) -c sbs_writer_phdf5.c $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_buffer.o: sbs_buffer.c sbs_buffer.h
	$(CC) $(CCFLAG) -c sbs_buffer.c $(INCDIR) $(INC) $(LIBDIR) $(LIB)

sbs_common.o: sbs_common.c sbs_common.h
	$(CC) $(CCFLAG) -c sbs_common.c $(INCDIR) $(INC) $(LIBDIR) $(LIB)

clean:
	rm -f *.o
