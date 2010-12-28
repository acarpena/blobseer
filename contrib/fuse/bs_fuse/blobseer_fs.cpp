/*
 *  blobseer_fs.c
 *  
 *
 *  Created by Diana Moise on 1/18/09.
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */
//#define __DEBUG
#include "file_info.hpp"
#include "client/object_handler.hpp"

#include <sys/stat.h>

#include <sstream>

using namespace std;

static const std::string SECRET  = "dd's secret";
static const unsigned int TTL = 86400;

typedef struct {
      object_handler * o;
      char * buf;
      off_t offset;
      size_t size;
} handler;

//called when the dht->put rpc is finished
static void put_callback(bool &result, int error_code)
{
      result = true;
      if (error_code != 0)
	    result = false;
}

//called when the dht->get rpc is finished
static void get_callback(file_info &fi, bool &result, buffer_wrapper val)
{
      bool b;
      result = true;
      if (val.size() == 0)
	    goto error;
      b = val.getValue(&fi, true);
      if (!b)
	    goto error;
      return;
error:
      result = false;
      return;
}

//find the value in the dht that has as key the pair (name, entry_type)
void find(const char *name, BlobSeerOps::type t, file_info &fi, bool &result, BlobSeerOps::dht_t *dht)
{
      DBG("Find " << name);
      result = true;
      //search for a specific entry type (directory or file)
      if (t != T_ANY) {
	    dht->get(buffer_wrapper(std::make_pair(string(name), t), true), boost::bind(get_callback, boost::ref(fi), boost::ref(result), _1));
	    dht->wait();
	    return;
      }
      dht->get(buffer_wrapper(std::make_pair(string(name), T_FILE), true), boost::bind(get_callback, boost::ref(fi), boost::ref(result), _1));
      dht->wait();
      if (!result)
	    goto dir;
      if (fi.deleted())
	    goto dir;
      return;
dir:
      dht->get(buffer_wrapper(std::make_pair(string(name), T_DIR), true), boost::bind(get_callback, boost::ref(fi), boost::ref(result), _1));
      dht->wait();
}

//update information about the entry "name" in the dht
//replace the value for the given key, with a new file_info
void update_info(const char *name, file_info fi, BlobSeerOps::dht_t *dht, bool d)
{
      DBG("Update info for " << name);
      bool result;
      string s(name);
      string dir;
      BlobSeerOps::kid_type p;
     
      //create the key (name, entry_type)
      if (d) {
	    dir = s.substr(0, s.find_last_of("/"));
	    if (dir.size() == 0)
		  dir = "/";
	    p = std::make_pair(dir, T_DIR);
      }
      else {
	    dir = s;
	    p = std::make_pair(dir, T_FILE);
      }
      dht->put(buffer_wrapper(p, true), buffer_wrapper(fi, true), TTL, SECRET, boost::bind(put_callback, boost::ref(result), _1));
      dht->wait();
      if (!result)
	    ERROR("Error dht->put");
}

//find the file_info for the directory that contains "name"
void find_dir(const char *name, file_info &fi, bool &result, BlobSeerOps::dht_t *dht)
{
      string s(name);
      string dir = s.substr(0, s.find_last_of("/"));
      if (dir.size() == 0)
	  dir = "/";

      find(dir.c_str(), T_DIR, boost::ref(fi), boost::ref(result), dht);
}

