#include <iostream>
#include <fstream>

#include <boost/bind.hpp>

#include "libconfig.h++"
//#include "vmanagement.hpp"

#include "common/config.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_meta.hpp"
#include "rpc/rpc_server.hpp"
//#include "main.hpp"

#include "bsServer.hpp"

using namespace std;


#define RPC_ID 5






int main(int argc, char *argv[]) {   
    std::string host, service;

   
    boost::asio::io_service io_service;
    rpc_server<config::socket_namespace, config::lock_t> vmgr_server(io_service);
  
	myserver srv;
	

    vmgr_server.register_rpc(RPC_ID,
				(rpcserver_callback_t)boost::bind(&myserver::my_rpc_function, boost::ref(srv), _1, _2));



/*    vmgr_server.register_rpc(VMGR_GETTICKET,
				(rpcserver_callback_t)boost::bind(&vmanagement::getTicket, boost::ref(vmgr), _1, _2));
    vmgr_server.register_rpc(VMGR_CREATE,
				(rpcserver_callback_t)boost::bind(&vmanagement::create, boost::ref(vmgr), _1, _2));
    vmgr_server.register_rpc(VMGR_LASTVER,
				(rpcserver_callback_t)boost::bind(&vmanagement::getVersion, boost::ref(vmgr), _1, _2));
    vmgr_server.register_rpc(VMGR_PUBLISH,
				(rpcserver_callback_t)boost::bind(&vmanagement::publish, boost::ref(vmgr), _1, _2));
    vmgr_server.register_rpc(VMGR_GETOBJNO,
				(rpcserver_callback_t)boost::bind(&vmanagement::get_objcount, boost::ref(vmgr), _1, _2));
  */
	
	
	
	
	
	
	vmgr_server.start_listening("localhost","55555");//config::socket_namespace::endpoint(config::socket_namespace::v4(), 55555));
    printf("listening \n");
    io_service.run();
    return 0;
}
