/*
 * NamespaceSimpleStorage.cpp
 *
 *  Created on: Mar 18, 2010
 *      Author: acarpena
 */
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include "NamespaceSimpleStorage.hpp"

#include "common/config.hpp"

NamespaceSimpleStorage::NamespaceSimpleStorage(const std::string &cfg):
			 mapStorage() {
	//scoped_lock lock(mgr_lock);
	// create the root directory
	ClientRequest initReq;
	Reply initRes;
	initReq.setPath("/");
	mkdir(initReq, initRes);

}

NamespaceSimpleStorage::NamespaceSimpleStorage(NamespaceSimpleStorage& nss):
			mapStorage(nss.getStorage()) {
	// copy constructor
}

NamespaceSimpleStorage::~NamespaceSimpleStorage() {
	// TODO Auto-generated destructor stub
}

NamespaceUnorderedMap NamespaceSimpleStorage::getStorage(void) {
	return mapStorage;
}

std::string NamespaceSimpleStorage::getConfigFile(void) {
	return configFile;
}

int NamespaceSimpleStorage::processRequest(const RequestType requestType, const ClientRequest req, Reply &result)
{

	switch(requestType) {

		case (NSMAN_MKDIR): {
			mkdir(req, result);
			break;
		}
		case (NSMAN_EXISTS): {
			exists(req, result);
			break;
		}
		case (NSMAN_ISFILE): {
			isFile(req, result);
			break;
		}
		case (NSMAN_RENAME): {
			rename(req, result);
			break;
		}
		case (NSMAN_CREATE): {
			create(req, result);
			break;
		}
		case (NSMAN_STATUS): {
			status(req, result);
			break;
		}
		case (NSMAN_DELETE): {
			deleteFile(req, result);
			break;
		}
		case (NSMAN_LISTDIR): {
			listDir(req, result);
			break;
		}
		case (NSMAN_BLOBID): {
			blobId(req, result);
			break;
		}
		default :{
			return rpcstatus::earg;
		}
	}


 return rpcstatus::ok;
}

void NamespaceSimpleStorage::mkdir(ClientRequest req, Reply &result)
{
	boost::char_separator<char> sep("/");
	std::string path = req.getPath();
	boost::tokenizer<boost::char_separator<char> > tokens(path, sep);
	boost::tokenizer<boost::char_separator<char> >::iterator iter;

	//scoped_lock lock(mgr_lock);
	DBG("Function Mkdir "<< req.getPath());
	// check if the full path already exists
	if (mapStorage.find(path) != mapStorage.end() )
	{
		DBG("Path "<< req.getPath() <<" already exists ");
		result.setExitCode(EEXISTS);
		return;
	}

	if (req.getPath() == "/") {
		FileMetadata md;
		md.setType(T_DIR);
		md.setPath("/");
		mapStorage["/"] = md;
	}
	// split the given path into tokens and check for each of them if
	// they are directories, if they exist and if the path itself is not already created
	std::string prefix;
	for (iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		std::string token = *iter;
		if (token.size() == 0)
		{
			continue;
		}

		// add a new token to the path
		prefix += "/" + token;
		// check if current prefix exists and it is a directory
		if (mapStorage.find(prefix) == mapStorage.end() )
		{
			// create it if it doesn't exist
			FileMetadata md;
			md.setType(T_DIR);
			md.setPath(prefix);
			mapStorage[prefix] = md;
		}

		FileMetadata md = mapStorage.at(prefix);
		// check if the path contains only directories
		if (md.getType() == T_FILE)
		{
			DBG("Path "<< req.getPath() <<" is a file ");
			result.setExitCode(ERRPATH);
			return;
		}

	}

	DBG("Directory "<< req.getPath() <<" created successfully ");
	result.setExitCode(SUCCESS);

}


void NamespaceSimpleStorage::exists(ClientRequest req, Reply &result)
{
	//scoped_lock lock(mgr_lock);
	result.setExitCode(SUCCESS);

	if (mapStorage.find(req.getPath()) == mapStorage.end() )
	{
		result.setOpResult(false);
		return;
	}

	result.setOpResult(true);


}