void add_entry(BlobSeerOps::kid_type p, file_info fi, BlobSeerOps::dht_t *dht, bool exists)
{
      INFO("Add entry for " << p.first);
      bool result;
      
      string vers(p.first);
      file_info f;
      //if the key already exists, create a new version
      if (exists) {
	  find(p.first.c_str(), p.second, boost::ref(f), boost::ref(result), dht);
	  std::vector<BlobSeerOps::kid_type> v = f.get_kids();
	  std::stringstream out;
	  int n = v.size() + 1;
	  out << n;
	  //add "/version_no " to the entry name
	  vers += string("/version_") + out.str();
	  //make this version the current one (add the other versions as its kids)
	  for (unsigned int i = 0; i < v.size(); i++)
	        fi.add_kid(v[i]);
	  f.del_all();
	  dht->put(buffer_wrapper(std::make_pair(vers, p.second), true), buffer_wrapper(f, true), TTL, SECRET, boost::bind(put_callback, boost::ref(result), _1));
	  dht->wait();
	  if (!result)
	        ERROR("Error dht->put");
	  fi.add_kid(std::make_pair(vers, p.second));
	  DBG("Version " << vers << " added");
      }

      dht->put(buffer_wrapper(p, true), buffer_wrapper(fi, true), TTL, SECRET,
		 boost::bind(put_callback, boost::ref(result), _1));
      dht->wait();
      if (!result)
	  ERROR("Error dht->put");
      
      //add the entry to the directory (if it doesn't already exist)
      file_info dir;
      find_dir(p.first.c_str(), boost::ref(dir), boost::ref(result), dht);
      std::vector<BlobSeerOps::kid_type> v = dir.get_kids();
      for (unsigned int i = 0; i < v.size(); i++)
	  if (v[i].first.compare(p.first) == 0)
	        return;
      dir.add_kid(p);
      update_info(p.first.c_str(), dir, dht, true);
}

void del_entry(BlobSeerOps::kid_type p, file_info fi, BlobSeerOps::dht_t *dht)
{
      INFO("Delete entry " << p.first);
      file_info f;
      bool result;
      
      //set the deletion flag for the entry and all its kids
      fi.set_deleted();
      update_info(p.first.c_str(), fi, dht, false);
      std::vector<BlobSeerOps::kid_type> v = fi.get_kids();
      for (unsigned int i = 0; i < v.size(); i++) {
	  find(v[i].first.c_str(), v[i].second, boost::ref(f), boost::ref(result), dht);
	  f.set_deleted();
	  update_info(v[i].first.c_str(), f, dht, false);
      }
      
      //remove the entry from the directory it belongs to
      file_info dir;
      find_dir(p.first.c_str(), boost::ref(dir), boost::ref(result), dht);
      dir.del_kid(p);
      update_info(p.first.c_str(), dir, dht, true);
}

BlobSeerOps::BlobSeerOps(std::string cfg, boost::uint64_t pg_size, unsigned int repl_cnt, unsigned int bsize) :
      config(cfg),
      page_size(pg_size),
      replica_count(repl_cnt),
      buf_size(bsize)
{
      libconfig::Config lcfg;
      try {
	  lcfg.readFile(config.c_str());
	  // get dht port
	  std::string service;
	  if (!lcfg.lookupValue("dht.service", service))
	        throw std::runtime_error("object_handler::object_handler(): DHT port is missing/invalid");
	  // get dht gateways
	  libconfig::Setting &s = lcfg.lookup("dht.gateways");
	  int ng = s.getLength();
	  if (!s.isList() || ng <= 0)
	        throw std::runtime_error("object_handler::object_handler(): Gateways are missing/invalid");
	  // get dht parameters
	  int retry, timeout, cache_size;
	  if (!lcfg.lookupValue("dht.retry", retry) || 
	        !lcfg.lookupValue("dht.timeout", timeout) || 
	        !lcfg.lookupValue("dht.cachesize", cache_size))
	        throw std::runtime_error("object_handler::object_handler(): DHT parameters are missing/invalid");
	  // build dht structure
	  file_to_blob = new dht_t(io_service, retry, timeout);
	  for (int i = 0; i < ng; i++) {
	        std::string stmp = s[i];
	        file_to_blob->addGateway(stmp, service);
	  }
      } catch(libconfig::FileIOException) {
	  throw std::runtime_error("I/O error trying to parse config file");
      } catch(libconfig::ParseException &e) {
	  std::ostringstream ss;
	  ss << "Parse exception (line " << e.getLine() << "): " << e.getError();
	  throw std::runtime_error(ss.str());
      } catch(std::runtime_error &e) {
	  throw e;
      } catch(...) {
	  throw std::runtime_error("Unknown exception");
      }
      
      //check if metadata already exists in the dht
      file_info fi;
      bool result;
      find("/", T_DIR, boost::ref(fi), boost::ref(result), file_to_blob);
      
      if (result) {
	    DBG("Root directory already exists");
	    return;
      }
      
      //if not, create the root directory
      fi.set_mode(S_IFDIR | 0755);
      time_t t;
      time(&t);
      fi.set_ctime(t);
      fi.set_user(fuse_get_context()->uid);
      fi.set_group(fuse_get_context()->gid);
      
      file_to_blob->put(buffer_wrapper(std::make_pair(string("/"), T_DIR), true), buffer_wrapper(fi, true), TTL, SECRET, boost::bind(put_callback, boost::ref(result), _1));
      file_to_blob->wait();
      if (!result)
	  ERROR("Error dht->put");
}

