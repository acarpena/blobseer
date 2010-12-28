#include <string>
#include <sstream>

// Monitoring includes
#include "pmanager_mon_listener.hpp"

// BlobSeer includes
#include "common/debug.hpp"
#include "common/structures.hpp"
#include "common/buffer_wrapper.hpp"
#include "provider/provider_adv.hpp"

#include "publisher.hpp"
#include "mon_utils/time_utils.hpp"
#include "mon_utils/formatting_utils.hpp"

using namespace boost::posix_time;


pmanager_mon_listener::pmanager_mon_listener(const std::string &conf_file){
    try{
        apmon = new ApMon((char*)conf_file.c_str());
    } catch(runtime_error &e) {
        ERROR(e.what());
        apmon = NULL;
    }
    
}

pmanager_mon_listener::~pmanager_mon_listener() {    
    delete apmon;
}

void pmanager_mon_listener::update_event(const boost::int32_t name, const monitored_params_t &params) {
    switch (name) {
        case PUBLISHER_GET:{
            provider_manager_request(params);
            break;
        }
        default:
            break;
    }
}


void pmanager_mon_listener::provider_manager_request(const monitored_params_t &params) {
	if(!apmon)
        return;
	try
	{
        std::vector<provider_adv> provider_list; // the list of providers sent to the client
		// the first parameter is a buffer_wrapper(list of providers)
        bool result = params.get<0>().getValue(&provider_list,true);
        
        if (!result)    // deserialization failed
        {
            return;
        }
        
        for (int i=0; i< provider_list.size(); i++)
        {
            stringstream ss;
            string buf;
            
            provider_adv prov = provider_list[i];
            std::string client_id = formatting_utils::format_client_id(params.get<1>());
            // send the parameter: prov_id (hostname), requestId (a random timestamp), clientId
            ss<<prov.get_host()<<"#"<<time_utils::get_current_time()<<"#"<<client_id;
            buf = ss.str();        
            
            apmon->sendParameter("ProviderManager", NULL, "prov_request", XDR_STRING, const_cast<char*>(buf.c_str()));
        }
        
        
		
	}
	catch(runtime_error &e)
	{
        
	}
    
}
