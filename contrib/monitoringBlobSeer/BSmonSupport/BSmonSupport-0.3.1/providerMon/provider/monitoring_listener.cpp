#include <string>
#include <sstream>
#include "boost/date_time/posix_time/posix_time.hpp"

// Monitoring includes
#include "monitoring_listener.hpp"

// BlobSeer includes
#include "common/debug.hpp"
#include "common/structures.hpp"

using namespace boost::posix_time;


monitoring_listener::monitoring_listener(const std::string &conf_file){
    try{
        apmon = new ApMon((char*)conf_file.c_str());
    } catch(runtime_error &e) {
        ERROR(e.what());
        apmon = NULL;
    }
    
}

monitoring_listener::~monitoring_listener() {    
    delete apmon;
}

void monitoring_listener::update_event(const boost::int32_t name, const monitored_params_t &params) {
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
            ERROR("Unknown hook type: " << name);
    }
}


void monitoring_listener::read_access(const monitored_params_t &params) {
	if(!apmon)
            return;
	try
	{
		stringstream ss;
		string buf;
		range=new metadata::query_t(0,0,0,0);
		params.get<1>().getValue(range,true);
		
        //get the current time from the clock
        boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());                
        
        ss<<range->id<<"#"<<range->offset<<"#"<<params.get<3>()<<now;
		buf = ss.str();
		
        apmon->sendParameter("Blob_IO", NULL, "provider_read",XDR_STRING, const_cast<char*>(buf.c_str()));
	}
	catch(runtime_error &e)
	{
	
	}

}

void monitoring_listener::write_access(const monitored_params_t &params) {
	if(!apmon)
            return;
	try
	{
		stringstream ss;
		string buf;
		range=new metadata::query_t(0,0,0,0);
		params.get<1>().getValue(range,true);
        
        //get the current time from the clock
        boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());   
        
		ss<<range->id<<"#"<<range->offset<<"#"<<range->version<<"#"<<params.get<3>()<<"#"<<now;
        buf = ss.str();
        
        apmon->sendParameter("Blob_IO", NULL, "provider_write",XDR_STRING, const_cast<char*>(buf.c_str()));
		
	}
	catch(runtime_error &e)
	{
	
	}
    
}

