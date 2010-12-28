#include "wnp_object.hpp"

using namespace std;

wnp_object::wnp_object(const std::string &config_file) :object_handler(config_file)
{
}
		
bool wnp_object::exec_write(boost::uint64_t offset, boost::uint64_t size, char *buffer, bool append) 
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

	cout << "WNP client!!" << endl;
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

// write the set of pages to the page providers
	boost::dynamic_bitset<> page_results(adv.size());
	TIMER_START(providers_timer);
	for (boost::uint64_t i = 0, j = 0; i * page_size < size; i++, j += replica_count) {
// prepare the page
		rpcvector_t write_params;
		metadata::query_t page_key(id, range.version, i, page_size);
		DBG("WRITE QUERY " << page_key);
		write_params.push_back(buffer_wrapper(page_key, true));
		DBG("PAGE KEY IN SERIAL FORM " << write_params.back());
		write_params.push_back(buffer_wrapper(buffer + i * page_size, page_size, true));
// write the replicas
		for (unsigned int k = j; k < j + replica_count; k++) {
			direct_rpc->dispatch(adv[k].get_host(), adv[k].get_service(), PROVIDER_WRITE, 
					write_params, boost::bind(rpc_write_callback, boost::ref(page_results), k, replica_count, _1, _2));
	// set the version & page index for leaf nodes
			adv[k].set_free(range.version);
			adv[k].set_update_rate(i);
		}
	}
	direct_rpc->run();

// make sure each page has at least one successfully written replica
	for (unsigned int i = 0; i < adv.size() && result; i++) {
		unsigned int k;
		for (k = i; k < i + replica_count; k++)
			if (page_results[k])
				break;
		if (k == i + replica_count) {
			ERROR("WRITE " << range << ": none of the replicas of page " << i / replica_count 
					<< " could be written successfully, aborted");
			result = false;
			break;
		}	    
	}

	TIMER_STOP(providers_timer, "WRITE " << range << ": Data written to providers, result: " << result);
	if (!result)
		return false;
	else
		return true;
	
}
