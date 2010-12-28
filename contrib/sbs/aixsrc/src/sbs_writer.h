#ifndef SBS_WRITER_H

#define SBS_WRITER_H



#include "mpi.h"

#include "sbs_variable.h"

int sbs_writer_hdf5_init(MPI_Comm comm, char* config);

int sbs_writer_hdf5_finalize(MPI_Comm comm);

int sbs_write_hdf5(MPI_Comm comm, sbs_variable v, char* filename);

#endif

