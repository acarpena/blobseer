/*
 * FileHandler.hpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#ifndef FILEHANDLER_HPP_
#define FILEHANDLER_HPP_
#define LOCAL_BUFFER_FACTOR 4
#include "common/config.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_client.hpp"
#include "client/object_handler.hpp"
#include <boost/shared_array.hpp>


typedef rpc_client<config::socket_namespace> rpc_client_t;

class FileHandler {
public:
	FileHandler(const std::string &config_file, const boost::uint64_t blobid);
	virtual ~FileHandler();

	//blob operations
	boost::int32_t get_objcount();
	boost::uint64_t get_size(boost::uint32_t version = 0) ;
	boost::uint64_t getLatestVersion() ;
	boost::uint64_t get_page_size() ;

	bool read(boost::uint64_t offset, boost::uint64_t size,  char *buffer, boost::uint32_t version = 0);
	bool append(boost::uint64_t size, char *buffer);
	bool write(boost::uint64_t offset, boost::uint64_t size, char *buffer);



private:
	boost::int32_t local_buffer_size;
	boost::int32_t local_position;
	boost::int64_t local_offset;
	char *local_buffer;
	boost::shared_array<char> local_buffer_pointer;
	bool cache_is_dirty;

	object_handler objectHandler;				// handler for blob operations
	bool do_write(boost::uint64_t offset, boost::uint64_t size, char *buffer);
	bool flush_cache();

};

#endif /* FILEHANDLER_HPP_ */
