/*
 * Reply.cpp
 *
 *  Created on: Mar 19, 2010
 *      Author: acarpena
 */

#include "Reply.hpp"

Reply::Reply() :
    requestExitCode(0), opResult(true), infos(){
	// TODO Auto-generated constructor stub

}

Reply::~Reply() {
	// TODO Auto-generated destructor stub
}

boost::uint8_t Reply::getExitCode() const
{
    return requestExitCode;
}

void Reply::setExitCode(boost::uint8_t reqExitCode)
{
    this->requestExitCode = reqExitCode;
}

bool Reply::getOpResult() const
{
    return opResult;
}

void Reply::setOpResult(bool value)
{
    this->opResult = value;
}

std::vector<FileMetadata> Reply::getInfos() const
{
    return infos;
}

void Reply::addInfo(FileMetadata info)
{
    this->infos.push_back(info);
}
