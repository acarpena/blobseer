#ifdef _PHDF5

#ifndef SBS_WRITER_H
#define SBS_WRITER_H

#include "mpi.h"
#include "sbs_variable.h"
int sbs_writer_phdf5_init(MPI_Comm comm);
int sbs_writer_phdf5_finalize(MPI_Comm comm);
int sbs_write_phdf5(sbs_variable v, char* filename);
int sbs_writer_phdf5_start_step(int step);
int sbs_writer_phdf5_end_step(int step);
#endif

#endif
