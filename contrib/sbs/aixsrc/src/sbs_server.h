#ifndef SBS_SERVER_H

#define SBS_SERVER_H



#include "mpi.h"



/* this is the main loop of a sbs server,

 taking a communicator gathering other sbs servers */

void sbs_server_main(MPI_Comm sbsc);



#endif

