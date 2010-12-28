#include <sstream>
#include <libconfig.h++>
#include <boost/bind.hpp>
#include "common/config.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_meta.hpp"
#include "rpc/rpc_client.hpp"
using namespace std;
#define RPC_ID 5

class bsClient
{
private:
	void proxy_callback(buffer_wrapper &params_for_server, const rpcreturn_t &error, const rpcvector_t &result)
	{
		printf("\nIn callback\n");
		std::string par;
		params_for_server.getValue(&par,true);
		std::cout <<"Sent to server:"<<par<<std::endl;
		std::string str;
		result.front().getValue(&str,true);
		std::cout <<"Received: "<<str <<std::endl;			  
		int nr;
		result.back().getValue(&nr,true);
		std::cout <<"Number: "<<nr <<std::endl;
	}
public:
	bsClient()
	{
		/*
		boost::asio::io_service io_service;
		rpc_client<config::socket_namespace, config::lock_t> direct_rpc(io_service);
		rpcvector_t read_params;
		rpcvector_t result;
		*/
	}
	void sendResult(std::string msg)
	{
		//std::string sir(msg);
		boost::asio::io_service io_service;
		rpc_client<config::socket_namespace, config::lock_t> direct_rpc(io_service);
		rpcvector_t read_params;
		rpcvector_t result;
		read_params.push_back(buffer_wrapper(msg,true));
		printf("Client:...wait for results....");
		//sleep(3);
		direct_rpc.dispatch("localhost", "55555", RPC_ID, read_params,
					boost::bind(&proxy_callback, read_params.back(), _1,_2));
		/*	
		std::string sir1("Bye");
		rpcvector_t read_params1;
		read_params1.push_back(buffer_wrapper(sir1,true));
		direct_rpc.dispatch("localhost", "55555", RPC_ID, 
							read_params1,
							boost::bind(&proxy_callback, read_params1.back(), _1,_2)
							);
		*/
		direct_rpc.run();
	}
};