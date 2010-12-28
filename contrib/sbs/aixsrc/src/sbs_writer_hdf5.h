#ifdef _HDF5



#ifndef SBS_WRITER_H

#define SBS_WRITER_H



#include "mpi.h"

#include "sbs_variable.h"

int sbs_writer_hdf5_init(MPI_Comm comm);

int sbs_writer_hdf5_finalize(MPI_Comm comm);

int sbs_writer_hdf5_start_step(int step, int rank);

int sbs_writer_hdf5_end_step(int step);

int sbs_write_hdf5(MPI_Comm comm, sbs_variable v);

#endif



#endif

