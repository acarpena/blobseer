#include <errno.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <sys/shm.h>

#include "mpi.h"



extern int errno;

#include "sbs_common.h"

#include "sbs_mailbox.h"

#include "sbs_cmdq.h"

#include "sbs_shmem.h"



/* some definitions to create the shared memory segment */

#define SEMKEYPATH "."

#define SHMKEYPATH "."

#define SEMKEYID ((getuid() & 0xff) != 0 ? getuid() : -1)

#define SHMKEYID ((getuid() & 0xff) != 0 ? getuid() : -1)



int generate_key()

{

#ifdef _LOCAL

	/* WARNING : this will only work if we split MPI_COMM_WORLD into exactly 2 separate

	communicators equaly sized */

	int rank;

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	rank -= (rank % (SBS_CLIENT_NBR+1));

	char semkeyid = (((getuid()+rank) & 0xff) != 0 ? (getuid()+rank) : (-1-rank));

	DEBUG(printf("in key generation: semkey is %d\n",(int)semkeyid);)

	return ftok(SEMKEYPATH,(((getuid() & 0xff) + rank) != 0 ? (getuid()+rank) : -1));

#else

	return ftok(SEMKEYPATH,SEMKEYID);

#endif

}





/* initialize the data field of a cmdq */

static int init_data_field(sbs_cmdq q)

{

	DEBUG(printf("initializing data field for cmdq\n");)

	
	if(sbs_cmdq_lock(q) == -1) return -1;

	
	q->data->input = 0;

	q->data->output = 0;

	
	if(sbs_cmdq_unlock(q) == -1) return -1;

	return 0;

}



/* this initialize the cmdq within a segment,

 this should not be used by a client, only by a server */

static sbs_cmdq sbs_cmdq_init(/*int client_id,*/ sbs_shmem segment)

{

	DEBUG(printf("initialize cmdq\n");)

	sbs_cmdq q = (sbs_cmdq)malloc(sizeof(struct sbs_cmdq_s));

	
	q->sem_id = segment->sem_id;

	q->data = (struct sbs_cmdq_data_s*)

	   (segment->data);

	semctl(q->sem_id,0,SETVAL,1);

	semctl(q->sem_id,1,SETVAL,0);

	semctl(q->sem_id,2,SETVAL,SBS_CMDQ_SIZE);

	init_data_field(q);

	return q;

}



/* map a cmdq from a shared memory segment, used by the client

 to get a ptr over cmdq */

static sbs_cmdq sbs_cmdq_map(int client_id, sbs_shmem segment)

{

	DEBUG(printf("mapping cmdq\n");)

	sbs_cmdq q = (sbs_cmdq)malloc(sizeof(struct sbs_cmdq_s));

	q->client_id = client_id;

	q->sem_id = segment->sem_id;

	q->data = (struct sbs_cmdq_data_s*)

	   (segment->data);

	return q;

}



/* initialize a mailbox, this is called by a client because

 it needs its client's id */

static sbs_mailbox sbs_mailbox_init(int client_id, sbs_shmem segment)

{	

	DEBUG(printf("initializing mailbox...\n");)

	sbs_mailbox mb = NULL;



	
	mb = (sbs_mailbox)malloc(sizeof(struct sbs_mailbox_s));



	
	mb->sem_id = segment->sem_id;

	mb->client_id = client_id;

	

	
	uint64_t offset = sizeof(struct sbs_cmdq_data_s)

			 + client_id*sizeof(struct sbs_shared_mb_contents_s);

	mb->shared_contents = 

		(struct sbs_shared_mb_contents_s*)

		(&(segment->data[offset]));

	mb->shared_contents->offset = 0;

	
	semctl(mb->sem_id, 3+mb->client_id*SEM_PER_CLIENT,

		SETVAL,1);

	semctl(mb->sem_id, 4+mb->client_id*SEM_PER_CLIENT,

		SETVAL,1);

	DEBUG(printf("mailbox initialized!\n");)

	return mb;

}



/* map a mailbox from a segment, this is used by a server to

 get a right ptr over an already initialized mailbox */

static sbs_mailbox sbs_mailbox_map(int client_id, sbs_shmem segment)

{	

	DEBUG(printf("mapping mailbox...\n");)

	sbs_mailbox mb = NULL;



	
	mb = (sbs_mailbox)malloc(sizeof(struct sbs_mailbox_s));



	
	mb->sem_id = segment->sem_id;

	mb->client_id = client_id;

	

	
	uint64_t offset = sizeof(struct sbs_cmdq_data_s)

			  + client_id*sizeof(struct sbs_shared_mb_contents_s);

	mb->shared_contents = 

		(struct sbs_shared_mb_contents_s*)

		(&(segment->data[offset]));



	DEBUG(printf("mailbox mapped!\n");)

	return mb;

}



/* creates a shared memory  segment */

sbs_shmem sbs_shmem_create() 

