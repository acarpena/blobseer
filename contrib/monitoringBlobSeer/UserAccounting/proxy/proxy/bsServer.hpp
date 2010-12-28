#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include "libconfig.h++"
//#include "vmanagement.hpp"
#include "common/config.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_meta.hpp"
#include "rpc/rpc_server.hpp"
using namespace std;
class myserver{
	public:
	myserver(){
		order=0;
	};
	
	~myserver(){
		
	};
	
	rpcreturn_t my_rpc_function(const rpcvector_t &params, rpcvector_t &result) {
		
		/*    if (params.size() != 1) {
		 ERROR("RPC error: wrong argument number");	
		 return rpcstatus::earg;
		 }
		 
		 
		 if (result.size() > 0) {
		 INFO("RPC success");
		 return rpcstatus::ok;
		 } else {
		 ERROR("RPC error: requested object " << query << " is unheard of");
		 return rpcstatus::eobj;
		 }
		 */
		
		std::string par;
		params.front().getValue(&par,true);
		cout << "BS server finally get:" << par;
		/*
		if (!par.compare("Hallo"))
		{
			std::string res("Bonjour");	
			result.push_back(buffer_wrapper(res, true));			
		}
		else
		{
			std::string res("Au revoir");
			
			result.push_back(buffer_wrapper(res, true));
		}*/
		result.push_back(buffer_wrapper(++order, true));		
		std::cout<<("In rpc ;)");	
		return rpcstatus::ok;			
	};
private:
	int order;	
};


