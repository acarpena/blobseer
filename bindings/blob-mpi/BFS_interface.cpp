/*
 * BFS_interface.cpp
 *
 *  Created on: Aug 23, 2010
 *      Author: trungtv
 */

#include "BFS_interface.hpp"

BFS_interface::BFS_interface(const std::string &config_file) : object_handler(config_file) {
	// TODO Auto-generated constructor stub

}

BFS_interface::~BFS_interface() {
	// TODO Auto-generated destructor stub
}

bool BFS_interface::fcreate(std::string blob_name, boost::uint64_t page_size, boost::uint32_t replica_count){
	boost::hash<std::string> hash_name;
	id = hash_name(blob_name);
	if(object_handler::fcreate(id, page_size, replica_count)) {
		local_buffer_size = object_handler::get_page_size() * LOCAL_BUFFER_FACTOR;
		local_offset = 0;
		local_buffer = new char [ local_buffer_size ];
		local_buffer_pointer = boost::shared_array<char>(local_buffer);
		local_position = 0;
		cache_is_dirty = false;
		INFO("Initialized the local buffer, size =" << local_buffer_size << " , offset =" << local_offset);
		return true;
	}
	else return false;
}

bool BFS_interface::set_blob(std::string blob_name) {
	if(blob_name.empty())
		return object_handler::get_latest();

	boost::hash<std::string> hash_name;
	id = hash_name(blob_name);
	if (object_handler::get_latest(id)) {

		local_buffer_size = object_handler::get_page_size() * LOCAL_BUFFER_FACTOR;
		local_offset = 0;
		local_buffer = new char [ local_buffer_size ];
		local_buffer_pointer = boost::shared_array<char>(local_buffer);
		local_position = 0;
		cache_is_dirty = false;
		INFO("Initialized the local buffer, size =" << local_buffer_size << " , offset =" << local_offset);
		return true;
	}
	else return false;

}

bool BFS_interface::do_write(boost::uint64_t offset, boost::uint64_t size, char *buffer) {
	//uint64_t new_offset;
	// Update object to latest version
	if (!object_handler::get_latest(0)) return false;

	object_size = object_handler::get_size(0);
	page_size = object_handler::get_page_size();

	INFO("Write offset=" << offset << " size=" << size << " on object_size =" << object_size << " page_size = " << page_size);
	// size of partial read from leftmost/rightmost page when read operation is unaligned
	uint64_t left_part = offset % page_size;
	uint64_t right_part = (page_size - (offset + size) % page_size) % page_size;
	uint64_t new_size = left_part + right_part + size;
	if (left_part !=0 || right_part !=0)
	{
		try{

			INFO("Read left/right part first");
			char * new_buffer = new char[new_size];
			boost::scoped_array<char> sa ( new_buffer );
			//boost::shared_array<char> new_share_buffer(new char[new_size]);
			//char *new_buffer = &new_share_buffer[0];
			//buffer_wrapper new_wrapper(new_buffer, new_size, true);
			// Read the left aligned page of the latest version
			if (left_part != 0)
			{
				INFO("Read left ");
				object_handler::read(offset -left_part, page_size, new_buffer, 0);
			}
			// Read the right aligned page
			if (right_part !=0 && offset + size < object_size)
			{
				INFO("Read right");
				object_handler::read(offset + size - (page_size - right_part), page_size, new_buffer + new_size - page_size, 0);
			}
			memcpy(new_buffer + page_size - left_part, buffer, size);
			if (!object_handler::write(offset -left_part, new_size, new_buffer))
			{
				INFO("Error on writing");
				//delete[] new_buffer;
				return false;
			}
			//delete[] new_buffer;
			INFO("Done writing");
		}
		catch (std::exception &e) {
			INFO("caught unexpected exception: " << e.what());
		}
	}
	else if (!object_handler::write(offset, size, buffer))
	{
		ERROR("Attempts writing unaligned pages");
		return false;
	}
	return true;
}


bool BFS_interface::flush_cache(){

	if (local_offset == local_position)
		return true;

	if (!object_handler::get_latest(0)) return false;

	uint64_t object_size = object_handler::get_size(0);
	uint64_t page_size = object_handler::get_page_size();
	uint64_t right_part = local_position % page_size;

	INFO("Flush cache. right_part=" << right_part << " local_position =" << local_position << " object_size =" << object_size);
	if (right_part !=0 && local_position - right_part + page_size <= object_size)
	{
		object_handler::read(local_position - right_part, page_size, local_buffer + local_position -right_part, 0);
	}
	// Align the local_position anyways
	if (right_part != 0)
		local_position = local_position - right_part + page_size;
	if (object_handler::write(local_offset, local_position - local_offset, local_buffer))
	{
		//local_offset = local_position;
		cache_is_dirty =false;
		return true;
	}
	else
		return false;

}

