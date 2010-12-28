/*
 * FileHandlerC.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: acarpena
 */

#include <cstdlib>
#include <cassert>
#include <cstring>

#include "FileHandlerC.h"
#include "FBlobseerClient/FileHandler.hpp"

void __attribute__ ((constructor)) blobseer_init() {
}
void __attribute__ ((destructor)) blobseer_fini() {
}


extern "C" int fblob_getlatest(fblob_t *fblob) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->getLatestVersion();
}

extern "C" uint64_t fblob_getsize(fblob_t *fblob, uint32_t version) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->get_size(version);
}

extern "C" int fblob_get_objcount(fblob_t *fblob) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->get_objcount();
}

extern "C" uint64_t fblob_get_page_size(fblob_t *fblob) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->get_page_size();
}


extern "C" int fblob_read(fblob_t *fblob, uint32_t version, uint64_t offset, uint64_t  size, char *buffer) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->read(offset, size, buffer, version);
}

extern "C" int fblob_write(fblob_t *fblob, uint64_t offset, uint64_t size, char *buffer) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->write(offset, size, buffer);
}

extern "C" int fblob_append(fblob_t *fblob, uint64_t size, char *buffer) {

	FileHandler *h = static_cast<FileHandler *>(fblob->file_handler_obj);

	return h->append(size, buffer);
}


