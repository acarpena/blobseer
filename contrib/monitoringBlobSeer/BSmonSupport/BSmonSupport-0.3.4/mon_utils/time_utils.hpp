#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;

namespace time_utils {
    
        boost::uint64_t get_current_time() {
        //get the current time from the clock
        boost::posix_time::ptime time_epoch(boost::gregorian::date(1970,1,1)); 
        boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());   
        
        boost::posix_time::time_duration timestamp = now - time_epoch;
        
        return timestamp.total_milliseconds();
    }
    
}
