/*
 * NamespaceClient.hpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#ifndef NAMESPACECLIENT_HPP_
#define NAMESPACECLIENT_HPP_

#include "common/config.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_client.hpp"

#include "common/NamespaceManConstants.hpp"
#include "common/ClientRequest.hpp"
#include "common/Reply.hpp"
#include "FileHandler.hpp"

typedef rpc_client<config::socket_namespace> rpc_client_t;
typedef boost::int8_t RequestType;

class NamespaceClient {
public:
	NamespaceClient(const std::string &config_file);
	virtual ~NamespaceClient();

	// namespace operations

	boost::uint8_t mkdir(const std::string path);
	boost::uint8_t exists(const std::string path, bool& result);
	boost::uint8_t isFile(const std::string path, bool& result);
	boost::uint8_t rename(const std::string path, const std::string newPath);
	boost::uint8_t status(const std::string path, FileMetadata& md);
	boost::uint8_t deleteFile(const std::string path);
	boost::uint8_t listDir(const std::string path, std::vector<FileMetadata>& list);

	//blob operations
	FileHandler* createFile(const std::string path, boost::uint64_t page_size, boost::uint32_t replica_count = 1);
	FileHandler* getFileHandler(const std::string path);
	void destroyFileHandler(FileHandler* handler);

protected:
	// method that sends the requests through RPCs
	Reply sendNamespaceRequest(RequestType t, const ClientRequest& req);


	boost::asio::io_service ioService;
	rpc_client_t *directRpc;					// the RPC client used to connect to the Namespace Manager server

    std::string configFile;
	std::string namespaceServerHost, namespaceServerPort;	// host and port for the namespace server
};

#endif /* NAMESPACECLIENT_HPP_ */
