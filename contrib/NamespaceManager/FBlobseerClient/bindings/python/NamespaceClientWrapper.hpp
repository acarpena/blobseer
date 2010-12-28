/*
 * NamespaceClient.hpp
 *
 *  Created on: Sep 13, 2010
 *      Author: acarpena
 */

#ifndef NAMESPACECLIENTWRAPPER_HPP_
#define NAMESPACECLIENTWRAPPPER_HPP_

#include "common/NamespaceManConstants.hpp"
#include "FileHandlerWrapper.hpp"
#include "FBlobseerClient/NamespaceClient.hpp"

class NamespaceClientWrapper : public NamespaceClient {
public:
	NamespaceClientWrapper(const std::string &config_file);
	virtual ~NamespaceClientWrapper();

	//blob operations
	FileHandlerWrapper* createFile(const std::string path, boost::uint64_t page_size, boost::uint32_t replica_count = 1);
	FileHandlerWrapper* getFileHandler(const std::string path);

};

#endif /* NAMESPACECLIENTWRAPPER_HPP_ */
