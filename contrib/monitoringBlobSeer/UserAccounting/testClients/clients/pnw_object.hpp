#ifndef PNW_OBJECT_H
#define PNW_OBJECT_H

#include <iostream>
#include <cstdlib>
#include <ctime> 

/////////BS stuff
#include <sstream>
#include <libconfig.h++>

#include <boost/dynamic_bitset.hpp>

#include "pmanager/publisher.hpp"
#include "provider/provider.hpp"
#include "vmanager/main.hpp"
#include "common/debug.hpp"
#include "client/replica_policy.hpp"

///////
#include "object_handler.hpp"


class pnw_object: public object_handler
{
	public: 
		pnw_object(const std::string &config_file);
		
	private:
		bool exec_write(boost::uint64_t offset, boost::uint64_t size, char *buffer, bool append) ;
};

#endif
