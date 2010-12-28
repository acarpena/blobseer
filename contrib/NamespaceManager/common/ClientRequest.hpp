/*
 * ClientRequest.hpp
 *
 *  Created on: Mar 19, 2010
 *      Author: acarpena
 */

#ifndef CLIENTREQUEST_HPP_
#define CLIENTREQUEST_HPP_

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>


class ClientRequest {
public:
	ClientRequest();
	virtual ~ClientRequest();
    std::string getPath() const ;
    void setPath(std::string path);
    std::string getNewPath() const ;
    void setNewPath(std::string newPath);
    boost::uint64_t getBlobId() const ;
    void setBlobId(boost::uint64_t blobid);

private:
	std::string path;					// the path concerned by the request
	std::string newPath;				// new path (used for rename)
	boost::uint64_t blobid;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & path;
		ar & newPath;
	    ar & blobid;
	}


};



#endif /* CLIENTREQUEST_HPP_ */
