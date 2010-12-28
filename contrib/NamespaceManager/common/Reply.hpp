/*
 * Reply.hpp
 *
 *  Created on: Mar 19, 2010
 *      Author: acarpena
 */

#ifndef REPLY_HPP_
#define REPLY_HPP_

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>
#include <vector>

#include "FileMetadata.hpp"

class Reply {
public:
	Reply();
	virtual ~Reply();
    boost::uint8_t getExitCode() const ;
    void setExitCode(boost::uint8_t reqExitCode);
    bool getOpResult() const ;
    void setOpResult(bool value) ;
    std::vector<FileMetadata> getInfos() const ;
    void addInfo(FileMetadata info);

private:
	boost::uint8_t requestExitCode;
	bool opResult;
	std::vector<FileMetadata> infos;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
	    ar & requestExitCode;
	    ar & opResult;
	    ar & infos;
	}

};

#endif /* REPLY_HPP_ */