void NamespaceSimpleStorage::isFile(ClientRequest req, Reply &result)
{
	//scoped_lock lock(mgr_lock);
	exists(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == true))
	{
		FileMetadata md;
		try {
			md = mapStorage.at(req.getPath());
		}
		catch (std::out_of_range) {
			md = FileMetadata();
		}
		// the path exists but it's not a file
		if (md.getType() != T_FILE)
		{
			result.setOpResult(false);
		}
	}

}

void NamespaceSimpleStorage::rename(ClientRequest req, Reply &result)
{

	DBG("Function Rename "<< req.getPath()<< "; new path:" <<req.getNewPath());
	//scoped_lock lock(mgr_lock);
	// check if the given path is the root directory
	if (req.getPath() == "/")
	{

		DBG("Cannot rename root: "<< req.getPath());
		result.setExitCode(ERRPATH);
		return;
	}

	// check if the given path exists
	exists(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == false))
	{
		DBG("Path not found "<< req.getPath());
		result.setExitCode(ENOTFOUND);
		return;
	}

	// check if the new path is not already created
	ClientRequest newReq;
	newReq.setPath(req.getNewPath());
	exists(newReq, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == true))
	{
		DBG("Cannot rename into existing directory "<< req.getNewPath());
		result.setExitCode(EEXISTS);
		return;
	}

	std::string path = req.getPath();
	std::string newPath = req.getNewPath();

	isFile(req, result);
	if (result.getOpResult() == true)	//the path that has to be renamed is a file
	{
		DBG("Renaming file "<< req.getPath()<<"...");

		// delete the old filename and save the file in the new path
		FileMetadata md;
		try {
			md = mapStorage.at(path);
		}
		catch (std::out_of_range) {
			md = FileMetadata();
		}
		md.setPath(newPath);
		mapStorage.erase(path);
		mapStorage[newPath] = md;

		// create the directories that host the new file
		// (if the don't already exist)
		int position = path.rfind('/');
		std::string parentDir = path.substr(0, position);
		ClientRequest dirReq;
		dirReq.setPath(parentDir);
		mkdir(dirReq, result);
	}
	else		// the path to be renamed is a directory
	{
		DBG("Renaming directory "<< req.getPath() <<"...");

		std::vector<std::string> oldFiles;
        std::vector<FileMetadata> newFiles;
		// iterate through all the stored metadata
		BOOST_FOREACH(NamespaceUnorderedMap::value_type pair, mapStorage)
		{
			FileMetadata md = pair.second;
			std::string s = md.getPath();
			// if the current element has the old directory as a prefix
			// delete it from the metadata tree
			// and add the element with the new path as a prefix
			if (s.find(path) == 0 )
			{
				oldFiles.push_back(std::string(s));
				s.replace(0, path.length(), newPath);
				md.setPath(s);
                newFiles.push_back(FileMetadata(md));
			}
		}

		BOOST_FOREACH(std::string str, oldFiles)
		{
			mapStorage.erase(str);
		}
		BOOST_FOREACH(FileMetadata md, newFiles)
		{
			mapStorage[md.getPath()] = md;
		}
	}

	DBG("Rename successful for "<< req.getPath());
	result.setExitCode(SUCCESS);
}

void NamespaceSimpleStorage::create(ClientRequest req, Reply &result)
{
	exists(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == true))
	{
		result.setExitCode(EEXISTS);
		return;
	}

	// find the position of the last "/" in the path
	std::string path = req.getPath();
	int position = path.rfind('/');
	// the new filename
	std::string filename = path.substr(position + 1);
	// the directory that contains it
	std::string parentDir = path.substr(0, position);

	// try to create the parent directory
	ClientRequest dirReq;
	dirReq.setPath(parentDir);
	mkdir(dirReq, result);
	if ((result.getExitCode() != SUCCESS)&&(result.getExitCode() != EEXISTS))
	{
		// the directory could not be created and it doesn't already exist
		return;
	}

	// the file can be safely created
	FileMetadata md;
	md.setBlobId(req.getBlobId());
	md.setType(T_FILE);
	md.setPath(path);

	//scoped_lock lock(mgr_lock);
	mapStorage[path] = md;
	result.setExitCode(SUCCESS);
}

