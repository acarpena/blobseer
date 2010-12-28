/*
 *  file_info.hpp
 *  
 *
 *  Created by Diana Moise on 1/19/09.
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */

#include <vector>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/serialization/utility.hpp>

#include "blobseer_fs.hpp"

//information about a file and the blob it is mapped to
//metadata information for a directory
class file_info
{
      private:
	  unsigned int id;	//blob id
	  boost::uint64_t size;	//file / dir size
	  bool del;	//shows if the file / dir was deleted
	  mode_t m;
	  dev_t d;
	  uid_t user;
	  pid_t group;
	  time_t ctime;
	  //for a directory, it contains the entries in that directory (as pairs: name, type)
	  //for a file, it contains the versions
	  std::vector< BlobSeerOps::kid_type > kids;
      public:
	  file_info() : size(0), del(false) {}
	  file_info(mode_t mm) : size(0), del(false), m(mm) {}
	  file_info(unsigned int i, mode_t mm) : id(i), size(0), del(false), m(mm) {}
	  
	  file_info(file_info const &f) :
		  id(f.id),
		  size(f.size),
		  del(f.del),
		  m(f.m),
		  d(f.d),
		  user(f.user),
		  group(f.group),
		  ctime(f.ctime),
		  kids(f.kids) {}
	  
	  bool deleted()
	  {
	        return del;
	  }
	  
	  void set_deleted()
	  {
	        del = true;
	  }
	  
	  void add_kid(BlobSeerOps::kid_type p)
	  {
	        kids.push_back(p);
	  }
	  void del_kid(BlobSeerOps::kid_type p)
	  {
	        std::vector< BlobSeerOps::kid_type >::iterator it = kids.begin();
	        while (it != kids.end()) {
		    BlobSeerOps::kid_type pp = *it;
		    if (pp == p) {
			kids.erase(it);
			return;
		    }
		    it ++;
	        }
	  }
	  void del_all()
	  {
	        kids.clear();
	  }
	  
	  std::vector< BlobSeerOps::kid_type > get_kids()
	  {
	        return kids;
	  }
	  
	  unsigned int get_id()
	  {
	        return id;
	  }
	  void set_id(unsigned int i)
	  {
	        id = i;
	  }
	  
	  boost::uint64_t get_size()
	  {
	        return size;
	  }
	  void set_size(boost::uint64_t s)
	  {
	        size = s;
	  }
	  
	  mode_t get_mode()
	  {
	        return m;
	  }
	  void set_mode(mode_t mm)
	  {
	        m = mm;
	  }
	  
	  dev_t get_dev()
	  {
	        return d;
	  }
	  void set_dev(dev_t dev)
	  {
	        d = dev;
	  }
	  
	  uid_t get_uid()
	  {
	        return user;
	  }
	  void set_user(uid_t u)
	  {
	        user = u;
	  }
	  
	  pid_t get_gid()
	  {
	        return group;
	  }
	  void set_group(pid_t g)
	  {
	        group = g;
	  }
	  
	  time_t get_ct()
	  {
	        return ctime;
	  }
	  void set_ctime(time_t ct)
	  {
	        ctime = ct;
	  }
	  
	  template <class Archive> void serialize(Archive &ar, unsigned int) 
	  {
	        ar & id & size & del & m & d & user & group & ctime & kids;
	  }
};

