/*
 *  blobseer_fs.hpp
 *  
 *
 *  Created by Diana Moise on 1/18/09.
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */

#include <time.h> 
#include "fusexx.hpp"

#include "libconfig.h++"

#include "common/simple_dht.hpp"
#include "common/cached_dht.hpp"

#include "common/debug.hpp"

#include <string>

#include <boost/unordered_map.hpp>

//entry type in the DHT
const boost::int32_t T_FILE = 1;
const boost::int32_t T_DIR = 2;
const boost::int32_t T_ANY = 3;

class BlobSeerOps : public fusexx::fuse<BlobSeerOps> {
      public:
	  typedef cached_dht<simple_dht<config::socket_namespace, config::lock_t>, buffer_wrapper_hash> dht_t;
	  typedef boost::int32_t type;
	  typedef std::pair<std::string, type> kid_type;
	  
	  BlobSeerOps(std::string cfg, boost::uint64_t pg_size, unsigned int repl_cnt, unsigned int bsize); // Constructor
	  // Overload the fuse methods
	  static int access(const char *, int);
	  static int getattr(const char*, struct stat*);
	  static int fgetattr(const char*, struct stat*, struct fuse_file_info*);
	  static int mknod(const char *, mode_t, dev_t);
	  static int create(const char *, mode_t, struct fuse_file_info *);
	  static int open(const char *, struct fuse_file_info *);
	  static int release(const char *, struct fuse_file_info *);
	  static int read(const char *, char *, size_t, off_t, struct fuse_file_info *);
	  static int write(const char *, const char *, size_t, off_t, struct fuse_file_info *);
	  static int rename(const char*, const char*);
	  static int chmod(const char*, mode_t);
	  static int chown(const char*, uid_t, gid_t);
	  static int unlink(const char*);
	  static int truncate(const char*, off_t);
	  
	  static int mkdir(const char*, mode_t);
	  static int rmdir(const char*);
	  static int opendir(const char*, struct fuse_file_info*);
	  static int readdir(const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*);
      private:
	  std::string config;
	  boost::uint64_t page_size;
	  unsigned int replica_count;
	  boost::uint64_t buf_size;
	  /*the dht maps each file to a blob
		key = (name, entry type), name = full path
		value = file_info
	  */
	  dht_t *file_to_blob;
	  boost::asio::io_service io_service;
};