void NamespaceSimpleStorage::status(ClientRequest req, Reply &result)
{
	exists(req, result);
	// check if the path exists
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == true))
	{
		//scoped_lock lock(mgr_lock);
		FileMetadata md;
		try {
			md = mapStorage.at(req.getPath());
		}
		catch (std::out_of_range) {
			md = FileMetadata();
		}
		// add the metadata infos to the reply
		result.addInfo(md);
	}
}

void NamespaceSimpleStorage::deleteFile(ClientRequest req, Reply &result)
{
	// return error if the given path doesn't exist
	exists(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == false))
	{
		result.setExitCode(ENOTFOUND);
		return;
	}
	//scoped_lock lock(mgr_lock);
	std::string path = req.getPath();
	if (path == "/")
	{
		result.setExitCode(ERRPATH);
		return;
	}

	// delete the given path
	mapStorage.erase(path);

	// if the given path is a directory
	// delete its contents too
	isFile(req, result);
	if (result.getOpResult() == false)
	{
		std::vector<std::string> elemsToDelete;

		// iterate through all the stored metadata
		BOOST_FOREACH(NamespaceUnorderedMap::value_type pair, mapStorage)
		{
			FileMetadata md = pair.second;
			std::string s = md.getPath();
			// if the current element has the requested directory as a prefix
			// delete it from the metadata tree
			if (s.find(path) == 0 )
			{
				elemsToDelete.push_back(s);
			}
		}

		BOOST_FOREACH(std::string str, elemsToDelete)
		{
			mapStorage.erase(str);
		}
	}

}

void NamespaceSimpleStorage::listDir(ClientRequest req, Reply &result)
{
	DBG("Function Listdir "<< req.getPath());

	// return error if the given path is a file
	exists(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == false))
	{
		DBG("Path "<< req.getPath()<< " not found");
		result.setExitCode(ENOTFOUND);
		return;
	}

	// return error if the given path is a file
	isFile(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == true))
	{
		DBG("Path "<< req.getPath()<< " is not a valid directory");
		result.setExitCode(ERRPATH);
		return;
	}

	// the path is a directory => can be listed
	result.setExitCode(SUCCESS);


	DBG("Listing "<< req.getPath()<<"...");
	// iterate through all the stored metadata
	//scoped_lock lock(mgr_lock);
	std::string path = req.getPath();
	BOOST_FOREACH(NamespaceUnorderedMap::value_type pair, mapStorage)
	{
		FileMetadata md = pair.second;
		std::string s = md.getPath();
		//DBG("Iterating ("<< path<<")- now "<<s << " find="<<s.find(path) << "find after="<<s.find('/', path.length() +1));
		// if the current element has the requested directory as a prefix
		if ((s != path) && (s.find(path) == 0))
		{
			// if the current element is not at a deeper level in the metadata tree,
			// it can be listed
			if (s.find('/', path.length() + 1) == std::string::npos)
			{
				result.addInfo(md);
			}
		}
	}
}


void NamespaceSimpleStorage::blobId(ClientRequest req, Reply &result)
{
	// if the given path is a file
	isFile(req, result);
	if ((result.getExitCode() == SUCCESS) && (result.getOpResult() == true))
	{
		//scoped_lock lock(mgr_lock);
		FileMetadata md;
		try {
			md = mapStorage.at(req.getPath());
		}
		catch (std::out_of_range) {
			md = FileMetadata();
		}

		// return the file info in the result
		result.addInfo(FileMetadata(md));
	}
	else
	{
		result.setExitCode(ERRPATH);
	}

}


