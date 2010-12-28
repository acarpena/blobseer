/*
 * NamespaceSimpleStorage.hpp
 *
 *  Created on: Mar 18, 2010
 *      Author: acarpena
 */

#ifndef NAMESPACESIMPLESTORAGE_HPP_
#define NAMESPACESIMPLESTORAGE_HPP_

#include <boost/unordered_map.hpp>
#include "common/null_lock.hpp"
#include "rpc/rpc_meta.hpp"


#include "common/NamespaceManConstants.hpp"
#include "common/FileMetadata.hpp"
#include "common/ClientRequest.hpp"
#include "common/Reply.hpp"

typedef boost::unordered_map<std::string, FileMetadata> NamespaceUnorderedMap;
typedef boost::int8_t RequestType;

class NamespaceSimpleStorage {
public:

	NamespaceSimpleStorage(const std::string& cfg);
	NamespaceSimpleStorage(NamespaceSimpleStorage& nss );
	virtual ~NamespaceSimpleStorage();

	NamespaceUnorderedMap getStorage(void);
	std::string getConfigFile(void);

	int processRequest(const RequestType requestType, ClientRequest req, Reply &result);
	void mkdir(ClientRequest req, Reply &result);
	void exists(ClientRequest req, Reply &result);
	void isFile(ClientRequest req, Reply &result);
	void rename(ClientRequest req, Reply &result);
	void create(ClientRequest req, Reply &result);
	void status(ClientRequest req, Reply &result);
	void deleteFile(ClientRequest req, Reply &result);
	void listDir(ClientRequest req, Reply &result);
	void blobId(ClientRequest req, Reply &result);

private:
	NamespaceUnorderedMap mapStorage;
	std::string configFile;
	// all operations are serialized and performed in the same thread.
	typedef null_lock::scoped_lock scoped_lock;
	null_lock mgr_lock;

};

#endif /* NAMESPACESIMPLESTORAGE_HPP_ */
