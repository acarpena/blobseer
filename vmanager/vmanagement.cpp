#include "vmanagement.hpp"
#include "main.hpp"
//#define __DEBUG
#include "common/debug.hpp"

vmanagement::vmanagement() : obj_count(0) {
}

vmanagement::~vmanagement() {
}

rpcreturn_t vmanagement::get_root(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {
	if (params.size() != 2) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}
	metadata::root_t last_root(0, 0, 0, 0, 0);
	boost::uint32_t id, version;
	if (!params[0].getValue(&id, true) || !params[1].getValue(&version, true)) {
		ERROR("[" << sender << "] RPC error: wrong argument");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(id);
		if (i != obj_hash.end()) {
			if (version == 0 || version >= i->second.roots.size())
				last_root = i->second.roots.back();
			else
				last_root = i->second.roots[version];
		}
	}
	INFO("[" << sender << "] RPC success: root request for " << version << " completed: " << last_root.node);
	result.push_back(buffer_wrapper(last_root, true));

	return rpcstatus::ok;
}

rpcreturn_t vmanagement::clone(const rpcvector_t &params, rpcvector_t &result, 
		const std::string &sender) {
	if (params.size() != 2) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}
	metadata::root_t last_root(0, 0, 0, 0, 0);
	boost::uint32_t id, version;

	if (!params[0].getValue(&id, true) || !params[1].getValue(&version, true)) {
		ERROR("[" << sender << "] RPC error: wrong argument");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(id);
		if (i != obj_hash.end()) {
			if (version == 0 || version >= i->second.roots.size())
				last_root = i->second.roots.back();
			else
				last_root = i->second.roots[version];

			unsigned int id = ++obj_count;
			obj_info new_obj(last_root);
			obj_hash.insert(std::pair<unsigned int, obj_info>(id, new_obj));
			last_root.node.id = id;
		}
	}
	INFO("[" << sender << "] RPC success: new clone generated for (" <<
			id << ", " << version << "): (" << id << ", 0)");
	result.push_back(buffer_wrapper(last_root, true));

	return rpcstatus::ok;
}

rpcreturn_t vmanagement::get_ticket(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {
	if (params.size() != 2) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}
	metadata::query_t query;
	vmgr_reply mgr_reply;
	bool append;

	if (!params[0].getValue(&query, true) || !params[1].getValue(&append, true)) {
		ERROR("[" << sender << "] RPC error: at least one argument is wrong");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(query.id);

		if (i != obj_hash.end()) {
			boost::uint64_t page_version = query.version;
			// check if we are dealing with an append, adjust offset if it is the case
			if (append)
				query.offset = i->second.progress_size;

			// if the WRITE will actually overflow, let the tree grow
			while (query.offset + query.size > i->second.max_size)
				i->second.max_size <<= 1;

			// insert this range in the uncompleted range queue.
			metadata::root_t new_root = i->second.roots.back();
			new_root.node.id = query.id;
			new_root.node.version = query.version = i->second.current_ticket++;
			new_root.node.size = i->second.max_size;
			if (query.offset + query.size > new_root.current_size) {
				new_root.current_size = query.offset + query.size;
				i->second.progress_size = new_root.current_size;
			}
			metadata::list_query_t list_query;
			list_query.queries.push_back(query);

			i->second.intervals.insert(obj_info::interval_entry_t(new_root.node.version, obj_info::list_query_root_flag_t(list_query, obj_info::root_flag_t(new_root, false))));

			mgr_reply.stable_root = i->second.roots.back();
			mgr_reply.root_size = i->second.max_size;
			mgr_reply.intervals = i->second.intervals;

			query.version = page_version;
			//INFO("[" << sender << " ] list_query " << list_query);
		}
	}
	if (mgr_reply.intervals.size() > 0) {
		result.push_back(buffer_wrapper(mgr_reply, true));
		INFO("[" << sender << "] RPC success: allocated a new version (" << mgr_reply.intervals.rbegin()->second.first
				<< ") for request " << query << " {CAV}");
		return rpcstatus::ok;
	} else {
		ERROR("[" << sender << "] RPC failed: requested object " << query << " is unheard of");
		return rpcstatus::eobj;
	}
}
rpcreturn_t vmanagement::get_ticket_list(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {
	if (params.size() != 1) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}
	metadata::list_query_t list_query;
	metadata::query_t query;
	vmgr_reply mgr_reply;

	if (!params[0].getValue(&list_query, true)) {
		ERROR("[" << sender << "] RPC error: wrong argument type");
		return rpcstatus::earg;
	} else {
		query = list_query.queries.back();
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(query.id);

		if (i != obj_hash.end()) {
			boost::uint64_t page_version = query.version;

			// if the WRITE will actually overflow, let the tree grow
			while (query.offset + query.size > i->second.max_size)
				i->second.max_size <<= 1;

			// insert this range in the uncompleted range queue.
			metadata::root_t new_root = i->second.roots.back();
			new_root.node.id = query.id;
			new_root.node.version = i->second.current_ticket++;
			for (unsigned int index = 0; index < list_query.queries.size(); index++)
				list_query.queries[index].version = new_root.node.version;
			new_root.node.size = i->second.max_size;
			if (query.offset + query.size > new_root.current_size) {
				new_root.current_size = query.offset + query.size;
				i->second.progress_size = new_root.current_size;
			}

			i->second.intervals.insert(obj_info::interval_entry_t(new_root.node.version, obj_info::list_query_root_flag_t(list_query, obj_info::root_flag_t(new_root, false))));

			mgr_reply.stable_root = i->second.roots.back();
			mgr_reply.root_size = i->second.max_size;
			mgr_reply.intervals = i->second.intervals;

			for (unsigned int index = 0; index < list_query.queries.size(); index++)
				list_query.queries[index].version = page_version;
		}
	}
	if (mgr_reply.intervals.size() > 0) {
		result.push_back(buffer_wrapper(mgr_reply, true));
		INFO("[" << sender << "] RPC success: allocated a new version (" << mgr_reply.intervals.rbegin()->first
				<< ") for request " << list_query << " {CAV}");
		return rpcstatus::ok;
	} else {
		ERROR("[" << sender << "] RPC failed: requested object " << query << " is unheard of");
		return rpcstatus::eobj;
	}
}

