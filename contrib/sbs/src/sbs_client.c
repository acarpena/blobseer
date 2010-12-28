#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sbs_common.h"
#include "sbs_types.h"
#include "sbs_variable.h"
#include "sbs_client.h"
/* this is a local function that creates the client */
static sbs_client sbs_client_init(int tid)
{
	sbs_client handler = NULL;
	handler = (sbs_client)malloc(sizeof(struct sbs_client_s));
	handler->shseg = (sbs_shmem)malloc(sizeof(struct sbs_shmem_s));

	// get client id
	handler->client_id = tid;
	handler->time = -1;
	handler->shseg = sbs_shmem_init();
	/* the mailbox and cmdq are already created in the 
	 shared segment, we only have to map it into the 
	 right structure */
	handler->cmdq = sbs_shmem_map_cmdq(handler->shseg, 
					   handler->client_id);
	handler->mailbox = sbs_shmem_get_mailbox(handler->shseg,
						 handler->client_id);
	return handler;
}

/* initialize a new client with a given thread id */
sbs_client sbs_initialize(int tid)
{
	LOG(tid,"initializing client\n");
	sbs_client handler;
	handler = sbs_client_init(tid);
	LOG(tid,"client initialized!\n");
	return handler;
}

/* start a new write-phase */
uint64_t sbs_start_step(sbs_client handler)
{
	LOG(handler->client_id,"starting new step\n");
	handler->time = handler->time + 1;
	struct sbs_cmd_s cmd = {CMD_STEP,
				handler->client_id,
				(uint64_t)(handler->time),0};
	sbs_cmdq_push(handler->cmdq,cmd);
	return (uint64_t)(handler->time);
}

/* end the current write-phase */
uint64_t sbs_end_step(sbs_client handler)
{
	LOG(handler->client_id,"ending step\n");
	return handler->time;
}

/* write a variable in mailbox and send cmd to server */
int sbs_write(sbs_client handler, char* varname, void* data, int type, 
	      int dim, uint64_t* offsets, size_t* lsizes)
{
	LOG(handler->client_id,"start writing var\n");
	sbs_variable v = (sbs_variable)malloc(sizeof(struct sbs_variable_s));
	
	v->name    = varname;
	v->type    = type;
	v->time    = handler->time;
	v->dim     = dim;
	v->offsets = offsets;
	v->lsizes  = lsizes;
	v->gsizes  = lsizes; // client doesn't know the global sizes

	// compute the required buffer size for metadata
	size_t metadata_size;
	// compute the required buffer size for data
	size_t data_size = sbs_sizeof(type);
	int i;
	for(i=0;i<dim;i++) data_size *= lsizes[i];
	
	// allocate the buffer for metadata
	char* mdbuf = sbs_variable_serialize(v,&metadata_size);
	
	// compute the total size required
	size_t total_size = data_size + metadata_size;
	DEBUG(printf("   size of metadata : %d\n",metadata_size));
	DEBUG(printf("   data_size : %d\n",data_size));
	DEBUG(printf("   total_size : %d\n",total_size));
	sbs_mailbox mb = handler->mailbox;
	// check if there is enough space in the mailbox
	if(sbs_mailbox_wait_available(mb, total_size) == -1) return -1;
	
	// write data into shared memory
	uint64_t offset;
	sbs_mailbox_write(mb,mdbuf,metadata_size,&offset);
	sbs_mailbox_write(mb,data,data_size,NULL);

	// send a command to sbs server to process the data
	struct sbs_cmd_s cmd = 
		{CMD_WRITE, handler->client_id, offset, total_size };
	sbs_cmdq_push(handler->cmdq, cmd);
	LOG(handler->client_id,"end writing var\n");
	return 0;
}

/* free the client, and shutdown server */
int sbs_finalize(sbs_client handler)
{
	LOG(handler->client_id,"client finalize\n");
	if(handler->client_id == 0)
	{
		struct sbs_cmd_s cmd = {CMD_EXIT, 0,0,0};
		sbs_cmdq_push(handler->cmdq, cmd);
	}
	free(handler);
	return 0;
} 