int BlobSeerOps::access(const char *name, int m)
{
      DBG("Checking access for " << name);

      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.get_mode() & m)
	  return 0;
      ERROR("Permission denied.");
      return -1;
}

int BlobSeerOps::getattr(const char *name, struct stat *stbuf)
{
      DBG("Getattr for " << name);
      memset(stbuf, 0, sizeof(struct stat));
      
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (S_ISDIR(fi.get_mode()))
	  stbuf->st_mode = S_IFDIR;
      else
	  stbuf->st_mode = S_IFREG;
      stbuf->st_mode |= fi.get_mode();
      stbuf->st_uid = fi.get_uid();
      stbuf->st_gid = fi.get_gid();
      stbuf->st_size = fi.get_size();
      stbuf->st_atime = fi.get_ct();
      stbuf->st_mtime = fi.get_ct();
      stbuf->st_ctime = fi.get_ct();
      
      return 0;
}

int BlobSeerOps::fgetattr(const char *name, struct stat *stbuf, struct fuse_file_info*)
{
      DBG("Fgetattr for " << name);
      memset(stbuf, 0, sizeof(struct stat));
      
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (S_ISREG(fi.get_mode())) {
	  stbuf->st_mode = S_IFREG;
      }
      else {
	  ERROR("Not a file!");
	  return -EISDIR;
      }
      stbuf->st_mode |= fi.get_mode();
      stbuf->st_uid = fi.get_uid();
      stbuf->st_gid = fi.get_gid();
      stbuf->st_atime = fi.get_ct();
      stbuf->st_mtime = fi.get_ct();
      stbuf->st_ctime = fi.get_ct();
      
      return 0;
}