{

	int rc;

	
	int sem_nbr = 3 + SBS_CLIENT_NBR*SEM_PER_CLIENT;

	
	key_t semkey, shmkey;

	sbs_shmem mem = (sbs_shmem)malloc(sizeof(struct sbs_shmem_s));

	

	semkey = generate_key();
	if(semkey == (key_t)-1)

	{

		printf("sbs_shmem_create: ftok failed\n");

		return NULL;

	}

	shmkey = generate_key();
	if(shmkey == (key_t)-1)

	{

		printf("sbs_shmem_create: ftok failed\n");

		return NULL;

	}

	DEBUG(printf("keys generated for shared contents\n");)



	mem->sem_id = semget(semkey,sem_nbr,

		0600|IPC_CREAT|IPC_EXCL);



	if(mem->sem_id == -1)

	{

		printf("sbs_shmem_create: semget failed\n");

		return NULL;

	}

	DEBUG(printf("semaphores generated\n");)

	

	unsigned short* sarray = (unsigned short*)malloc(sizeof(unsigned short)*sem_nbr);

	int i;

	for(i=0;i<sem_nbr;i++) 

		sarray[i] = ((i < 3) || !(i-3 % SEM_PER_CLIENT)) ? 0 : 1;

	rc = semctl(mem->sem_id, 1, SETALL, sarray);

	if(rc == -1)

	{

		printf("sbs_shmem_create: semctl failed\n");

		return NULL;

	}

	DEBUG(printf("semaphores set\n");)



	size_t seg_size = sizeof(struct sbs_cmdq_data_s)

		+ SBS_CLIENT_NBR*sizeof(struct sbs_shared_mb_contents_s);	

	mem->mem_id = shmget(shmkey,seg_size, 0600|IPC_CREAT|IPC_EXCL);

	if(mem->mem_id == -1)

	{

		printf("sbs_shmem_create: shmget failed\n");

		return NULL;

	}

	DEBUG(printf("shared segment created\n");)

	

	mem->data = shmat(mem->mem_id, NULL, 0);

	if(mem->data == NULL)

	{

		printf("sbs_shmem_create: shmat failed\n");

		return NULL;

	}



	return mem;

}



/* this function initialize a shared segment from an already

 created one, it is used in the client */

sbs_shmem sbs_shmem_init() 

{

	int sem_nbr = 3 + SBS_CLIENT_NBR*SEM_PER_CLIENT;

	key_t semkey, shmkey;

	sbs_shmem mem = (sbs_shmem)malloc(sizeof(struct sbs_shmem_s));

	

	semkey = generate_key();
	if(semkey == (key_t)-1)

	{

		printf("sbs_shmem_init: ftok failed\n");

		return NULL;

	}

	shmkey = generate_key();
	if(shmkey == (key_t)-1)

	{

		printf("sbs_shmem_init: ftok failed\n");

		return NULL;

	}

	DEBUG(printf("keys generated for shared contents\n");)

	mem->sem_id = semget(semkey,sem_nbr,0600);

	if(mem->sem_id == -1)

	{

		printf("sbs_shmem_create: semget failed\n");

		return NULL;

	}

	DEBUG(printf("semaphores generated\n");)



	size_t seg_size = sizeof(struct sbs_cmdq_data_s)

		+ SBS_CLIENT_NBR*sizeof(struct sbs_shared_mb_contents_s);	

	mem->mem_id = shmget(shmkey,seg_size, 0600);

	if(mem->mem_id == -1)

	{

		printf("sbs_shmem_init: shmget failed\n");

		return NULL;

	}

	DEBUG(printf("shared segment created\n");)

	

	mem->data = shmat(mem->mem_id, NULL, 0);

	if(mem->data == NULL)

	{

		printf("sbs_shmem_init: shmat failed\n");

		return NULL;

	}



	return mem;

}



/* returns the cmdq associated with the segment calling 

 sbs_cmdq_init */

sbs_cmdq sbs_shmem_get_cmdq(sbs_shmem segment, int i) 

{

	
	return sbs_cmdq_init(/*i, */segment);

	
}



/* returns the cmdq associated with the segment calling 

 sbs_cmda_map */

sbs_cmdq sbs_shmem_map_cmdq(sbs_shmem segment, int i)

{

	if(i >= 0 && i < SBS_CLIENT_NBR)

	return sbs_cmdq_map(i, segment);

	else return NULL;

}



sbs_mailbox sbs_shmem_get_mailbox(sbs_shmem segment, int i)

{

	if(i >= 0 && i < SBS_CLIENT_NBR)

	return sbs_mailbox_init(i, segment);

	else return NULL;

}



sbs_mailbox sbs_shmem_map_mailbox(sbs_shmem segment, int i)

{

	if(i >= 0 && i < SBS_CLIENT_NBR)

	return sbs_mailbox_map(i, segment);

	else return NULL;

}



/* delete a shared memory segment */

int sbs_shmem_delete(sbs_shmem shmem) 

{

	int rc = 0;

	int res = 0;

	rc = semctl(shmem->sem_id, 1, IPC_RMID);

	if(rc == -1)

	{	

		res = -1;

		printf("sbs_shmem_delete: semctl returned -1\n");

	}

	rc = shmdt(shmem->data);

	if(rc == -1) 

	{

		res = -1;

		printf("sbs_shmem_delete: shmdt returned -1\n");

	}

	rc = shmctl(shmem->mem_id, IPC_RMID, NULL);

	if(rc == -1) 

	{

		res = -1;

		printf("sbs_shmem_delete: shmctl returned -1\n"); 

	}

	return rc;

}

