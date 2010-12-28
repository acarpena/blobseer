/*
 * FileHandler.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#include <sstream>
#include <libconfig.h++>

#include <boost/dynamic_bitset.hpp>

#include "common/debug.hpp"

#include "common/NamespaceManConstants.hpp"
#include "FileHandler.hpp"

/*
 * Constructor:
 * Initializes the object handler associated with the given path
 *
 */
FileHandler::FileHandler(const std::string &configFile, const boost::uint64_t blobid) :
objectHandler(configFile) {

	objectHandler.get_latest(blobid);

	local_buffer_size = objectHandler.get_page_size() * LOCAL_BUFFER_FACTOR;
	local_offset = 0;
	local_buffer = new char [ local_buffer_size ];
	memset(local_buffer,0,local_buffer_size);
	local_buffer_pointer = boost::shared_array<char>(local_buffer);
	local_position = 0;
	cache_is_dirty = false;
	INFO("Initialized the local buffer, size =" << local_buffer_size << " , offset =" << local_offset);
}

FileHandler::~FileHandler() {
	if (cache_is_dirty == true)
	{
		INFO("before flush destructor");
		flush_cache();
		INFO("after flush destructor");
	}
}


boost::int32_t FileHandler::get_objcount(){
	return objectHandler.get_objcount();
}

boost::uint64_t FileHandler::get_size(boost::uint32_t version) {

	return objectHandler.get_size(version);
}

boost::uint64_t FileHandler::getLatestVersion(void) {
	objectHandler.get_latest();
	return objectHandler.get_version();
}

boost::uint64_t FileHandler::get_page_size(void) {
	return objectHandler.get_page_size();
}


bool FileHandler::read(boost::uint64_t offset, boost::uint64_t size, char *buffer, boost::uint32_t version){
	INFO("Read operation");

	if (cache_is_dirty == true)
	{
		flush_cache();
		version = objectHandler.get_version();
	}

	// Special case
	if (size > local_buffer_size)
	{
		local_offset = local_position = offset;
		return objectHandler.read(offset, size, buffer, 0);
	}

	//	return objectHandler.read(offset, size, buffer, version);
	//If cache is empty
	if (local_offset == local_position)
	{
		uint64_t object_size = objectHandler.get_size(version);
		uint64_t page_size = objectHandler.get_page_size();
		uint64_t left_part = offset % page_size;
		local_offset = offset - left_part;
		uint64_t read_size = local_offset + local_buffer_size > object_size ? object_size - local_offset : local_buffer_size;
		INFO("Build cache by filling it.");
		if (!objectHandler.read(local_offset, read_size, local_buffer, version)) return false;
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
		return read(offset, size, buffer, version);
	}
	// Else if there is an interleaving between local_buffer and buffer
	else
	{
		if (offset < local_offset)
		{
			// Flush the left part
			if (!read(offset, local_offset - offset, buffer)) return false;

			// Loop the write for the right part
			return read(local_offset, offset + size - local_offset, buffer + local_offset - offset, version);
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
				return read(local_offset, size + offset - local_offset, buffer + local_offset - offset, version);
			}
		}
		else if ( offset > local_position)
		{
			//FLUSH();
			//flush_cache();
			local_offset = local_position = offset;
			// Loop the write
			read(offset, size, buffer, version);
		}

	}

}

bool FileHandler::append(boost::uint64_t size, char *buffer){

	if (!objectHandler.append(size, buffer)) {
		return false;
	}
	return true;
}

bool FileHandler::flush_cache(){

	if (local_offset == local_position)
		return true;

	uint64_t version = objectHandler.get_version();
	uint64_t object_size = objectHandler.get_size(version);
	uint64_t page_size = objectHandler.get_page_size();
	uint64_t right_part = local_position % page_size;
	INFO("Flush cache. right_part=" << right_part << " local_position =" << local_position << " object_size =" << object_size << " version =" << version );
	if (right_part !=0 && local_position - right_part + page_size <= object_size)
	{
		objectHandler.read(local_position - right_part, page_size, local_buffer + local_position -right_part, 0);
	}
	// Align the local_position anyways
	if (right_part != 0)
		local_position = local_position - right_part + page_size;
	INFO("Writing one page from local buffer. offset=" << local_offset << " local_position =" << local_position);
	if (objectHandler.write(local_offset, local_position - local_offset, local_buffer))
	{
		//local_offset = local_position;
		cache_is_dirty =false;
		return true;
	}
	else
		return false;

}

bool FileHandler::write(boost::uint64_t offset, boost::uint64_t size, char *buffer){

	if (cache_is_dirty == false)
	{
		local_offset = local_position = offset;
		//cache_is_dirty = true;
	}
	//If cache is empty
	if (cache_is_dirty == false && local_offset == local_position)
	{
		uint64_t version = objectHandler.get_version();
		uint64_t object_size = objectHandler.get_size(version);
		uint64_t page_size = objectHandler.get_page_size();
		uint64_t left_part = offset % page_size;

		INFO("Build cache when writing offset=" << offset << " size =" << size << " on object_size =" << object_size << " left_part =" << left_part);
		if ( left_part != 0)
		{
			local_offset = offset - left_part;
			INFO("Build cache, read one page from version=" << version);
			objectHandler.read(local_offset, page_size, local_buffer, 0);
			local_position = local_offset + page_size;
			cache_is_dirty = true;
		}
	}

	// If no interleaving
	if ( offset + size < local_offset || offset > local_offset + local_buffer_size)
	{
		// Flush the cache
		// FLUSH()
		INFO("before flush 1");
		flush_cache();
		INFO("after flush 1");
		local_offset = local_position = 0;
		// Recursive write
		return write(offset, size, buffer);
	}
	// Else if there is an interleaving between local_buffer and buffer
	else
	{
		// Special case
		if (offset == local_offset && size > local_buffer_size)
		{
			uint64_t page_size = objectHandler.get_page_size();
			uint64_t right_part = (offset + size) % page_size;
			if (!objectHandler.write(offset, size - right_part, buffer)) return false;
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
			return write(local_offset, offset + size - local_offset, buffer + local_offset - offset);
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
				INFO("before flush 2");
				 flush_cache();
				INFO("after flush 2");
				local_offset = local_position;
				// Loop the write
				return write(local_offset, size + offset - local_offset, buffer + local_offset - offset);
			}
		}
		else if ( offset > local_position)
		{
			//FLUSH();
			INFO("before flush 3");
			flush_cache();
			INFO("after flush 3");
			local_offset = local_position = offset;
			// Loop the write
			write(offset, size, buffer);
		}

	}
}

bool FileHandler::do_write(boost::uint64_t offset, boost::uint64_t size, char *buffer){

	uint64_t new_offset;
	// Update object to latest version
	uint64_t version = objectHandler.get_version();
	uint64_t object_size = objectHandler.get_size(version);
	uint64_t page_size = objectHandler.get_page_size();
	INFO("Write offset=" << offset << " size=" << size << " on object_size =" << object_size << " page_size = " << page_size << " version=" << version);
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
				objectHandler.read(offset -left_part, page_size, new_buffer, version);
			}
			// Read the right aligned page
			if (right_part !=0 && offset + size < object_size)
			{
				INFO("Read right");
				objectHandler.read(offset + size - (page_size - right_part), page_size, new_buffer + new_size - page_size, version);
			}
			memcpy(new_buffer + page_size - left_part, buffer, size);
			if (!objectHandler.write(offset -left_part, new_size, new_buffer))
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
	else if (!objectHandler.write(offset, size, buffer))
	{
		INFO("Bugs on writing without aligned pages");
		return false;
	}
	return true;

}



