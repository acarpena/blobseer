/*
 * NamespaceClient.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#include <sstream>
#include <libconfig.h++>

#include <boost/dynamic_bitset.hpp>

#include "common/debug.hpp"

#include "NamespaceClient.hpp"

NamespaceClient::NamespaceClient(const std::string &configFile) :
						ioService(), directRpc(new rpc_client_t(ioService)), configFile(configFile) {
	libconfig::Config cfg;

	try {
		cfg.readFile(configFile.c_str());
		// get NamespaceManager host and port
		if (!cfg.lookupValue("NamespaceServer.host", namespaceServerHost) ||
				!cfg.lookupValue("NamespaceServer.port", namespaceServerPort))
			throw std::runtime_error("NamespaceClient::NamespaceClient(): NamespaceServer parameters are missing in the configuration file");

	} catch(libconfig::FileIOException) {
		throw std::runtime_error("NamespaceClient::NamespaceClient(): I/O error trying to parse config file: " + configFile);

	} catch(libconfig::ParseException &e) {
		std::ostringstream ss;
		ss << "NamespaceClient::NamespaceClient(): Parse exception (line " << e.getLine() << "): " << e.getError();
		throw std::runtime_error(ss.str());

	} catch(std::runtime_error &e) {
		throw e;
	} catch(...) {
		throw std::runtime_error("NamespaceClient::NamespaceClient(): Unknown exception");
	}

}

NamespaceClient::~NamespaceClient() {
}


template <class T>
static void rpcCallback(bool &res, T &output, const rpcreturn_t &error, const rpcvector_t &result) {

	if (error == rpcstatus::ok && result.size() == 1 && result[0].getValue(&output, true)) {
    	DBG("RPC success");
    	return;
    	}
    res = false;
    ERROR("Could not perform RPC to NamesapceManager successfully; result is: " << result.size() << "RPC status is: " << error);
}


Reply NamespaceClient::sendNamespaceRequest(RequestType t, const ClientRequest& req) {

	Reply reply;
	bool result = true;

	rpcvector_t params;

	DBG ("sending request: " <<(int)t);
	params.clear();
	params.push_back(buffer_wrapper(t, true));
	params.push_back(buffer_wrapper(req, true));

	directRpc->dispatch(namespaceServerHost, namespaceServerPort, NSMAN_OPS, params,
			boost::bind(&rpcCallback<Reply>, boost::ref(result), boost::ref(reply), _1, _2));

	directRpc->run();

	return reply;
}

boost::uint8_t NamespaceClient::mkdir(const std::string path){

	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_MKDIR, req);
	return reply.getExitCode();
}

boost::uint8_t NamespaceClient::exists(const std::string path, bool& result){

	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_EXISTS, req);

	result = reply.getOpResult();
	return reply.getExitCode();
}

boost::uint8_t NamespaceClient::isFile(const std::string path, bool& result){
	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_ISFILE, req);
	result = reply.getOpResult();

	return reply.getExitCode();
}

boost::uint8_t NamespaceClient::rename(const std::string path, const std::string newPath){

	ClientRequest req;
	req.setPath(path);
	req.setNewPath(newPath);

	Reply reply = sendNamespaceRequest(NSMAN_RENAME, req);

	return reply.getExitCode();
}

boost::uint8_t NamespaceClient::status(const std::string path, FileMetadata& md){

	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_STATUS, req);
	rpcreturn_t retCode = reply.getExitCode();

	if (retCode == SUCCESS) {
		// the request returned some metadata
		md = reply.getInfos().front();
	}
	else {
		// the request was not successful
		md = FileMetadata();
	}

	return reply.getExitCode();
}


boost::uint8_t NamespaceClient::deleteFile(const std::string path){

	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_DELETE, req);

	return reply.getExitCode();
}


boost::uint8_t NamespaceClient::listDir(const std::string path, std::vector<FileMetadata>& list){

	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_LISTDIR, req);

	rpcreturn_t retCode = reply.getExitCode();
	list = reply.getInfos();

	return retCode;
}



//blob operations
FileHandler* NamespaceClient::createFile(const std::string path,
					boost::uint64_t page_size, boost::uint32_t replica_count){

	object_handler *objHandler = new object_handler(configFile);

	if (!objHandler->create(page_size, replica_count))
	{
		DBG("Namespace CreateFile failed at Blob Create ");
		// initialize a FileHandler without a blobId
		return 0;
	}

	boost::uint64_t blobid = objHandler->get_id();
	DBG("Blob created successfully: blobid="<<blobid);
	delete objHandler;

	ClientRequest req;
	req.setPath(path);
	req.setBlobId(blobid);

	Reply reply = sendNamespaceRequest(NSMAN_CREATE, req);

	rpcreturn_t retCode = reply.getExitCode();
	if (retCode != SUCCESS)
	{
		DBG("Namespace CreateFile failed: error code is "<< retCode);
		// initialize a FileHandler without a blobId
		return 0;
	}

	return new FileHandler(configFile, blobid);
}


FileHandler* NamespaceClient::getFileHandler(const std::string path){

	ClientRequest req;
	req.setPath(path);

	Reply reply = sendNamespaceRequest(NSMAN_BLOBID, req);

	rpcreturn_t retCode = reply.getExitCode();
	if (retCode != SUCCESS)
	{
		DBG("GetFileHandler failed. Error code is "<< retCode);
		// initialize a FileHandler without a blobId
		return 0;
	}

	// blobID obtained correctly
	FileMetadata md = reply.getInfos().front();
	if (md.getType() == T_DIR)
	{
		DBG("GetFileHandler failed. The requested file is a directory.");
		// initialize a FileHandler without a blobId
		return 0;
	}
	return new FileHandler(configFile, md.getBlobId());
}

void NamespaceClient::destroyFileHandler(FileHandler* handler) {

	delete handler;
}


