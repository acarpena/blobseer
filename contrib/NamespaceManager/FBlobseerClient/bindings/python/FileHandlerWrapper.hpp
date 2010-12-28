/*
 * FileHandler.hpp
 *
 *  Created on: Sep 10, 2010
 *      Author: acarpena
 */

#ifndef FILEHANDLERWRAPPER_HPP_
#define FILEHANDLERWRAPPER_HPP_

#include <boost/python.hpp>
#include "FBlobseerClient/FileHandler.hpp"

class FileHandlerWrapper
{
public:
    FileHandlerWrapper(FileHandler* file);
    virtual ~FileHandlerWrapper();

    PyObject* read(boost::uint64_t offset, boost::uint64_t size, boost::uint32_t version);
    boost::int32_t get_objcount();
    boost::uint64_t get_size(boost::uint32_t version);
    boost::uint64_t getLatestVersion(void);
    boost::uint64_t get_page_size(void);
    bool append(boost::uint64_t size, char *buffer);
    bool write(boost::uint64_t offset, boost::uint64_t size, char *buffer);

private:
    FileHandler* fileHandler;

};

#endif /* FILEHANDLERWRAPPER_HPP_ */
