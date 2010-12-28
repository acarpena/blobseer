/*
 * NamespaceManager.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */

#include <iostream>
#include <fstream>
#include <boost/bind.hpp>

#include "libconfig.h++"

#include "common/debug.hpp"
#include "common/config.hpp"
#include "rpc/rpc_server.hpp"
#include "rpc/rpc_meta.hpp"

#include "common/NamespaceManConstants.hpp"
#include "storage/NamespaceSimpleStorage.hpp"
#include "NamespaceManager.hpp"

using namespace std;

int main(int argc, char *argv[]) {

	std::string port;
	std::string configFile;

    if (argc != 2) {
	cout << "Usage: " << configFile<< " <config_file>" << endl;
	cout << "Config file:\nNamespaceServer: {\n\tport = <port_no>\n};\n";
	return 1;
    }

    configFile = argv[1];
    libconfig::Config cfg;
    try {
	cfg.readFile(configFile.c_str());
	if (!(cfg.lookupValue("NamespaceServer.port", port)
	      //&& cfg.lookupValue("NamespaceServer.host", host)
	      //&& cfg.lookupValue("NamespaceServer.threads", threads)
		))
	    throw libconfig::ConfigException();
    } catch(libconfig::FileIOException &e) {

    	ERROR("I/O exception on config file");
    	return 2;
    } catch(libconfig::ParseException &e) {

    	ERROR("Parse exception (line " << e.getLine() << "): " << e.getError());
    	return 3;
    } catch(...) {
    	ERROR("Invalid configuration, check format (run without config file for help)");
    	return 4;
    }


    typedef NamespaceManager<NamespaceSimpleStorage> NamespaceManagerType;

    boost::asio::io_service io_service;
    rpc_server<config::socket_namespace> namespaceServer(io_service);
    NamespaceSimpleStorage nsStorage(configFile);
    NamespaceManagerType nsManager(nsStorage);

    namespaceServer.register_rpc(NSMAN_OPS,
			     (rpcserver_extcallback_t)boost::bind(&NamespaceManagerType::namespaceOperations,
								  boost::ref(nsManager), _1, _2, _3));

    namespaceServer.start_listening(config::socket_namespace::endpoint(config::socket_namespace::v4(),
								   atoi(port.c_str())));
    INFO("listening on " << namespaceServer.pretty_format_str());
    io_service.run();
    return 0;
}



