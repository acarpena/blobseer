#ifndef SBS_CMDQ_H
#define SBS_CMDQ_H

typedef struct sbs_cmdq_s {
	// shared data
	struct sbs_cmdq_data_s {
		struct sbs_cmd_s { 
			int cmd_type;
			int client_id;
			uint64_t offset;
			size_t size;
		} t[SBS_CMDQ_SIZE]; // storage array
		int input;       // input index
		int output;      // output index
	} *data;
	// local access ids
	int sem_id;	 // semaphore id
	int client_id;   // client id
} *sbs_cmdq;

extern int errno;

//sbs_cmdq sbs_cmdq_init(int client_id, sbs_shmem segment);

/* delete a queue structure (does not delete the shared contents) */
int  sbs_cmdq_delete(sbs_cmdq q);
/* lock the cmdq */
int  sbs_cmdq_lock(sbs_cmdq q);
/* unlock the cmdq */
int  sbs_cmdq_unlock(sbs_cmdq q);
/* push a value, lock if the queue is full */
int  sbs_cmdq_push(sbs_cmdq q, struct sbs_cmd_s value);
/* get first element in the queue, or lock if there is no element */
int  sbs_cmdq_pop(sbs_cmdq q, struct sbs_cmd_s* value);
/* return the size of the queue without locking */
int  sbs_cmdq_getsize(sbs_cmdq q);
#endif
