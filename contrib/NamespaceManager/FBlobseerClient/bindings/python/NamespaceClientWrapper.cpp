/*
 * NamespaceClientWrapper.cpp
 *
 *  Created on: Sep 13, 2010
 *      Author: acarpena
 */

#include "NamespaceClientWrapper.hpp"

NamespaceClientWrapper::NamespaceClientWrapper(const std::string &configFile) :
	NamespaceClient(configFile) {

}


NamespaceClientWrapper::~NamespaceClientWrapper() {

}

//blob operations
FileHandlerWrapper* NamespaceClientWrapper::createFile(const std::string path,
					boost::uint64_t page_size, boost::uint32_t replica_count){


	FileHandler* file = this->NamespaceClient::createFile(path, page_size, replica_count);
	if (file == 0)
	{
		DBG("New FielHandlerWrapper failed");
		return 0;
	}
	DBG("New FileHandlerWrapper created successfully");
	return new FileHandlerWrapper(file);
}


FileHandlerWrapper* NamespaceClientWrapper::getFileHandler(const std::string path){

	FileHandler* file = this->NamespaceClient::getFileHandler(path);
	if (file == 0)
	{
		DBG("Get FielHandlerWrapper failed");
		return 0;
	}
	DBG("Get FileHandlerWrapper succeeded");
	return new FileHandlerWrapper(file);
}


