#include <string>
#include<boost/tokenizer.hpp>

namespace formatting_utils {
    
    std::string format_client_id(std::string client) {
        
        // initial client string is "ip:port"
        boost::char_separator<char> separator(":");
        boost::tokenizer< boost::char_separator<char> > tok(client, separator);
                    
        // return just the IP address
        return *(tok.begin());
    }
           
}


