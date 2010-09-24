#include "vmanagement.hpp"
#include "main.hpp"
#define __DEBUG
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
// Only hold sibling of unpublished version
void vmanagement::compute_sibling_versions(vmgr_reply::siblings_enum_t &siblings,
		metadata::query_t &edge_node,
		obj_info::interval_list_t &intervals,
		boost::uint64_t root_size,
		metadata::query_t except_this_query) {
	metadata::query_t current_node = edge_node;
	while (current_node.size < root_size) {

		metadata::query_t brother = current_node;
		if (current_node.offset % (2 * current_node.size) == 0)
			brother.offset = current_node.offset + current_node.size;
		else {
			brother.offset = current_node.offset - current_node.size;
			current_node.offset = brother.offset;
		}
		DBG("edge_node " << edge_node << " except_this_query" << except_this_query << " current_node" << current_node << " brother " << brother);
		current_node.size *= 2;
		// current node is now the parent, brother is the direct sibling.
		for (obj_info::interval_list_t::reverse_iterator j = intervals.rbegin(); j != intervals.rend(); j++) {
			DBG("j->first " << j->first);

			if (!(j->first == except_this_query) && brother.intersects(j->first)) {
				brother.version = j->first.version;
				siblings.push_back(brother);
				DBG("Added " << brother << " to mgr_reply");
				break;
			}
		}
	}
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
		boost::uint32_t page_version = query.version;

		if (i != obj_hash.end()) {
			boost::uint64_t page_size = i->second.roots.back().page_size;

			// check if we are dealing with an append, adjust offset if it is the case
			if (append)
				query.offset = i->second.progress_size;

			// calculate the left and right leaf
			metadata::query_t left(query.id, query.version, (query.offset / page_size) * page_size, page_size);
			metadata::query_t right(query.id, query.version,
					((query.offset + query.size) / page_size - ((query.offset + query.size) % page_size == 0 ? 1 : 0)) * page_size,
					page_size);

			// if the WRITE will actually overflow, let the tree grow
			while (query.offset + query.size > i->second.max_size)
				i->second.max_size <<= 1;

			// reserve a ticket.
			mgr_reply.ticket = i->second.current_ticket++;
			mgr_reply.stable_root = i->second.roots.back();
			mgr_reply.root_size = i->second.max_size;
			mgr_reply.append_offset = query.offset;

			// compute left and right sibling versions.
			compute_sibling_versions(mgr_reply.left, left, i->second.intervals,
					i->second.max_size);
			compute_sibling_versions(mgr_reply.right, right, i->second.intervals,
					i->second.max_size);

			// insert this range in the uncompleted range queue.
			metadata::root_t new_root = i->second.roots.back();
			new_root.node.id = query.id;
			new_root.node.version = query.version = mgr_reply.ticket;
			new_root.node.size = i->second.max_size;
			if (query.offset + query.size > new_root.current_size) {
				new_root.current_size = query.offset + query.size;
				i->second.progress_size = new_root.current_size;
			}
			i->second.intervals.insert(obj_info::interval_entry_t(query, obj_info::root_flag_t(new_root, false, 0)));
			query.version = page_version;
		}
	}
	if (mgr_reply.ticket) {
		result.push_back(buffer_wrapper(mgr_reply, true));
		INFO("[" << sender << "] RPC success: allocated a new version (" << mgr_reply.ticket << ") for request " << query << " {CAV}");
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
	metadata::list_query_t list_range;
	vmgr_reply mgr_reply;

	if (!params[0].getValue(&list_range, true)) {
		ERROR("[" << sender << "] RPC error: wrong argument type");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(list_range.queries[0].id);
		boost::uint32_t page_version = list_range.queries[0].version;

		if (i != obj_hash.end()) {
			boost::uint64_t page_size = i->second.roots.back().page_size;

			// if the WRITE will actually overflow, let the tree grow
			while (list_range.queries.back().offset + list_range.queries.back().size > i->second.max_size)
				i->second.max_size <<= 1;

			// reserve a ticket.
			mgr_reply.ticket = i->second.current_ticket++;
			mgr_reply.stable_root = i->second.roots.back();
			mgr_reply.root_size = i->second.max_size;
			// mgr_reply.append_offset = query.offset;
			//boost::int32_t list_range_size = list_range.queries.size() -1;
			DBG("intervals size" << i->second.intervals.size());
			// insert this range in the uncompleted range queue.
			for (unsigned int ichunk = 0 ; ichunk < list_range.queries.size(); ichunk++) {
				metadata::root_t new_root = i->second.roots.back();
				new_root.node.id = list_range.queries[ichunk].id;
				new_root.node.version = list_range.queries[ichunk].version = mgr_reply.ticket;
				new_root.node.size = i->second.max_size;
				if (list_range.queries[ichunk].offset + list_range.queries[ichunk].size > new_root.current_size) {
					new_root.current_size = list_range.queries[ichunk].offset + list_range.queries[ichunk].size;
					i->second.progress_size = new_root.current_size;
				}

				//std::pair<obj_info::interval_list_t::iterator,bool> temp = i->second.intervals.insert(obj_info::interval_entry_t(list_range.queries[ichunk], obj_info::root_flag_t(new_root, false, ichunk*100)));
				i->second.intervals.insert(obj_info::interval_entry_t(list_range.queries[ichunk], obj_info::root_flag_t(new_root, false, ichunk)));

				DBG("Got this range" << list_range.queries[ichunk] << " is inserted :" );
				DBG("intervals size" << i->second.intervals.size());

			}

			for (unsigned int ichunk = 0 ; ichunk < list_range.queries.size(); ichunk++) {
				// calculate the left and right leaf
				metadata::query_t left(list_range.queries[ichunk].id, list_range.queries[ichunk].version, (list_range.queries[ichunk].offset / page_size) * page_size, page_size);
				metadata::query_t right(list_range.queries[ichunk].id, list_range.queries[ichunk].version,
						((list_range.queries[ichunk].offset + list_range.queries[ichunk].size) / page_size - ((list_range.queries[ichunk].offset + list_range.queries[ichunk].size) % page_size == 0 ? 1 : 0)) * page_size,
						page_size);


				// compute left and right sibling versions.
				DBG("COMPUTE left sibling versions");

				compute_sibling_versions(mgr_reply.left, left, i->second.intervals,
						i->second.max_size, list_range.queries[ichunk]);

				DBG("COMPUTE right sibling versions");

				compute_sibling_versions(mgr_reply.right, right, i->second.intervals,
						i->second.max_size, list_range.queries[ichunk]);

				// Undo change to list_range.queries[ichunk].version
				list_range.queries[ichunk].version = page_version;

			}

		}
	}
	if (mgr_reply.ticket) {
		result.push_back(buffer_wrapper(mgr_reply, true));
		INFO("[" << sender << "] RPC success: allocated a new version (" << mgr_reply.ticket << ") for request " << list_range << " {CAV}");
		return rpcstatus::ok;
	} else {
		ERROR("[" << sender << "] RPC failed: requested object " << list_range << " is unheard of");
		return rpcstatus::eobj;
	}
}

