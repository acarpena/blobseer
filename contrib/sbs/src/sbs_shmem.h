#ifndef SBS_SHMEM_H
#define SBS_SHMEM_H

#include "sbs_mailbox.h"
#include "sbs_cmdq.h"

typedef struct sbs_shmem_s {
	char* data;
	int sem_id;
	int mem_id;
} *sbs_shmem;

/* create a shared memory segment, must be called by
 the server */
sbs_shmem sbs_shmem_create();
/* initialize a memory segment from an already created one,
 must be called by the client */
sbs_shmem sbs_shmem_init();
/* create a cmdq associated with a segment */
sbs_cmdq sbs_shmem_get_cmdq(sbs_shmem segment, int i);
/* map a cmda from a segment */
sbs_cmdq sbs_shmem_map_cmdq(sbs_shmem segment, int i);
/* create a mailbox associated with a segment */
sbs_mailbox sbs_shmem_get_mailbox(sbs_shmem segment, int i);
/* map a mailbox from a segment */
sbs_mailbox sbs_shmem_map_mailbox(sbs_shmem segment, int i);
/* delete a shared memory segment */
int sbs_shmem_delete(sbs_shmem shmem);

#endif
