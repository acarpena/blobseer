#ifndef __PMANAGER_MON_LISTENER
#define __PMANAGER_MON_LISTENER

#include <vector>

// BlobSeer includes
#include "common/config.hpp"
#include "common/structures.hpp"

#include "adv_manager.hpp"

// ApMon includes
#include "ApMon.h"

class pmanager_mon_listener {
    ApMon *apmon;
    
    // Monitored operations
    void provider_manager_request(const monitored_params_t &params);
    
public:
    void update_event(const boost::int32_t name, const monitored_params_t &params);
    pmanager_mon_listener(const std::string &conf_file);
    ~pmanager_mon_listener();
};
#endif
