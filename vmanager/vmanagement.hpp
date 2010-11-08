#ifndef __VMANAGEMENT
#define __VMANAGEMENT

#include <map>

#include "common/config.hpp"
#include "common/null_lock.hpp"
#include "common/structures.hpp"
#include "rpc/rpc_meta.hpp"

class vmanagement {
public:
    rpcreturn_t get_root(const rpcvector_t &params, rpcvector_t &result,
			 const std::string &sender);
    rpcreturn_t get_ticket(const rpcvector_t &params, rpcvector_t &result,
			   const std::string &sender);
    rpcreturn_t get_ticket_list(const rpcvector_t &params, rpcvector_t &result,
			   const std::string &sender);
    rpcreturn_t get_objcount(const rpcvector_t &params, rpcvector_t &result,
			     const std::string &sender);
    rpcreturn_t create(const rpcvector_t &params, rpcvector_t &result,
		       const std::string &sender);
    rpcreturn_t fcreate(const rpcvector_t &params, rpcvector_t &result,
		       const std::string &sender);
    rpcreturn_t publish(const rpcvector_t &params, rpcvector_t &result,
			const std::string &sender);
    rpcreturn_t clone(const rpcvector_t &params, rpcvector_t &result,
			const std::string &sender);

    ~vmanagement();
    vmanagement();
private:
    typedef boost::unordered_map<unsigned int, obj_info, boost::hash<unsigned int> > obj_hash_t;

    void compute_sibling_versions(metadata::siblings_enum_t &siblings,
				  metadata::query_t &edge_node,
				  obj_info::interval_list_t &intervals, 
				  boost::uint64_t root_size,
				  metadata::query_t except_this_query = metadata::query_t(0, 0, 0, 0));
    obj_hash_t obj_hash;
    boost::int32_t obj_count;

    // all operations are serialized and performed in the same thread.
    typedef null_lock::scoped_lock scoped_lock;
    null_lock mgr_lock;
};

#endif
