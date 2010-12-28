#ifndef __PROVIDER_MON_LISTENER
#define __PROVIDER_MON_LISTENER

// BlobSeer includes
#include "provider/page_manager.hpp"

// ApMon includes
#include "ApMon.h"

class provider_mon_listener {
    ApMon *apmon;

    void write_access(const monitored_params_t &params);
    void read_access(const monitored_params_t &params);
public:
    void update_event(const boost::int32_t name,const monitored_params_t &params);
    provider_mon_listener(const std::string &conf_file);
    ~provider_mon_listener();
};
#endif
