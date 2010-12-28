#ifndef SBS_CONTROLLER_H
#define SBS_CONTROLLER_H

#include "mpi.h"
#include "sbs_shmem.h"

/* this is the only function of a controller, that takes
 a shared segment descriptor and a ptr to a newly received cmd
 and process it */
int sbs_controller_process(MPI_Comm comm, sbs_shmem segment, struct sbs_cmd_s* cmd);

#endif
