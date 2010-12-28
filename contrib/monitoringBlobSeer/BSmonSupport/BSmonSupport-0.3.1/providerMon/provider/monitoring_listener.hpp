#ifndef __MONITORING_LISTENER
#define __MONITORING_LISTENER

#include <vector>

// BlobSeer includes
#include "common/config.hpp"
#include "provider/page_manager.hpp"
#include "common/structures.hpp"

// ApMon includes
#include "ApMon.h"

class monitoring_listener {
    ApMon *apmon;
    metadata::query_t *range;
    void write_access(const monitored_params_t &params);
    void read_access(const monitored_params_t &params);
public:
    void update_event(const boost::int32_t name,const monitored_params_t &params);
    monitoring_listener(const std::string &conf_file);
    ~monitoring_listener();
};
#endif