rpcreturn_t vmanagement::publish(const rpcvector_t &params, rpcvector_t & /*result*/, const std::string &sender) {
	if (params.size() != 1) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}
	metadata::query_t interval(0, 0, 0, 0);
	bool found = false;
	if (!params[0].getValue(&interval, true)) {
		ERROR("[" << sender << "] RPC error: wrong argument");
		return rpcstatus::earg;
	} else {
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(interval.id);
		if (i != obj_hash.end()) {
			obj_info::interval_list_t::iterator j = i->second.intervals.find(interval);
			// if this publish request has indeed reserved a ticket
			if (j != i->second.intervals.end()) {
				// mark the interval as completed
				j->second.get<1>() = true;
				// all consecutive completed intervals from the beginning will be discarded
				// and the last one will be the published version
				found = true;
				for (obj_info::interval_list_t::iterator k = i->second.intervals.begin();
						k != i->second.intervals.end() && k->second.get<1>();
						k = i->second.intervals.begin()) {

					boost::uint32_t ticket = k->second.get<0>().node.version;
					obj_info::interval_list_t::iterator h = k++;

					while (k != i->second.intervals.end() && k->second.get<0>().node.version == ticket) {
						i->second.intervals.erase(h);
						h = k++;
					}
					i->second.roots.push_back(h->second.get<0>());
					i->second.intervals.erase(h);
				}
			}
			DBG("latest published root = " << i->second.roots.back().node <<
					", current_size = " << i->second.roots.back().current_size);
		}
	}
	if (!found) {
		ERROR("[" << sender << "] RPC error: requested object " << interval << " is unheard of");
		return rpcstatus::eobj;
	} else {
		INFO("[" << sender << "] RPC success: marked " << interval << " as ready in the writer queue");
		return rpcstatus::ok;
	}
}

rpcreturn_t vmanagement::publish_list(const rpcvector_t &params, rpcvector_t & /*result*/, const std::string &sender) {
	if (params.size() != 1) {
		ERROR("[" << sender << "] RPC error: wrong argument number");
		return rpcstatus::earg;
	}

	metadata::list_query_t list_range;
	bool found = false;
	if (!params[0].getValue(&list_range, true)) {
		ERROR("[" << sender << "] RPC error: wrong argument");
		return rpcstatus::earg;
	} else {
		INFO("[" << sender << "] RPC success: got " << list_range <<  "to be published eventually");
		scoped_lock lock(mgr_lock);

		obj_hash_t::iterator i = obj_hash.find(list_range.queries[0].id);
		if (i != obj_hash.end()) {
			obj_info::interval_list_t::iterator j = i->second.intervals.find(list_range.queries[0]);
			// if this publish request has indeed reserved a ticket
			if (j != i->second.intervals.end()) {
				// mark the interval as completed
				j->second.get<1>() = true;
				// all consecutive completed intervals from the beginning will be discarded
				// and the last one will be the published version
				found = true;
				for (obj_info::interval_list_t::iterator k = i->second.intervals.begin();
						k != i->second.intervals.end() && k->second.get<1>();
						k = i->second.intervals.begin()) {

					boost::uint32_t ticket = k->second.get<0>().node.version;
					obj_info::interval_list_t::iterator h = k++;

					while (k != i->second.intervals.end() && k->second.get<0>().node.version == ticket) {
						i->second.intervals.erase(h);
						h = k++;
					}

					i->second.roots.push_back(h->second.get<0>());
					i->second.intervals.erase(h);
				}
			}
			DBG("latest published root = " << i->second.roots.back().node <<
					", current_size = " << i->second.roots.back().current_size);
		}
	}
	if (!found) {
		ERROR("[" << sender << "] RPC error: requested object " << list_range << " is unheard of");
		return rpcstatus::eobj;
	} else {
		INFO("[" << sender << "] RPC success: marked " << list_range << " as ready in the writer queue");
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
