#include "pnw_object.hpp"

using namespace std;

pnw_object::pnw_object(const std::string &config_file) :object_handler(config_file)
{
}

bool pnw_object::exec_write(boost::uint64_t offset, boost::uint64_t size, char *buffer, bool append) 
{
	if (latest_root.page_size == 0)
		throw std::runtime_error("object_handler::write(): write attempt on unallocated/uninitialized object");
	ASSERT(offset % latest_root.page_size == 0 && size % latest_root.page_size == 0);

	TIMER_START(write_timer);
	bool result = true;

	metadata::query_t range(id, rnd(), offset, size);
	boost::uint64_t page_size = latest_root.page_size;
	unsigned int replica_count = latest_root.replica_count;

	std::vector<provider_adv> adv;
	interval_range_query::node_deque_t node_deque;
	rpcvector_t params;

	cout << "PNW client!!" << endl;
	
	
    // try to get a list of providers
	TIMER_START(publisher_timer);
	params.clear();
	params.push_back(buffer_wrapper((size / page_size) * replica_count, true));
	direct_rpc->dispatch(publisher_host, publisher_service, PUBLISHER_GET, params,
			     boost::bind(&rpc_get_serialized<std::vector<provider_adv> >, boost::ref(result), boost::ref(adv), _1, _2));
	direct_rpc->run();
	TIMER_STOP(publisher_timer, "WRITE " << range << ": PUBLISHER_GET, result: " << result);
	if (!result || adv.size() < (size / page_size) * replica_count)
		return false;

// get a ticket from the version manager
	params.clear();
	params.push_back(buffer_wrapper(range, true));
	params.push_back(buffer_wrapper(append, true));

	vmgr_reply mgr_reply;
	TIMER_START(ticket_timer);
	direct_rpc->dispatch(vmgr_host, vmgr_service, VMGR_GETTICKET, params,
				bind(&rpc_get_serialized<vmgr_reply>, boost::ref(result), boost::ref(mgr_reply), _1, _2));
	direct_rpc->run();
	TIMER_STOP(ticket_timer, "WRITE " << range << ": VMGR_GETTICKET, result: " << result);
	if (!result)
		return false;

// construct the set of leaves to be written to the metadata
	range.offset = offset = mgr_reply.append_offset;
	range.version = mgr_reply.ticket;
	for (boost::uint64_t i = offset; i < offset + size; i += page_size)
		node_deque.push_back(metadata::query_t(id, mgr_reply.ticket, i, page_size));

	TIMER_START(metadata_timer);
	result = query->writeRecordLocations(mgr_reply, node_deque, adv);
	
	TIMER_STOP(metadata_timer, "WRITE " << range << ": writeRecordLocations(), result: " << result);
	if (!result)
		return false;

// publish the latest written version
	TIMER_START(publish_timer);
	params.clear();
	params.push_back(buffer_wrapper(range, true));
	direct_rpc->dispatch(vmgr_host, vmgr_service, VMGR_PUBLISH, params,
				boost::bind(rpc_result_callback, boost::ref(result), _1, _2));
	direct_rpc->run();
	TIMER_STOP(publish_timer, "WRITE " << range << ": VMGR_PUBLISH, result: " << result);
	TIMER_STOP(write_timer, "WRITE " << range << ": has completed, result: " << result);
	if (result) {
		latest_root.node.version = mgr_reply.ticket;
		return true;
	} else
		return false;
}

