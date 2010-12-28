/*
 * FileMetadata.hpp
 *
 *  Created on: Mar 18, 2010
 *      Author: acarpena
 */

#ifndef FILEMETADATA_HPP_
#define FILEMETADATA_HPP_

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class FileMetadata {
public:
	FileMetadata();
	FileMetadata(const FileMetadata& fm);
	virtual ~FileMetadata();
    boost::uint32_t getType() const;
    void setType(boost::uint32_t type);
    boost::uint64_t getBlobId() const;
    void setBlobId(boost::uint64_t blobId);
    std::string getPath() const;
    void setPath(std::string path);

private:
	std::string path;
	boost::uint32_t type;
	boost::uint64_t blobId;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
	    ar & path;
		ar & type;
	    ar & blobId;
	}
};

#endif /* FILEMETADATA_HPP_ */
