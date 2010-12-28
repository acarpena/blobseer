#include <string>
#include <sstream>

// Monitoring includes
#include "metadata_mon_listener.hpp"

// BlobSeer includes
#include "common/debug.hpp"
#include "common/structures.hpp"

#include "mon_utils/time_utils.hpp"
#include "mon_utils/formatting_utils.hpp"

metadata_mon_listener::metadata_mon_listener(const std::string &conf_file){
    try{
        apmon = new ApMon((char*)conf_file.c_str());
    } catch(runtime_error &e) {
        ERROR(e.what());
        apmon = NULL;
    }
    
}

metadata_mon_listener::~metadata_mon_listener() {    
    delete apmon;
}

void metadata_mon_listener::update_event(const boost::int32_t name, const monitored_params_t &params) {
    switch (name) {
        case METADATA_NODES_READ:{
            metadata_nodes_read(params);
            break;
        } 
        case METADATA_LEAVES_READ:{
            metadata_leaves_read(params);
            break;
        }
        default:
            break;
    }
}



void metadata_mon_listener::metadata_nodes_read(const monitored_params_t &params) {
	if(!apmon)
        return;
	try
	{
		stringstream ss;
		string buf;
        
        boost::uint64_t requested_nodes;
        requested_nodes = params.get<2>();
        
        std::string client_id = formatting_utils::format_client_id(params.get<3>());
        // number of requested nodes----request timestamp (to filter duplicatessent to different ML farms)---client id
        ss<<requested_nodes<<"#"<<time_utils::get_current_time()<<"#"<<client_id;
        buf = ss.str();
        
        apmon->sendParameter("MetadataAcceses", NULL, "requested_nodes", XDR_STRING, const_cast<char*>(buf.c_str()));

	}
	catch(runtime_error &e)
	{
        
	}
    
}


void metadata_mon_listener::metadata_leaves_read(const monitored_params_t &params) {
	if(!apmon)
        return;
	try
	{
		stringstream ss;
		string buf;
         
        metadata::dhtnode_t node(false);
		// the first parameter is a buffer_wrapper(dht node id)
        bool is_dht_node = params.get<1>().getValue(&node, true);
        
        if (!is_dht_node) 
        {
            // the parameter is not a dht node => no monitoring information to send
            return;
        }
        
        // the leaf contains the page id returned to the client
        if (node.is_leaf)
        {
            std::string client_id = formatting_utils::format_client_id(params.get<3>());
            
            // page id----request timestamp (to filter duplicatessent to different ML farms)---client id
            ss<<node.left<<"#"<<time_utils::get_current_time()<<"#"<<client_id;
            buf = ss.str();
            
            apmon->sendParameter("MetadataAcceses", NULL, "requested_pageID", XDR_STRING, const_cast<char*>(buf.c_str()));            
            
        }
	}
	catch(runtime_error &e)
	{
	}
    
}

