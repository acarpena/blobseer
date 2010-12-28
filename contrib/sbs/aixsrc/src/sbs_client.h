#ifndef SBS_CLIENT_H

#define SBS_CLIENT_H



#include "mpi.h"

#include "sbs_common.h"

#include "sbs_shmem.h"

#include "sbs_mailbox.h"

#include "sbs_cmd.h"

#include "sbs_cmdq.h"



/* this describe a client */

typedef struct sbs_client_s {

 	int client_id; /* uniq id among a given processor */

	uint64_t time; /* current iteration of the simulation */

	sbs_shmem shseg; /* attached shared segment */

	sbs_mailbox mailbox; /* ptr to the shared mailbox */

	sbs_cmdq cmdq; /* ptr to the shared cmdq */

} *sbs_client;



/* initialize a new client with a given thread id */

sbs_client sbs_initialize(int tid);

/* start a new write-phase */

uint64_t sbs_start_step(sbs_client handler);

/* end the current write-phase */

uint64_t sbs_end_step(sbs_client handler);

/* write a variable in mailbox and send cmd to server */

int sbs_write(sbs_client handler, char* varname,void* data,int type, 

	      int dim, uint64_t* offsets, size_t* lsizes);

/* free the client, and shutdown server */

int sbs_finalize(sbs_client handler);

#endif

