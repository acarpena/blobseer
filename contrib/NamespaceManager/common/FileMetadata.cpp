/*
 * FileMetadata.cpp
 *
 *  Created on: Mar 18, 2010
 *      Author: acarpena
 */

#include "FileMetadata.hpp"

FileMetadata::FileMetadata() {
}

FileMetadata::FileMetadata(const FileMetadata& fm) {
	this->path = std::string(fm.getPath());
	this->blobId = fm.getBlobId();
	this->type = fm.getType();
}

FileMetadata::~FileMetadata() {
	// TODO Auto-generated destructor stub
}

std::string FileMetadata::getPath() const
{
    return path;
}

void FileMetadata::setPath(std::string path)
{
    this->path = path;
}

boost::uint32_t FileMetadata::getType() const
{
    return type;
}

void FileMetadata::setType(boost::uint32_t type)
{
    this->type = type;
}

boost::uint64_t FileMetadata::getBlobId() const
{
    return blobId;
}

void FileMetadata::setBlobId(boost::uint64_t blobId)
{
    this->blobId = blobId;
}