bool BFS_interface::BFS_write(boost::uint64_t offset, boost::uint64_t size, char *buffer){

	if (cache_is_dirty == false)
	{
		local_offset = local_position = offset;
		//cache_is_dirty = true;
	}
	//If cache is empty
	if (cache_is_dirty == false && local_offset == local_position)
	{
		if(!object_handler::get_latest(0)) return false;
		uint64_t object_size = object_handler::get_size(0);
		uint64_t page_size = object_handler::get_page_size();
		uint64_t left_part = offset % page_size;

		INFO("Build cache when writing offset=" << offset << " size =" << size << " on object_size =" << object_size << " left_part =" << left_part);
		if ( left_part != 0)
		{
			local_offset = offset - left_part;
			INFO("Build cache, read one page from latest version");
			object_handler::read(local_offset, page_size, local_buffer, 0);
			local_position = local_offset + page_size;
			cache_is_dirty = true;
		}
	}

	// If no interleaving
	if ( offset + size < local_offset || offset > local_offset + local_buffer_size)
	{
		// Flush the cache
		// FLUSH()
		flush_cache();
		local_offset = local_position = 0;
		// Recursive write
		return BFS_write(offset, size, buffer);
	}
	// Else if there is an interleaving between local_buffer and buffer
	else
	{
		// Special case
		if (offset == local_offset && size > local_buffer_size)
		{
			uint64_t page_size = object_handler::get_page_size();
			uint64_t right_part = (offset + size) % page_size;
			if (!object_handler::write(offset, size - right_part, buffer)) return false;
			if (right_part !=0)
			{
				memcpy(local_buffer, buffer + size - right_part, right_part);
				local_offset = offset + size -right_part;
				local_position = offset + size;
				cache_is_dirty = true;
			}
			return true;
		}
		else if (offset < local_offset)
		{
			// Flush the left part
			if (!do_write(offset, local_offset - offset, buffer)) return false;

			// Loop the write for the right part
			return BFS_write(local_offset, offset + size - local_offset, buffer + local_offset - offset);
		}
		else if (offset <= local_position )
		{

			// If buffer is a part of local_buffer
			if (offset + size <= local_buffer_size + local_offset)
			{
				memcpy(local_buffer + offset - local_offset, buffer, size);
				// Set the new local_position
				local_position = offset + size > local_position ? offset + size : local_position;
				cache_is_dirty = true;
				return true;
			}
			else	//
			{
				memcpy(local_buffer + offset - local_offset, buffer, local_buffer_size + local_offset - offset);
				local_position = local_offset + local_buffer_size;
				// Call flush function. if OK, reset the local_offset
				// FLUSH()
				cache_is_dirty = true;
				flush_cache();
				local_offset = local_position;
				// Loop the write
				return BFS_write(local_offset, size + offset - local_offset, buffer + local_offset - offset);
			}
		}
		else if ( offset > local_position)
		{
			//FLUSH();
			flush_cache();
			local_offset = local_position = offset;
			// Loop the write
			return BFS_write(offset, size, buffer);
		}

	}
	return true;
}


bool BFS_interface::BFS_read(boost::uint64_t offset, boost::uint64_t size, char *buffer, boost::uint32_t version){
	INFO("Read operation");
	if (cache_is_dirty == true)
	{
		flush_cache();
		version = object_handler::get_version();
	}

	// Special case
	if (size > local_buffer_size)
	{
		local_offset = local_position = offset;
		return object_handler::read(offset, size, buffer, 0);
	}

	//	return object_handler::read(offset, size, buffer, version);
	//If cache is empty
	if (local_offset == local_position)
	{
		uint64_t object_size = object_handler::get_size(version);
		uint64_t page_size = object_handler::get_page_size();
		uint64_t left_part = offset % page_size;
		local_offset = offset - left_part;
		uint64_t read_size = local_offset + local_buffer_size > object_size ? object_size - local_offset : local_buffer_size;
		INFO("Build cache by filling it.");
		if (!object_handler::read(local_offset, read_size, local_buffer, version)) return false;
		local_position = local_offset + read_size;
	}

	// If no interleaving
	if ( offset + size < local_offset || offset > local_offset + local_buffer_size)
	{
		// Flush the cache
		// FLUSH()
		//flush_cache();
		local_offset = local_position;
		// Recursive write
		return BFS_read(offset, size, buffer, version);
	}
	// Else if there is an interleaving between local_buffer and buffer
	else
	{
		if (offset < local_offset)
		{
			// Read the left part
			if (!object_handler::read(offset, local_offset - offset, buffer)) return false;

			// Loop the read for the right part
			return BFS_read(local_offset, offset + size - local_offset, buffer + local_offset - offset, version);
		}
		else if (offset <= local_position )
		{
			// If buffer is a part of local_buffer
			if (offset + size <= local_buffer_size + local_offset)
			{
				memcpy(buffer, local_buffer + offset - local_offset, size);
				// Set the new local_position
				local_position = offset + size > local_position ? offset + size : local_position;
				return true;
			}
			else	//
			{
				memcpy(buffer, local_buffer + offset - local_offset, local_buffer_size + local_offset - offset);
				local_position = local_offset + local_buffer_size;
				// Call flush function. if OK, reset the local_offset
				// FLUSH()
				//flush_cache();
				local_offset = local_position;
				// Loop the write
				return BFS_read(local_offset, size + offset - local_offset, buffer + local_offset - offset, version);
			}
		}
		else if ( offset > local_position)
		{
			//FLUSH();
			//flush_cache();
			local_offset = local_position = offset;
			// Loop the write
			return BFS_read(offset, size, buffer, version);
		}

	}
	return true;
}


