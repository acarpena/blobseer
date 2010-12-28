#ifndef SBS_MAILBOX_H
#define SBS_MAILBOX_H

#include <stdint.h>
#include "sbs_common.h"
/* ================================================================= */
typedef struct sbs_mailbox_s {
	// shared contents
	struct sbs_shared_mb_contents_s {
		uint64_t offset; // current offset of total used space
		char     contents[SBS_MAILBOX_SIZE];
	} *shared_contents;
	// local variables
	int sem_id;    // semaphore id
	int client_id; // associated client id
} *sbs_mailbox;
/* ================================================================= */
// initialize a mailbox structure know id
// sbs_mailbox sbs_mailbox_init(int client_id, sbs_shmem segment);
// -----------------------------------------------------------------
// delete a mailbox, free all the semaphores and shared memory
int         sbs_mailbox_delete(sbs_mailbox mb);
// -----------------------------------------------------------------
// write "size" bytes into mailbox, from "input" buffer
// "offset" then contains the offset at which data is written
int         sbs_mailbox_write(sbs_mailbox mb, const char* input, 
                              int size, uint64_t* offset);
// -----------------------------------------------------------------
// read "size" bytes from mailbox from "offset" into "output"
int         sbs_mailbox_read(sbs_mailbox mb, char* output, 
                             int size, uint64_t offset);
// -----------------------------------------------------------------
// lock until the given size is available in the mailbox
int sbs_mailbox_wait_available(sbs_mailbox mb, size_t size);

// -----------------------------------------------------------------
// set mb offset to 0 if offset == until
int sbs_mailbox_realign(sbs_mailbox mb, uint64_t until);
#endif
