#ifdef _ADIOS



#ifndef SBS_WRITER_H

#define SBS_WRITER_H



#include "mpi.h"

#include "sbs_variable.h"

int sbs_writer_adios_init(MPI_Comm comm, char* config);

int sbs_writer_adios_finalize(MPI_Comm comm);

int sbs_write_adios(MPI_Comm comm, sbs_variable v, char* filename);

#endif



#endif

