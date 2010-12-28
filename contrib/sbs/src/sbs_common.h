#include <stdio.h>
#ifndef SBS_COMMON_H
#define SBS_COMMON_H

/* some logging definitions */
#define LOG(id,m) //printf("%d : ",id); printf(m)
#ifdef _DEBUG
#define DEBUG(p) p
#else
#define DEBUG(p)
#endif

/* sizes */
#define KB 1024
#define MB 1024*1024
#define GB 1024*1024*1024

/* number of computation task in one node */
#define SBS_CLIENT_NBR      4
/* max mailbox size */
#define SBS_MAILBOX_SIZE   256*KB
/* max number of elements in cmdq */
#define SBS_CMDQ_SIZE       128

/* number of semaphores per client
 should not be < 2 */
#define SEM_PER_CLIENT 2
double gettime();
#endif
