#include <cstdlib>
#include <cassert>
#include <cstring>

#include "BFS_interface-c.h"
#include "BFS_interface.hpp"

void __attribute__ ((constructor)) blobseer_init() {
}
void __attribute__ ((destructor)) blobseer_fini() {
}

extern "C" int blob_init(const char *config_file, blob_env_t *env) {
    env->cfg_file = static_cast<void *>(new std::string(config_file));
    return 1;
}

extern "C" int blob_finalize(blob_env_t *env) {
    delete static_cast<std::string *>(env->cfg_file);

    return 1;
}

extern "C" int blob_create(blob_env_t *env, offset_t page_size, unsigned int replica_count, blob_t *blob) {
    BFS_interface *h = new BFS_interface(*(static_cast<std::string *>(env->cfg_file)));
    if (!h->create(page_size, replica_count))
	return 0;

    blob->obj = static_cast<void *>(h);
    blob->id = h->get_id();
    blob->page_size = h->get_page_size();
    blob->latest_version = h->get_version();

    return 1;
}
extern "C" int blob_fcreate(blob_env_t *env, char *blob_name, offset_t page_size, unsigned int replica_count, blob_t *blob) {
    BFS_interface *h = new BFS_interface(*(static_cast<std::string *>(env->cfg_file)));
    if (!h->fcreate((static_cast<std::string>(blob_name)), page_size, replica_count))
	return 0;

    blob->obj = static_cast<void *>(h);
    blob->id = h->get_id();
    blob->page_size = h->get_page_size();
    blob->latest_version = h->get_version();

    return 1;
}

extern "C" int blob_setid(blob_env_t *env, char *blob_name, blob_t *blob) {
    BFS_interface *h = new BFS_interface(*(static_cast<std::string *>(env->cfg_file)));
    if (!h->set_blob(static_cast<std::string>(blob_name)))
	return 0;

    blob->obj = static_cast<void *>(h);
    blob->id = h->get_id();
    blob->page_size = h->get_page_size();
    blob->latest_version = h->get_version();

    return 1;
}

extern "C" int blob_free(blob_env_t */*env*/, blob_t *blob) {
    delete static_cast<BFS_interface *>(blob->obj);

    return 1;
}

extern "C" offset_t blob_getsize(blob_t *blob, id_t version) {
    BFS_interface *h = static_cast<BFS_interface *>(blob->obj);

    return h->get_size(version);
}

extern "C" int blob_getlatest(blob_t *blob) {
    BFS_interface *h = static_cast<BFS_interface *>(blob->obj);

    if (!h->get_latest())
	return 0;

    blob->latest_version = h->get_version();
    return 1;
}

extern "C" int blob_read(blob_t *blob, id_t version, offset_t offset, offset_t size, char *buffer) {
    bool ret = false;
    
    BFS_interface *h = static_cast<BFS_interface *>(blob->obj);
    try {
	    ret = h->read(offset, size, buffer, version);
    } catch (std::exception &e) {
	std::cerr << "READ ERROR: " << e.what() << std::endl;
	return 0;
    }
    
    if (!ret) {
	std::cerr << "READ ERROR: " << offset << ", " << size << std::endl;
	return 0;
    } else
	return 1;
}

extern "C" int blob_write(blob_t *blob, offset_t offset, offset_t size, char *buffer) {
    BFS_interface *h = static_cast<BFS_interface *>(blob->obj);

    return h->write(offset, size, buffer);
}

extern "C" int blob_append(blob_t *blob, offset_t size, char *buffer) {
    BFS_interface *h = static_cast<BFS_interface *>(blob->obj);

    return h->append(size, buffer);
}
