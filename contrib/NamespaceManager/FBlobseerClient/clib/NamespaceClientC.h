/*
 * NamespaceClientC.h
 *
 *  Created on: Mar 23, 2010
 *      Author: acarpena
 */

#ifndef NAMESPACECLIENTC_H_
#define NAMESPACECLIENTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "FileHandlerC.h"

typedef struct {
	char *path;
	uint32_t type;
	uint64_t blobId;
} fmetadata_t;

typedef struct {
    void *namespace_obj;
} ns_handler_t;

typedef struct metadata_list {
   fmetadata_t *md_list;
   int size;
} fmetadata_list_t;

typedef struct blob_env {
    void *cfg_file;
} blob_env_t;

int ns_init(const char *config_file, blob_env_t *env);
int ns_finalize(blob_env_t *env);

// namespace operations
int ns_initializeNamespaceHandler(blob_env_t *env, ns_handler_t *ns_handler);
int ns_free(blob_env_t */*env*/, ns_handler_t *ns_handler);

int ns_mkdir(ns_handler_t *ns_handler, const char *path);
int ns_exists(ns_handler_t *ns_handler, const char *path, uint8_t  *result);
int ns_isFile(ns_handler_t *ns_handler, const char *path, int  *result);
int ns_rename(ns_handler_t *ns_handler, const char *path, const char *newPath);
int ns_status(ns_handler_t *ns_handler, const char *path, fmetadata_t *md);
int ns_deleteFile(ns_handler_t *ns_handler, const char *path);
int ns_listDir(ns_handler_t *ns_handler, const char *path, fmetadata_list_t *list);

//blob operations
fblob_t* ns_createFile(ns_handler_t *ns_handler, const char *path, uint64_t page_size, uint32_t replica_count);
fblob_t* ns_getFileHandler(ns_handler_t *ns_handler, const char *path);
void ns_destroyFileHandler(ns_handler_t *ns_handler, fblob_t * handler);


#ifdef __cplusplus
}
#endif


#endif /* NAMESPACECLIENTC_H_ */