rpcreturn_t vmanagement::publish(const rpcvector_t &params, rpcvector_t & /*result*/, const std::string &sender) {
	if (params.size() != 2) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}
	//metadata::query_t interval(0, 0, 0, 0);
	int published_version = 0;
	int id = 0;
	bool found = false;
	if (!params[0].getValue(&id, true) || !params[1].getValue(&published_version, true) ) {
		ERROR("[" << sender << "] RPC error: wrong argument");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(id);
		if (i != obj_hash.end()) {
		    obj_info::interval_list_t::iterator j = i->second.intervals.find(published_version);
		    // if this publish request has indeed reserved a ticket
		    if (j != i->second.intervals.end()) {
			// mark the interval as completed
			j->second.second.second = true;
			// all consecutive completed intervals from the beginning will be discarded
			// and the last one will be the published version
			found = true;
			for (obj_info::interval_list_t::iterator k = i->second.intervals.begin();
			     k != i->second.intervals.end() && k->second.second.second;
			     k = i->second.intervals.begin()) {
			    i->second.roots.push_back(k->second.second.first);
			    i->second.intervals.erase(k);
			}
		    }
		    DBG("latest published root = " << i->second.roots.back().node <<
			", current_size = " << i->second.roots.back().current_size);
		}
	    }
	    if (!found) {
		ERROR("[" << sender << "] RPC error: requested object id " << id << ": version " << published_version << " is unheard of");
		return rpcstatus::eobj;
	    } else {
		INFO("[" << sender << "] RPC success: marked id " << id << ": version " << published_version <<  " as ready in the writer queue");
		return rpcstatus::ok;
	    }
}

rpcreturn_t vmanagement::create(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {
	if (params.size() != 2) {
		ERROR("[" << sender << "] RPC error: wrong argument number, required: page size, replication count");
		return rpcstatus::earg;
	}

	boost::uint64_t ps;
	boost::uint32_t rc;
	if (!params[0].getValue(&ps, true) || !params[1].getValue(&rc, true)) {
		ERROR("[" << sender << "] RPC error: wrong arguments");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		unsigned int id = ++obj_count;
		obj_info new_obj(id, ps, rc);
		obj_hash.insert(std::pair<unsigned int, obj_info>(id, new_obj));
		result.push_back(buffer_wrapper(new_obj.roots.back(), true));
	}

	INFO("[" << sender << "] RPC success: created a new blob: (" << obj_count << ", " << ps << ", " << rc << ") {CCB}");
	return rpcstatus::ok;
}

rpcreturn_t vmanagement::fcreate(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {
	if (params.size() != 3) {
		ERROR("[" << sender << "] RPC error: wrong argument number, required: blob id, page size, replication count");
		return rpcstatus::earg;
	}

	boost::uint32_t id;
	boost::uint64_t ps;
	boost::uint32_t rc;
	if (!params[0].getValue(&id, true) || !params[1].getValue(&ps, true) || !params[2].getValue(&rc, true)) {
		ERROR("[" << sender << "] RPC error: wrong arguments");
		return rpcstatus::earg;
	} else {

		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(id);
		if (i != obj_hash.end()) {
			metadata::root_t last_root(0, 0, 0, 0, 0);
			last_root = i->second.roots.back();
			result.push_back(buffer_wrapper(last_root, true));
			INFO("[" << sender << "] RPC success: create to open completed: " << last_root.node);
		}
		else {
			obj_info new_obj(id, ps, rc);
			obj_hash.insert(std::pair<unsigned int, obj_info>(id, new_obj));
			++obj_count;
			result.push_back(buffer_wrapper(new_obj.roots.back(), true));
			INFO("[" << sender << "] RPC success: created a new blob: (" << id << ", " << ps << ", " << rc << ") {CCB}");
		}
	}
	return rpcstatus::ok;
}


rpcreturn_t vmanagement::get_objcount(const rpcvector_t &params, rpcvector_t &result, const std::string &sender) {
	if (params.size() != 0) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}

	result.push_back(buffer_wrapper(obj_count, true));
	INFO("[" << sender << "] RPC success: number of blobs is " << obj_count);
	return rpcstatus::ok;
}
