#ifndef __METADATA_MON_LISTENER
#define __METADATA_MON_LISTENER

// BlobSeer includes
#include "provider/page_manager.hpp"

// ApMon includes
#include "ApMon.h"

class metadata_mon_listener {
    ApMon *apmon;

    void metadata_nodes_read(const monitored_params_t &params);
    void metadata_leaves_read(const monitored_params_t &params);
public:
    void update_event(const boost::int32_t name,const monitored_params_t &params);
    metadata_mon_listener(const std::string &conf_file);
    ~metadata_mon_listener();
};
#endif
