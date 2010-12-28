/*
 * NamespaceClientC.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: acarpena
 */

#include <cstdlib>
#include <cassert>
#include <cstring>

#include "NamespaceClientC.h"
#include "FileHandlerC.h"
#include "common/FileMetadata.hpp"
#include "FBlobseerClient/NamespaceClient.hpp"

void __attribute__ ((constructor)) ns_handler_init() {
}
void __attribute__ ((destructor)) ns_handler_fini() {
}

extern "C" int ns_init(const char *config_file, blob_env_t *env) {
    env->cfg_file = static_cast<void *>(new std::string(config_file));
    return 1;
}

extern "C" int ns_finalize(blob_env_t *env) {
    delete static_cast<std::string *>(env->cfg_file);

    return 1;
}

extern "C" int ns_initializeNamespaceHandler(blob_env_t *env, ns_handler_t *ns_handler) {
    NamespaceClient *nsClient = new NamespaceClient(*(static_cast<std::string *>(env->cfg_file)));

    ns_handler->namespace_obj = static_cast<void *>(nsClient);

    return 1;
}

extern "C" int ns_free(blob_env_t */*env*/, ns_handler_t *ns_handler) {
    delete static_cast<NamespaceClient *>(ns_handler->namespace_obj);

    return 1;
}

extern "C" int ns_mkdir(ns_handler_t *ns_handler, const char *path) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

	return nsClient->mkdir(std::string(path));
}

extern "C" int ns_exists(ns_handler_t *ns_handler, const char *path, uint8_t  *result) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

	return nsClient->exists(std::string(path), *((bool *)result));
}

extern "C" int ns_isFile(ns_handler_t *ns_handler, const char *path, int  *result)	{
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

return nsClient->isFile(std::string(path), *((bool *)result));
}

extern "C" int ns_rename(ns_handler_t *ns_handler, const char *path, const char *newPath) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

	return nsClient->rename(std::string(path), std::string(newPath));
}

extern "C" int ns_handler_status(ns_handler_t *ns_handler, const char *path, fmetadata_t *md) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

	FileMetadata md_obj;
	int result = nsClient->status(std::string(path), md_obj);
	md->path = strdup(md_obj.getPath().c_str());
	md->type = md_obj.getType();
	md->blobId = md_obj.getBlobId();

	return result;
}

extern "C" int ns_deleteFile(ns_handler_t *ns_handler, const char *path) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

	return nsClient->deleteFile(std::string(path));
}

extern "C" int ns_listDir(ns_handler_t *ns_handler, const char *path, fmetadata_list_t *list) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);

	std::vector<FileMetadata> md_vect;
	int result = nsClient->listDir(std::string(path), md_vect);

	uint i;
    list->size = md_vect.size();
    if (md_vect.size() != 0)
    {
        list->md_list = (fmetadata_t*)malloc(md_vect.size()*sizeof(fmetadata_t));
        for(i=0; i<md_vect.size(); i++)
        {
            FileMetadata md_obj = md_vect[i];

            list->md_list[i].path = strdup(md_obj.getPath().c_str());
            list->md_list[i].type = md_obj.getType();
            list->md_list[i].blobId = md_obj.getBlobId();
        }
    }
	return result;
}


//blob operations
extern "C" fblob_t* ns_createFile(ns_handler_t *ns_handler, const char *path, uint64_t page_size, uint32_t replica_count){
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);
	FileHandler* fh = nsClient->createFile(std::string(path), page_size, replica_count);

	fblob_t *fblob;
	fblob = new fblob_t();
	fblob->file_handler_obj = fh;

	return fblob;
}

extern "C" fblob_t* ns_getFileHandler(ns_handler_t *ns_handler, const char *path) {
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);
	FileHandler* fh = nsClient->getFileHandler(std::string(path));

	fblob_t *fblob;
	fblob = new fblob_t();
	fblob->file_handler_obj = fh;

	return fblob;
}

extern "C" void ns_destroyFileHandler(ns_handler_t *ns_handler,fblob_t * handler){
	NamespaceClient *nsClient = static_cast<NamespaceClient *>(ns_handler->namespace_obj);
	FileHandler *fh = static_cast<FileHandler*>(handler->file_handler_obj);

	nsClient->destroyFileHandler(fh);
	delete handler;
}



