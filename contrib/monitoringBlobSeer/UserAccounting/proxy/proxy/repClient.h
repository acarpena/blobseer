#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h> 
#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <libconfig.h++>
#include <boost/bind.hpp>
#include "common/config.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_meta.hpp"
#include "rpc/rpc_client.hpp"
#define RPC_ID 5
#define SRV "paralapeche"
#define PORT 3333
using namespace std;

class repClient
{

private:
	int sockfd,n;
	struct sockaddr_in serv_addr;
    	struct hostent *server;
    	char buffer[256];
	
	void static proxy_callback(buffer_wrapper &params_for_server, const rpcreturn_t &error, const rpcvector_t &result)
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
	repClient()
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) 
			error("ERROR opening socket");
		server = gethostbyname(SRV);
		if (server == NULL)
			error("ERROR, no such host!");
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
		serv_addr.sin_port = htons(PORT);
	}

	void error(char *msg)
	{	
		cerr << msg << std::endl;
		exit(1);
	}

	std::string sendRequest()
	{
		if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
			error("ERROR connecting");
    		printf("Please enter the operator:");
    		bzero(buffer,256);
    		fgets(buffer,255,stdin);
    		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) 
			error("ERROR writing to socket");
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		if (n < 0) 
			error("ERROR reading from socket");
		//printf("%s\n",buffer);
		return std::string(buffer);
		
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
		direct_rpc.run();
	}
};


