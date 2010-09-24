/*
 * BFS_interface.hpp
 *
 *  Created on: Aug 23, 2010
 *      Author: trungtv
 */

#ifndef BFS_INTERFACE_HPP_
#define BFS_INTERFACE_HPP_

#define LOCAL_BUFFER_FACTOR 4

#include "client/object_handler.hpp"

class BFS_interface: public object_handler {
public:
	BFS_interface(const std::string &config_file);
	virtual ~BFS_interface();

	bool fcreate(std::string blob_name, boost::uint64_t page_size, boost::uint32_t replica_count = 1);
	bool set_blob(std::string blob_name);

	bool BFS_read(boost::uint64_t offset, boost::uint64_t size, char *buffer, boost::uint32_t version);
	bool BFS_write(boost::uint64_t offset, boost::uint64_t size, char *buffer);
	//using object_handler::fcreate;
/*	bool clone(boost::int32_t id = 0, boost::int32_t version = 0);
	bool get_latest(boost::uint32_t id = 0);

	bool read(boost::uint64_t offset, boost::uint64_t size, char *buffer, boost::uint32_t version = 0);
	bool append(boost::uint64_t size, char *buffer);
	bool write(boost::uint64_t offset, boost::uint64_t size, char *buffer);
*/
private:
	boost::uint32_t id;

	boost::uint32_t local_buffer_size;
	boost::uint32_t local_position;
	boost::uint64_t local_offset;

	uint64_t page_size;
	uint64_t object_size;

	char *local_buffer;
	boost::shared_array<char> local_buffer_pointer;
	bool cache_is_dirty;

	bool do_write(boost::uint64_t offset, boost::uint64_t size, char *buffer);
	bool flush_cache();
	//std::string blob_name;
};

#endif /* BFS_INTERFACE_HPP_ */
