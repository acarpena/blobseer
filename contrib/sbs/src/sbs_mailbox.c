#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "sbs_common.h"
#include "sbs_mailbox.h"
#include "sbs_shmem.h"

/* ================================================================= */
// -----------------------------------------------------------------
// this function has moved in sbs_shmem.c as a static function
/*sbs_mailbox sbs_mailbox_init(int client_id, sbs_shmem segment)
{	
	LOG("initializing mailbox...\n");
	sbs_mailbox mb = NULL;

	// create mailbox structure in memory
	mb = (sbs_mailbox)malloc(sizeof(struct sbs_mailbox_s));

	// get semaphores
	mb->sem_id = segment->sem_id;
	mb->client_id = client_id;
	
	// attach the segment
	uint64_t offset = SBS_CLIENT_NBR*sizeof(struct sbs_cmdq_data_s)
			  + client_id*sizeof(struct sbs_shared_mb_contents_s);
	mb->shared_contents = 
		(struct sbs_shared_mb_contents_s*)
		(&(segment->data[offset]));
		
	LOG("mailbox initialized!\n");
	return mb;
}*/

// -----------------------------------------------------------------
// delete a mailbox, free all the semaphores and shared memory
int sbs_mailbox_delete(sbs_mailbox mb)
{
	DEBUG(printf("deleting mailbox\n");)
	free(mb);
	return 0 ;
}
// -----------------------------------------------------------------
// block until "size" bytes are free in the mailbox
int sbs_mailbox_wait_available(sbs_mailbox mb, size_t size)
{	
	DEBUG(printf("requesting size in mailbox\n");)
	/*
	//printf("[time] %lf : requesting size %ld\n",gettime(),(long)size);
	struct sembuf op = {mb->client_id*SEM_PER_CLIENT+3,-size,0};
	//printf("[state] %lf start waiting\n",gettime());
	if(size > SBS_MAILBOX_SIZE)
	{
		printf("unable to satisfy size request\n");
		return -1;
	}
	
	semop(mb->sem_id, &op, 1);
	//printf("[time] %lf : size request accepted\n",gettime());
	//printf("[state] %lf end waiting\n",gettime());
	*/
	int ok = 0;
	struct sembuf p1 = {mb->client_id*SEM_PER_CLIENT+3,-1,0};
	struct sembuf v1 = {mb->client_id*SEM_PER_CLIENT+3,1,0};
	struct sembuf signal = {mb->client_id*SEM_PER_CLIENT+4,-1,0};

	if(size > SBS_MAILBOX_SIZE)
	{
		printf("unable to satisfy size request\n");
		return -1;
	}
	
        do
	{
		semop(mb->sem_id, &p1, 1);
		uint64_t freespace = 
			SBS_MAILBOX_SIZE - mb->shared_contents->offset;
		if(freespace >= size)
		{
			semop(mb->sem_id, &v1, 1);
			ok = 1;
		}
		else
		{
			semop(mb->sem_id, &v1, 1);
			semop(mb->sem_id, &signal, 1);
		}
	} while(!ok);
	return 0;
}
// -----------------------------------------------------------------
// write "size" bytes into mailbox, from "input" buffer
// "offset" then contains the offset at which data is written
int sbs_mailbox_write(sbs_mailbox mb, const char* input, 
                              int size, uint64_t* offset)
{
	DEBUG(printf("start writing in mailbox\n");)
	uint64_t where = mb->shared_contents->offset;
	char *dst = &(mb->shared_contents->contents[where]);
	memcpy(dst,input,size);
	mb->shared_contents->offset += size;
	if(offset != NULL) *offset = where;
	// TODO : check size and offset
	DEBUG(printf("write done!\n");)
	return size;
}
// -----------------------------------------------------------------
// read "size" bytes from mailbox from "offset" into "output"
int sbs_mailbox_read(sbs_mailbox mb, char* output, 
                             int size, uint64_t offset)
{
	DEBUG(printf("start reading in mailbox\n");)
	
	char *src = &(mb->shared_contents->contents[offset]);
	memcpy(output,src,size);
	// TODO : check size and offset are valid
	DEBUG(printf("read done!\n");)
	return size;
}

// -----------------------------------------------------------------
// set mb offset to 0 if offset == until
int sbs_mailbox_realign(sbs_mailbox mb, uint64_t until)
{
	DEBUG(printf("realigning mailbox\n");)
	/*struct sembuf op = { SEM_PER_CLIENT*(mb->client_id)+4,
			     -1, 0};
	//printf("[time] %lf : in realign, offset is %ld, read until %ld\n",
	//	gettime(), (long)(mb->shared_contents->offset),(long)until); 
	semop(mb->sem_id, &op, 1);
	if(mb->shared_contents->offset == until) 
	{
		//printf("[time] %lf : reached realignement\n",gettime());
		mb->shared_contents->offset = 0;
		op.sem_op = 1;
		semop(mb->sem_id, &op, 1);
		int semid = mb->sem_id;
		int semno = mb->client_id*SEM_PER_CLIENT+3;
		return semctl(semid,semno,SETVAL,SBS_MAILBOX_SIZE);
	} else {
		//printf("[time] %lf : no realignement\n",gettime());
		op.sem_op = 1;
		semop(mb->sem_id, &op, 1);
		return 0;
	}*/
	struct sembuf p1 = {SEM_PER_CLIENT*(mb->client_id)+3, -1, 0};
	struct sembuf v1 = {SEM_PER_CLIENT*(mb->client_id)+3, 1, 0};
	
	semop(mb->sem_id, &p1, 1);
	if(until >= mb->shared_contents->offset)
	{
		mb->shared_contents->offset = 0;
		semop(mb->sem_id, &v1, 1);
		semctl(mb->sem_id,SEM_PER_CLIENT*(mb->client_id)+4,SETVAL,1);	
	}	
	else
	{
		semop(mb->sem_id, &v1, 1);
	}
	return 0;
}
