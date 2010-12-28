/*
 * ClientRequest.cpp
 *
 *  Created on: Mar 19, 2010
 *      Author: acarpena
 */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest()  :
        path(""), newPath(""),blobid(0) {

}

ClientRequest::~ClientRequest() {
}

std::string ClientRequest::getPath() const
{
    return path;
}

void ClientRequest::setPath(std::string path)
{
    this->path = path;
}

std::string ClientRequest::getNewPath() const
{
    return newPath;
}

void ClientRequest::setNewPath(std::string newPath)
{
    this->newPath = newPath;
}

boost::uint64_t ClientRequest::getBlobId() const
{
	return blobid;
}

void ClientRequest::setBlobId(boost::uint64_t blobid) {
	this->blobid = blobid;
}