//create a file
int BlobSeerOps::mknod(const char *name, mode_t m, dev_t d)
{
      DBG("Mknod " << name);
      stringstream stream(name);
      string dir("/"), fis, tmp;
      
      file_info fi;
      bool result;
      bool exists;
            
      //check if the directories in the path exist and they really are directories
      do {
	  tmp += dir;
	  find(tmp.c_str(), T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
	  if (!result) {
	        if (getline(stream, fis, '/')) {
		    ERROR(tmp << " doesn't exist!");
		    return -ENOENT;
	        }
	        else
		    break;
	  }
	  else {
	        if (fi.deleted()) {
		    if (getline(stream, fis, '/')) {
			ERROR("No such file!");
			return -ENOENT;
		    }
	        }
	        else
		    if (!S_ISDIR(fi.get_mode())) {
			ERROR(tmp << " is not a directory!");
			return -ENOTDIR;
		    }
	  }
      }
      while (getline(stream, dir, '/'));

      find(name, T_FILE, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  exists = false;
	  goto create;
      }
      exists = true;
      if (!fi.deleted()) {
	  ERROR("file " << name << " already exists!");
	  return -EEXIST;
      }

create:
    //create a blob for this file
      object_handler * o = new object_handler(self->config);
      if (!o->create(self->page_size, self->replica_count)) {
	  ERROR("Could not create blob!");
	  return -ENOENT;
      }
      INFO("Blob " << o->get_id() << " created");
      //create the metadata for the file
      file_info fi_1(o->get_id(), m);
      fi_1.set_dev(d);
      fi_1.set_user(fuse_get_context()->uid);
      fi_1.set_group(fuse_get_context()->gid);
      fi_1.set_size(0);
      time_t t;
      time(&t);
      fi_1.set_ctime(t);
            
      add_entry(std::make_pair(string(name), T_FILE), fi_1, self->file_to_blob, exists);
   
      delete o;
      return 0;
}

int BlobSeerOps::open(const char *name, struct fuse_file_info * ffi)
{
      DBG("Open file " << name);
      file_info fi;
      bool result;
      find(name, T_FILE, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      
      handler * fh = (handler*)malloc(sizeof(handler));
      fh->o = new object_handler(self->config);
    
      if (!fh->o->get_latest(fi.get_id())) {
	  ERROR("Could not get the latest version");
	  return -ENOENT;
      }
      
      fh->buf = (char*)malloc(self->buf_size);
      memset(fh->buf, 0, self->buf_size);
      fh->offset = -1;
      fh->size = 0;
      
      //put the object handler in the fuse_file_info structure to be available in other functions
      ffi->fh = (uint64_t)fh;
      ffi->direct_io = 1;
      return 0;
}

int BlobSeerOps::create(const char *name, mode_t m, struct fuse_file_info * ffi)
{
      DBG("Create " << name);
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (result && !fi.deleted())
	  goto open;
      if (mknod(name, m, 123) == 0)
	  goto open;
      return -ENOENT;
open:
      return open(name, ffi);
}

int bs_read(object_handler *o, char *buf, size_t size, off_t offset)
{
      if (!o->get_latest()) {
	  ERROR("Could not get the latest version");
	  return -ENOENT;
      }
      
      boost::uint64_t b_size = o->get_size(), page_size = o->get_page_size();

      if (offset < 0 || offset > (off_t)b_size) {
	  ERROR("Bad offset");
	  return -EINVAL;
      }
      
      //align the offset and size to page_size
      DBG("Total size: " << b_size);
      if ((offset + size) > b_size)
	  size = b_size - offset;
      if (size == 0)
	  return 0;
      
      size_t new_size = (size / page_size) * page_size;
      off_t new_offset = (offset / page_size) * page_size;
      
      bool one = false, two = false;
      
      if ((offset % page_size) != 0) {
	  new_size += page_size;
	  one = true;
      }
      if ((offset + size) % page_size != 0) {
	  new_size += page_size;
	  two = true;
      }
      if (one && two && ((offset / page_size) == ((offset + size) / page_size)))
	  new_size -= page_size;
      
      DBG("New size and offset: " << new_size << " " << new_offset);
      
      char *buff = (char*) malloc (new_size);
      memset(buff, 0, new_size);
      try {
	  if (!o->read(new_offset, new_size, buff)) {
	        ERROR("Could not read!");
	        return -EIO;
	  }
      }
      catch (...) {}

      memcpy(buf, buff + offset % page_size, size);
      free(buff);
      INFO("Read: " << size << " bytes from blob");
      return size;
}

int BlobSeerOps::read(const char *name, char *buf, size_t size, off_t offset, struct fuse_file_info *ffi)
{
      INFO("Read " << size << " bytes from offset " << offset << " from file " << name);

      handler * fh = (handler*)ffi->fh;
      if (fh == NULL) {
	  ERROR("File not opened!");
	  return -EBADF;
      }
      if (fh->offset == -1)
	    goto fill_buffer;
      
      if ((offset >= fh->offset) && ((offset + size) <= (fh->offset + self->buf_size))) {
	    INFO("Reading from buffer " << size << " bytes");
	    memcpy(buf, fh->buf + (offset - fh->offset), size);
	    return size;
      }
      
fill_buffer:
      memset(fh->buf, 0, self->buf_size);
      if (self->buf_size > size) {
	    if (bs_read(fh->o, fh->buf, self->buf_size, offset) < 0)
		  return -EIO;
	    fh->offset = offset;
	    memcpy(buf, fh->buf, size);
      }
      else
	    if (bs_read(fh->o, buf, size, offset) < 0)
		  return -EIO;
      return size;
}

int bs_write(object_handler *o, const char *buf, size_t size, off_t offset)
{
      if (!o->get_latest()) {
	  ERROR("Could not get the latest version");
	  return -ENOENT;
      }
      
      boost::uint64_t page_size = o->get_page_size();
      
      //align offset and size to page_size
      size_t new_size = (size / page_size) * page_size;
      off_t new_offset = (offset / page_size) * page_size;

      bool one = false, two = false;
      
      if ((offset % page_size) != 0) {
	  new_size += page_size;
	  one = true;
      }
    
      if ((offset + size) % page_size != 0) {
	  new_size += page_size;
	  two = true;
      }
    
      if (one && two && ((offset / page_size) == ((offset + size) / page_size)))
	  new_size -= page_size;
   
      DBG("New offset and size: " << new_offset << " " << new_size);
      
      char *buff = (char*) malloc (new_size);
      memset(buff, 0, new_size);
      if ( (offset % page_size) != 0)
	  try {
	        if (!o->read(new_offset, page_size, buff))
		    ERROR("Could not read!");
	  } catch (...) {}
      memcpy((char*)(buff + offset % page_size), (char*)buf, size);
      if ((offset + size) % page_size != 0) {
	  char *b = (char*) malloc(page_size);
	  memset(b, 0, page_size);
	  try {
	        if (!o->read(((offset + size) / page_size) * page_size, page_size, b))
		    ERROR("Could not read!");
	  } catch(...) {}
	  memcpy(buff + offset % page_size + size, b + (offset + size) % page_size, page_size - (offset + size) % page_size);
	  free(b);
      }
      if (!o->write(new_offset, new_size, buff)) {
	  ERROR("Could not write!");
	  return -EIO;
      }
      free(buff);
      INFO("Wrote " << size << " bytes to blob");
      return size;
}

int bs_flush(handler *fh)
{
      INFO("Flush buffer.");
      if (bs_write(fh->o, fh->buf, fh->size, fh->offset) < 0)
	    return -EIO;
      memset(fh->buf, 0, fh->size);
      fh->size = 0;
      fh->offset = -1;
      return 0;
}

int BlobSeerOps::write(const char *name, const char *buf, size_t size, off_t offset, struct fuse_file_info *ffi)
{
      INFO("Write to file " << name << " from " << offset << " size: " << size);
      
      handler * fh = (handler*)ffi->fh;
      if (fh == NULL) {
	  ERROR("File not opened!");
	  return -EBADF;
      }

      if (offset < 0) {
	  ERROR("Error: bad offset");
	  return -EINVAL;
      }
      
      if (fh->offset == -1) {
	    fh->offset = offset;
	    goto buffer;
      }
      
      if ((off_t)(fh->offset + fh->size) != offset) {
	    if (bs_flush(fh) < 0)
		  return -EIO;
	    fh->offset = offset;
      }
      
buffer:
      if ((fh->size + size) > self->buf_size) {
	    if (bs_flush(fh) < 0)
		  return -EIO;
	    return bs_write(fh->o, buf, size, offset);
      }
      memcpy((char*)(fh->buf + fh->size), buf, size);
      fh->size += size;
      if (fh->size == self->buf_size)
	    if (bs_flush(fh) < 0)
		  return -EIO;
      return size;
}

int BlobSeerOps::release(const char *name, struct fuse_file_info *ffi)
{
      DBG("Release " << name);
      
      handler * fh = (handler*)ffi->fh;
      if (fh == NULL) {
	  ERROR("File not opened!");
	  return -ENOENT;
      }
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (fi.get_size() != fh->o->get_size()) {
	  fi.set_size(fh->o->get_size());
	  update_info(name, fi, self->file_to_blob, false);
      }
      
      if (fh->size > 0)
	    if (ffi->writepage)
		  if (bs_flush(fh) < 0)
			return -EIO;

      ffi->fh = NULL;
      delete fh->o;
      free(fh->buf);
      free(fh);

      return 0;
}

int BlobSeerOps::chmod(const char *name, mode_t m)
{
      DBG("Chmod for " << name);
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      
      if (S_ISDIR(fi.get_mode())) {
	  ERROR(name << " is a directory!");
	  return -ENOENT;
      }
      
      file_info f(fi);
      f.set_mode(m);
      add_entry(std::make_pair(string(name), T_FILE), f, self->file_to_blob, true);
      
      return 0;
}

int BlobSeerOps::truncate(const char *name, off_t length)
{
      DBG("Truncate " << name << " to " << length << " bytes");
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (S_ISDIR(fi.get_mode())) {
	  ERROR(name << " is a directory!");
	  return -ENOENT;
      }
      if (length < 0) {
	  ERROR("Bad offset value");
	  return -EINVAL;
      }
      boost::uint64_t b_size = fi.get_size();
      if (length < (off_t)b_size)
	  goto done;
      if (length > (off_t)b_size) {
	  object_handler * o = new object_handler(self->config);
	  if (!o->get_latest(fi.get_id())) {
	        ERROR("Could not get the latest version");
	        return -ENOENT;
	  }
	  char *buf = (char*)malloc(self->page_size);
	  memset(buf, 0, self->page_size);
	  int i, p = b_size / self->page_size, q = length / self->page_size;
	  if (b_size % self->page_size != 0)
	        p++;
	  if (length % self->page_size != 0)
	        q++;
	  for (i = (p + 1); i <= q; i++) {
	        if (!o->write((i - 1) * self->page_size, self->page_size, buf)) {
		    ERROR("Could not write!");
		    return -EIO;
	        }
	  }
	  delete o;
	  goto done;
      }
      return 0;
done:
      file_info f(fi);
      f.set_size(length);
      add_entry(std::make_pair(string(name), T_FILE), f, self->file_to_blob, true);
      return 0;
}

int BlobSeerOps::chown(const char *name, uid_t u, gid_t g)
{
      DBG("Chown for " << name);
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (S_ISDIR(fi.get_mode())) {
	  ERROR(name << " is a directory!");
	  return -ENOENT;
      }
      file_info f(fi);
      f.set_user(u);
      f.set_group(g);
      add_entry(std::make_pair(string(name), T_FILE), f, self->file_to_blob, true);

      return 0;
}

int BlobSeerOps::rename(const char *old_name, const char *new_name)
{
      DBG("Rename: " << old_name << " to " << new_name);
      file_info old_fi, fi, dir_fi;
      bool result;
      find(old_name, T_ANY, boost::ref(old_fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR(old_name << " no such file!");
	  return -ENOENT;
      }
      if (old_fi.deleted()) {
	  ERROR(old_name << " no such file!");
	  return -ENOENT;
      }
    
      //check for new_name directory
      find_dir(new_name, boost::ref(dir_fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("Directory doesn't exist");
	  return -ENOENT;
      }
      if (S_ISREG(dir_fi.get_mode())) {
	  ERROR("Not a directory!");
	  return -ENOTDIR;
      }
      
      //remove old file
      int rez = self->unlink(old_name);
      if (rez != 0)
	  return rez;
      
      add_entry(std::make_pair(string(new_name), T_FILE), old_fi, self->file_to_blob, false);
 
      return 0;
}

int BlobSeerOps::unlink(const char *name)
{
      DBG("Unlink " << name);
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR(name << " no such file!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      
      if (S_ISDIR(fi.get_mode())) {
	  ERROR(name << " is a directory!");
	  return -EISDIR;
      }
      
      del_entry(std::make_pair(string(name), T_FILE), fi, self->file_to_blob);
            
      return 0;
}

int BlobSeerOps::mkdir(const char *name, mode_t m)
{
      DBG("Mkdir " << name);
      stringstream stream(name);
      string dir("/"), fis, tmp;
      
      file_info fi;
      bool result;
      
      do {
	  tmp += dir;
	  find(tmp.c_str(), T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
	  if (!result) {
	        if (getline(stream, fis, '/')) {
		    ERROR(tmp << " doesn't exist!");
		    return -ENOENT;
	        }
	        else
		    break;
	  }
	  else {
	        if (fi.deleted()) {
		    if (getline(stream, fis, '/')) {
			ERROR("No such file!");
			return -ENOENT;
		    }
	        }
	        else
		    if (!S_ISDIR(fi.get_mode())) {
			ERROR(tmp << " is not a directory!");
			return -ENOTDIR;
		    }
	  }
      }
      while (getline(stream, dir, '/'));
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (result && !fi.deleted()) {
	  ERROR("Directory " << name << " already exists!");
	  return -EEXIST;
      }
      file_info fi_1(m | S_IFDIR);
      fi_1.set_user(fuse_get_context()->uid);
      fi_1.set_group(fuse_get_context()->gid);
      time_t t;
      time(&t);
      fi_1.set_ctime(t);
      
      add_entry(std::make_pair(string(name), T_DIR), fi_1, self->file_to_blob, false);
      
      DBG("Directory " << name << " created ");
      return 0;
}

int BlobSeerOps::opendir(const char *name, struct fuse_file_info *ffi)
{
      DBG("Opendir " << name);
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("Directory " << name << " doesn't exist!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }

      if (!S_ISDIR(fi.get_mode())) {
	  ERROR(name << " is not a directory!");
	  return -ENOTDIR;
      }
      return 0;
}

int BlobSeerOps::readdir(const char *name, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *ffi)
{
      DBG("Readdir " << name);
      (void) offset;
      (void) ffi;
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("Directory " << name << " doesn't exist!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      filler(buf, ".", NULL, 0);
      filler(buf, "..", NULL, 0);
      
      std::vector<BlobSeerOps::kid_type> v = fi.get_kids();
     
      for (unsigned int i = 0; i < v.size(); i++ ) {
	  string s(name);
	  int poz = s.size();
	  if (s.compare("/") != 0)
	        poz ++;
	  string ss = v[i].first.substr(poz);
	  filler(buf, ss.c_str(), NULL, 0);
      }
    
      return 0;
}

int BlobSeerOps::rmdir(const char *name)
{
      DBG("Rmdir " << name);
      file_info fi;
      bool result;
      find(name, T_ANY, boost::ref(fi), boost::ref(result), self->file_to_blob);
      if (!result) {
	  ERROR("Directory " << name << " doesn't exist!");
	  return -ENOENT;
      }
      if (fi.deleted()) {
	  ERROR("No such file!");
	  return -ENOENT;
      }
      if (!S_ISDIR(fi.get_mode())) {
	  ERROR(name << " is not a directory!");
	  return -ENOTDIR;
      }
      if (!fi.get_kids().empty()) {
	  ERROR("Directory not empty!");
	  return -ENOTEMPTY;
      }
      del_entry(std::make_pair(string(name), T_DIR), fi, self->file_to_blob);
      
      return 0;
}
