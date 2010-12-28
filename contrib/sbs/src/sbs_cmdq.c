#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include "sbs_common.h"
#include "sbs_shmem.h"
#include "sbs_cmdq.h"

extern int errno;
/* just to help a little... don't forget to set the right sem num
 before using! */
static struct sembuf p = {0,-1,0};
static struct sembuf v = {0, 1,0};

/* lock the cmdq */
int sbs_cmdq_lock(sbs_cmdq q)
{	
	DEBUG(printf("locking cmdq\n");)
	p.sem_num = 0;
	if(semop(q->sem_id,&p,1) == -1)
	{
		printf("problem occured when locking the cmdq\n");
		return -1;
	} else return 0;
}

/* unlock the cmdq */
int sbs_cmdq_unlock(sbs_cmdq q)
{
	DEBUG(printf("unlocking cmdq\n");)
	v.sem_num = 0;
	if(semop(q->sem_id,&v,1) == -1)
	{
		printf("problem occured when locking the cmdq\n");
		return -1;
	} else return 0;
}

/* ******************************************************************
// These two functions have moved in sbs_shmem.c as static functions
static int init_data_field(sbs_cmdq q)
{
	LOG("initializing data field for cmdq\n");
	// blocage de la file
	if(sbs_cmdq_lock(q) == -1) return -1;
	// modification des indices
	q->data->input = 0;
	q->data->output = 0;
	// déblocage
	if(sbs_cmdq_unlock(q) == -1) return -1;
	return 0;
}

sbs_cmdq sbs_cmdq_init(int client_id, sbs_shmem segment)
{
	LOG("initialize cmdq\n");
	sbs_cmdq q = (sbs_cmdq)malloc(sizeof(struct sbs_cmdq_s));
	init_data_field(q);
	q->client_id = client_id;
	q->sem_id = segment->sem_id;
	q->data = (struct sbs_cmdq_data_s*)
	   (&(segment->data[client_id*(sizeof(struct sbs_cmdq_data_s))]));
	return q;
}
****************************************************************** */

/* delete a queue structure (does not delete the shared contents) */
int sbs_cmdq_delete(sbs_cmdq q)
{
	DEBUG(printf("deleting cmdq\n");)
	free(q);
	return 0;
}

/* push a value, lock if the queue is full */
int sbs_cmdq_push(sbs_cmdq q, struct sbs_cmd_s value)
{
	DEBUG(printf("pushing element in cmdq\n");)
	p.sem_num = 2;
	// decreasing the free-space-semaphore, or lock
	if(semop(q->sem_id,&p,1) == -1) return -1;
	// lock the cmdq before writing
	if(sbs_cmdq_lock(q) == -1) return -1;
	// add element
	q->data->t[q->data->input] = value;
	q->data->input++;
	q->data->input %= SBS_CMDQ_SIZE;
	// increase occuped size
	v.sem_num = 1;
	if(semop(q->sem_id,&v,1) == -1) return -1;
	// unlock cmdq
	return sbs_cmdq_unlock(q);
}

/* get first element in the queue, or lock if there is no element */
int sbs_cmdq_pop(sbs_cmdq q, struct sbs_cmd_s* value)
{
	DEBUG(printf("getting first element of cmdq\n");)
	errno = 0;
	// decrease occuped size semaphore, or lock
	p.sem_num = 1;
	if(semop(q->sem_id,&p,1) == -1) {errno = ECANCELED; return 0;}
	// lock cmdq
	if(sbs_cmdq_lock(q) == -1) {errno = ECANCELED; return 0;}
	// read
	memcpy(value,&(q->data->t[q->data->output]),sizeof(struct sbs_cmd_s));
	// change output index
	q->data->output++;
	q->data->output %= SBS_CMDQ_SIZE;
	// increase available space
	v.sem_num = 2;
	if(semop(q->sem_id,&v,1) == -1) {errno = ECANCELED; return -1;}
	// unlock cmdq
	if(sbs_cmdq_unlock(q) == -1) {errno = ECANCELED; return -1;}
	return 0;
}

/* return the size of the queue without locking */
int sbs_cmdq_getsize(sbs_cmdq q) 
{
	return semctl(q->sem_id,1,GETVAL,0);
}
