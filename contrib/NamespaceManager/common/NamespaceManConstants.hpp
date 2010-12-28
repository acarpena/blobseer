/*
 * NamespaceServerRpcs.hpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#ifndef NAMESPACESERVERRPCS_HPP_
#define NAMESPACESERVERRPCS_HPP_


#include <boost/cstdint.hpp>

const boost::int32_t NSMAN_OPS = 1;

const boost::int32_t NSMAN_BLA = 1;

//namespace RequestType {
const boost::int8_t NSMAN_MKDIR = 0;
const boost::int8_t NSMAN_EXISTS = 1;
const boost::int8_t NSMAN_ISFILE = 2;
const boost::int8_t NSMAN_RENAME = 3;
const boost::int8_t NSMAN_CREATE = 4;
const boost::int8_t NSMAN_STATUS = 5;
const boost::int8_t NSMAN_DELETE = 6;
const boost::int8_t NSMAN_LISTDIR = 7;
const boost::int8_t NSMAN_BLOBID = 9;
//};

enum PathType {
	T_FILE,
	T_DIR
};

enum RequestExitCodes {
	SUCCESS,			// successful operation
	ENOTFOUND,			// error: path not found
	ERRPATH,			// error: incorrect path
	EEXISTS				// error: path already exists
};

#endif /* NAMESPACESERVERRPCS_HPP_ */
