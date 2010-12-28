/*
 * FileHandlerC.h
 *
 *  Created on: Mar 24, 2010
 *      Author: acarpena
 */

#ifndef FILEHANDLERC_H_
#define FILEHANDLERC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    void *file_handler_obj;
} fblob_t;

int fblob_getlatest(fblob_t *fblob);
uint64_t fblob_getsize(fblob_t *fblob, uint32_t version);
int fblob_get_objcount(fblob_t *fblob);
uint64_t fblob_get_page_size(fblob_t *fblob);

int fblob_read(fblob_t *fblob, uint32_t version, uint64_t offset, uint64_t  size, char *buffer);
int fblob_write(fblob_t *fblob, uint64_t offset, uint64_t size, char *buffer);
int fblob_append(fblob_t *fblob, uint64_t size, char *buffer);


#ifdef __cplusplus
}
#endif


#endif /* FILEHANDLERC_H_ */
