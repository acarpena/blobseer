#include <stdlib.h>

#include <unistd.h>

#include <stdio.h>

#include "mpi.h"

#include "sbs_common.h"

#include "sbs_shmem.h"

#include "sbs_cmdq.h"

#include "sbs_mailbox.h"

#include "sbs_server.h"

#include "sbs_buffer.h"

#include "sbs_controller.h"

#include "sbs_config.h"



#ifdef _ADIOS

#	include "sbs_writer_adios.h"

#endif

#ifdef _HDF5

#	include "sbs_writer_hdf5.h"

#endif

#ifdef _PHDF5

#	include "sbs_writer_phdf5.h"

#endif



sbs_shmem segment; /* the shared segment is global */

sbs_cmdq  cmdq; /* the cmdq is also global */



int sbs_server_process_from_sbss(int cmd, MPI_Status* status, MPI_Comm comm);

struct sbs_cmd_s* get_next_cmd(sbs_cmdq q);



/* this is the main loop of a sbs server,

 taking a communicator gathering other sbs servers */

void sbs_server_main(MPI_Comm sbsc) 

{

	DEBUG(printf("starting server main loop\n");)

	int need_exit = 0;

	int sbsc_cmd; 
	sbs_buffer buffer;



	
	segment = sbs_shmem_create();

	cmdq = sbs_shmem_get_cmdq(segment,0);	

	

	/* this is for non-blocking MPI receive */

	MPI_Request sbsc_req = MPI_REQUEST_NULL;

	

	/* initialize the buffer */

	buffer = sbs_buffer_create();

	

	int myrank = 0;

	if(sbsc != MPI_COMM_NULL)

		MPI_Comm_rank(sbsc,&myrank);

	/* initializing buffer */

	sbs_config_initialize(myrank,buffer);

#ifdef _ADIOS

	/* initializing adios */

	sbs_writer_adios_init(sbsc,"config.xml");

#endif

#ifdef _HDF5

	sbs_writer_hdf5_init(sbsc);

#endif

#ifdef _PHDF5

	sbs_writer_phdf5_init(sbsc);

#endif



	
	

	
	while(!need_exit) {

		
		
		if(sbsc_req == MPI_REQUEST_NULL

		   && !(sbsc == MPI_COMM_NULL))			

			MPI_Irecv(&sbsc_cmd,1,MPI_INT,

			          MPI_ANY_SOURCE,0,sbsc,&sbsc_req);

		int flag;

		int r = 0;

		MPI_Status status;	

		
		
		if(sbsc_req != MPI_REQUEST_NULL)

			MPI_Test(&sbsc_req, &flag, &status);

		else flag = 0;

		
		if(flag) r = sbs_server_process_from_sbss(sbsc_cmd,

			                                  &status,sbsc);

		need_exit = need_exit || (r != 0);



		
		struct sbs_cmd_s* cmd = get_next_cmd(cmdq);

		if(cmd != NULL)

		{

			DEBUG(printf("cmd received\n");)

			r = sbs_controller_process(sbsc,segment,cmd);

			free(cmd);

		}

		need_exit = need_exit || (r != 0);	

	}

	

	
	sbs_shmem_delete(segment);

	sbs_cmdq_delete(cmdq);



#ifdef _ADIOS

	sbs_writer_adios_finalize(sbsc);

#endif

#ifdef _HDF5

	sbs_writer_hdf5_finalize(sbsc);

#endif

#ifdef _PHDF5

	sbs_writer_phdf5_finalize(sbsc);

#endif



	DEBUG(printf("exiting server main loop\n");)

}



/* read next command from the shared cmdq */

struct sbs_cmd_s* get_next_cmd(sbs_cmdq q)

{

	static int mode;

	int size = sbs_cmdq_getsize(q);

	if(size > 0)

	{

		struct sbs_cmd_s* cmd = (struct sbs_cmd_s*)

				malloc(sizeof(struct sbs_cmd_s));

		sbs_cmdq_pop(q,cmd);

		if(mode == 0) {

			double t = MPI_Wtime();

			printf("[timer] %lf processing commands\n",t);

			mode = 1;

		}

		return cmd;

	}

	else

	{

		if(mode == 1) {

			double t = MPI_Wtime();

			printf("[timer] %lf no more command\n",t);

			mode = 0;

		}

		return NULL;

	}

}



/* this process a cmd from another MPI sbs instance */

int sbs_server_process_from_sbss(int cmd, MPI_Status *status, MPI_Comm comm) 

{

	
	
	
	DEBUG(printf("processing cmd from another sbss instance\n");)

	printf("sbss : server received cmd %d from rank %d\n",

		cmd,status->MPI_SOURCE);

	return 0;

}

