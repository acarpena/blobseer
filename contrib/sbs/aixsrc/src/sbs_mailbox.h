#ifndef SBS_MAILBOX_H

#define SBS_MAILBOX_H



#include <stdint.h>

#include "sbs_common.h"

/* ================================================================= */

typedef struct sbs_mailbox_s {

	
	struct sbs_shared_mb_contents_s {

		uint64_t offset; 
		char     contents[SBS_MAILBOX_SIZE];

	} *shared_contents;

	
	int sem_id;    
	int client_id; 
} *sbs_mailbox;

/* ================================================================= */





int         sbs_mailbox_delete(sbs_mailbox mb);




int         sbs_mailbox_write(sbs_mailbox mb, const char* input, 

                              int size, uint64_t* offset);



int         sbs_mailbox_read(sbs_mailbox mb, char* output, 

                             int size, uint64_t offset);



int sbs_mailbox_wait_available(sbs_mailbox mb, size_t size);





int sbs_mailbox_realign(sbs_mailbox mb, uint64_t until);

#endif

