#include <string>
#include <sstream>

// Monitoring includes
#include "provider_mon_listener.hpp"

// BlobSeer includes
#include "common/debug.hpp"
#include "common/structures.hpp"

#include "mon_utils/time_utils.hpp"
#include "mon_utils/formatting_utils.hpp"

provider_mon_listener::provider_mon_listener(const std::string &conf_file){
    try{
        apmon = new ApMon((char*)conf_file.c_str());
    } catch(runtime_error &e) {
        ERROR(e.what());
        apmon = NULL;
    }
    
}

provider_mon_listener::~provider_mon_listener() {    
    delete apmon;
}

void provider_mon_listener::update_event(const boost::int32_t name, const monitored_params_t &params) {
    switch (name) {
        case PROVIDER_WRITE:{
            write_access(params);
            break;
        }
        case PROVIDER_READ:{
            read_access(params);
            break;
        }
        default:
            break;
    }
}


void provider_mon_listener::read_access(const monitored_params_t &params) {
	if(!apmon)
        return;
	 try
	{
		stringstream ss;
		string buf;
		metadata::query_t range(0,0,0,0);
		params.get<1>().getValue(&range,true);
		
        std::string client_id = formatting_utils::format_client_id(params.get<3>());
        
        // page_id-----blob_id----watermark----page_size-------client------timestamp
        ss<<range<<"#"<<range.id<<"#"<<range.version<<"#"<<range.size<<"#"<<client_id<<"#"<<time_utils::get_current_time();
		buf = ss.str();
		
        apmon->sendParameter("Blob_IO", NULL, "provider_read",XDR_STRING, const_cast<char*>(buf.c_str()));
	}
	catch(runtime_error &e)
	{
        
	}
    
}

void provider_mon_listener::write_access(const monitored_params_t &params) {
	if(!apmon)
        return;
	try
	{
		stringstream ss;
		string buf;
		metadata::query_t range(0,0,0,0);
		params.get<1>().getValue(&range,true);
        
        std::string client_id = formatting_utils::format_client_id(params.get<3>());
        
        // blob_id----index---watermark----page_size-------client------timestamp
		ss<<range.id<<"#"<<range.offset<<"#"<<range.version<<"#"<<range.size<<"#"<<client_id<<"#"<<time_utils::get_current_time();
        buf = ss.str();
        
        apmon->sendParameter("Blob_IO", NULL, "provider_write",XDR_STRING, const_cast<char*>(buf.c_str()));
		
	}
	catch(runtime_error &e)
	{
        
	}
    
}

