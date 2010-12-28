/*
 * NamespaceManager.hpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#ifndef NAMESPACEMANAGER_HPP_
#define NAMESPACEMANAGER_HPP_

template <class NsManStorage>
class NamespaceManager {
public:
	rpcreturn_t namespaceOperations(const rpcvector_t &params, rpcvector_t &result, const std::string &sender);

	NamespaceManager(NsManStorage& nsStor): nsManStorage(nsStor)  {}
	virtual ~NamespaceManager() {}

private:
	NsManStorage nsManStorage;

};


/*
 * Method that serves requests from the clients and passes them to the namespace storage layer
 *
 */
template <class NsManStorage>
rpcreturn_t NamespaceManager<NsManStorage>::namespaceOperations(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {

	if (params.size() < 2) {
		return rpcstatus::earg;
	}

	//INFO("params ="<<params.size());
	//INFO("param[0] ="<<params[0]);

	// the first parameter is the type of the request
	// the second parameter is the request object
	RequestType requestType;
	ClientRequest req;
	if ((!params[0].getValue(&requestType, true)) || (!params[1].getValue(&req, true))) {
		ERROR(" RPC error: wrong arguments - cannot deserialize into a request type and a request");
		return rpcstatus::earg;
	}

	DBG("Received request: "<<(int)requestType << " for path: "<<req.getPath());

	rpcreturn_t reqResult;
	Reply reply;
	reply.setOpResult(1);
	reqResult = nsManStorage.processRequest(requestType, req, reply);
    result.push_back(buffer_wrapper(reply, true));

	INFO("[" << sender << "] RPC for request: "<<(int)requestType << " for path: "<<req.getPath() <<" success");
	return reqResult;
}



#endif /* NAMESPACEMANAGER_HPP_ */
