#ifndef __RPC_CLIENT
#define __RPC_CLIENT

#include "boost/unordered_map.hpp"
#include <stdexcept>

#include "common/debug.hpp"
#include "rpc_meta.hpp"

/// Threaded safe per host buffering for RPC calls
/**
   Dispatches go through the RPC request buffer. The main idea is to enqueue them
   on a per host basis. This way we can transparently execute multiple RPC requests
   for the same server by using a single connection.
 */
template <class Transport, class Lock>
class request_queue_t {
public:
    typedef rpcinfo_t<Transport> request_t;    
    typedef boost::shared_ptr<request_t> prpcinfo_t;

private:
    typedef boost::unordered_map<unsigned int, prpcinfo_t> hostmap_t;
    typedef typename hostmap_t::iterator hostmap_it;
    typedef boost::unordered_map<string_pair_t, hostmap_t, boost::hash<string_pair_t> > requests_t;
    typedef typename requests_t::iterator requests_it;
    typedef typename Lock::scoped_lock scoped_lock;
    typedef std::pair<string_pair_t, hostmap_t> requests_entry_t;
    typedef std::pair<unsigned int, prpcinfo_t> request_entry_t;

    Lock mutex;
    requests_t request_map, pending_map;
    unsigned int request_id;
    
public:
    request_queue_t() : request_id(0) { }
    void clear();

    void enqueue(prpcinfo_t request);
    void mark_pending(prpcinfo_t request);

    prpcinfo_t dequeue_host(const string_pair_t &host_id);
    prpcinfo_t dequeue_pending(const string_pair_t &host_id);
    bool dequeue_pending(prpcinfo_t rpc_data);
    prpcinfo_t dequeue_any();

    unsigned int peek_host(const string_pair_t &host_id);
}; 

template <class Transport, class Lock>
void request_queue_t<Transport, Lock>::enqueue(prpcinfo_t request) { 
    scoped_lock lock(mutex);

    request->assign_id(request_id++);
    request_map[request->host_id].erase(request->id);
    request_map[request->host_id].insert(request_entry_t(request->id, request));
}

template <class Transport, class Lock>
void request_queue_t<Transport, Lock>::mark_pending(prpcinfo_t request) { 
    scoped_lock lock(mutex);
    
    pending_map[request->host_id].erase(request->id);
    pending_map[request->host_id].insert(request_entry_t(request->id, request));
}

template <class Transport, class Lock>
typename request_queue_t<Transport, Lock>::prpcinfo_t request_queue_t<Transport, Lock>::dequeue_host(const string_pair_t &host_id) {
    scoped_lock lock(mutex);
    prpcinfo_t result;

    hostmap_t &hm = request_map[host_id];
    hostmap_it i = hm.begin();
    if (i != hm.end()) {
	result = i->second;
	hm.erase(i->first);
	if (hm.size() == 0)
	    request_map.erase(host_id);
    }
    return result;
}

template <class Transport, class Lock>
bool request_queue_t<Transport, Lock>::dequeue_pending(prpcinfo_t rpc_data) {
    scoped_lock lock(mutex);

    return pending_map[rpc_data->host_id].erase(rpc_data->id) > 0;
}

template <class Transport, class Lock>
typename request_queue_t<Transport, Lock>::prpcinfo_t request_queue_t<Transport, Lock>::dequeue_pending(const string_pair_t &host_id) {
    scoped_lock lock(mutex);
    prpcinfo_t result;

    hostmap_t &hm = pending_map[host_id];
    hostmap_it i = hm.begin();
    if (i != hm.end()) {
	result = i->second;
	hm.erase(i->first);
	if (hm.size() == 0)
	    pending_map.erase(host_id);
    }

    if (result && result->socket)
	result->socket->close();
    
    return result;
}

template <class Transport, class Lock>
typename request_queue_t<Transport, Lock>::prpcinfo_t request_queue_t<Transport, Lock>::dequeue_any() {
    scoped_lock lock(mutex);
    prpcinfo_t result;

    requests_it j = request_map.begin();
    while (j != request_map.end() && j->second.size() == 0)
	++j;
    if (j != request_map.end()) {
	hostmap_it i = j->second.begin();
	result = i->second;
	j->second.erase(i->first);
	if (j->second.size() == 0)
	    request_map.erase(j->first);
    }
    return result;
}

template <class Transport, class Lock>
void request_queue_t<Transport, Lock>::clear() {
    scoped_lock lock(mutex);

    request_map.clear();
    pending_map.clear();
}

/// The RPC client
template <class Transport, class Lock> class rpc_client {
public:
    /// RPC client constructor 
    /**
       @param io_sevice The io_service to use.
       @param timeout The timeout to wait for an answer on the socket
     */
    rpc_client(boost::asio::io_service &io_service, unsigned int timeout = DEFAULT_TIMEOUT);
    ~rpc_client(); 

    /// Dispatch an RPC request to the io_service. The result is managed by the client directly.
    /**
       The io_service will eventually run the RPC, collect the result and call a specified callback.
       The result will be managed by the client directly. This might be useful if you want to use a 
       custom buffer for results.
       @param host The name of the host
       @param service The name of the service
       @param name The RPC id
       @param params Vector to the params (buffer_wrapped).
       @param result_callback The client side callback
       @param result The client managed result vector (all custom items must be buffer_wrapped)
     */    
    void dispatch(const std::string &host, const std::string &service,
		  boost::uint32_t name,
		  const rpcvector_t &params,
		  rpcclient_callback_t result_callback,
		  const rpcvector_t &result);

    /// Dispatch an RPC request to the io_service. The result is managed by the client directly.
    /**
       The io_service will eventually run the RPC, collect the result and call a specified callback.
       The result will be managed automatically and passed to the callback.
       @param host The name of the host
       @param service The name of the service
       @param name The RPC id
       @param params Vector to the params (buffer_wrapped).
       @param result_callback The client side callback
     */    
    void dispatch(const std::string &host, const std::string &service,
		  boost::uint32_t name,
		  const rpcvector_t &params,
		  rpcclient_callback_t result_callback);

    /// Run the io_service.
    /** 
	Run this instead of the io_service's run, if you intend to wait for RPC calls.
	It will ensure all callbacks will eventually execute; will flush the RPC buffer 
	and reset the io_service.
    */
    bool run();
    
private:
    typedef request_queue_t<Transport, Lock> requests_t;
    typedef typename requests_t::request_t rpcinfo_t;
    typedef typename requests_t::prpcinfo_t prpcinfo_t;
    typedef boost::shared_ptr<typename Transport::socket> psocket_t;

    typedef cached_resolver<Transport, Lock> host_cache_t;
    
    static const unsigned int DEFAULT_TIMEOUT = 5;    
    // the system caps the number of max opened handles, let's use 240 for now (keep 16 handles in reserve)
    static const unsigned int WAIT_LIMIT = 240;    

    host_cache_t *host_cache;
    unsigned int waiting_count, timeout;
    boost::asio::io_service *io_service;
    requests_t request_queue;

    void handle_connect(prpcinfo_t rpc_data, const boost::system::error_code& error);

    void handle_next_request(psocket_t socket, const string_pair_t &host_id);

    void handle_resolve(prpcinfo_t rpc_data, 
			const boost::system::error_code& error, typename Transport::endpoint end);

    void handle_header(prpcinfo_t rpc_data, 
		       const boost::system::error_code& error, size_t bytes_transferred);

    void handle_callback(prpcinfo_t rpc_data, const boost::system::error_code &error);

    void handle_params(prpcinfo_t rpc_data, unsigned int index);

    void handle_param_size(prpcinfo_t rpc_data, unsigned int index,
			   const boost::system::error_code& error, size_t bytes_transferred);

    void handle_param_buffer(prpcinfo_t rpc_data, unsigned int index,
			     const boost::system::error_code& error, size_t bytes_transferred);

    void handle_answer(prpcinfo_t rpc_data, unsigned int index,
		       const boost::system::error_code& error, size_t bytes_transferred);
    
    void handle_answer_size(prpcinfo_t rpc_data, unsigned int index,
			    const boost::system::error_code& error,
			    size_t bytes_transferred);

    void handle_answer_buffer(prpcinfo_t rpc_data, unsigned int index,
			      const boost::system::error_code& error,
			      size_t bytes_transferred);

    void on_timeout(const boost::system::error_code& error);
};

template <class Transport, class Lock>
rpc_client<Transport, Lock>::rpc_client(boost::asio::io_service &io, unsigned int t) :
    host_cache(new host_cache_t(io)), waiting_count(0), timeout(t), io_service(&io) { }

template <class Transport, class Lock>
rpc_client<Transport, Lock>::~rpc_client() {
    delete host_cache;
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::dispatch(const std::string &host, const std::string &service,
					   boost::uint32_t name,
					   const rpcvector_t &params,
					   rpcclient_callback_t callback,
					   const rpcvector_t &result) {
    prpcinfo_t info(new rpcinfo_t(host, service, name, params, callback, result));
    request_queue.enqueue(info);
    handle_next_request(psocket_t(), info->host_id);
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::dispatch(const std::string &host, const std::string &service,
					   boost::uint32_t name,
					   const rpcvector_t &params,
					   rpcclient_callback_t callback) {
    prpcinfo_t info(new rpcinfo_t(host, service, name, params, callback, rpcvector_t()));
    request_queue.enqueue(info);
    handle_next_request(psocket_t(), info->host_id);
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_next_request(psocket_t socket, const string_pair_t &host_id) {    
    if (socket) {
	prpcinfo_t rpc_data = request_queue.dequeue_host(host_id);
	if (!rpc_data)
	    rpc_data = request_queue.dequeue_pending(host_id);
	if (rpc_data) {
	    rpc_data->socket = socket;
	    waiting_count++;
	    DBG("[RPC " << rpc_data->id << " " << rpc_data->host_id.first << " " << rpc_data->host_id.second
		<< "] about to reuse socket, wait limit: " << waiting_count);
	    handle_connect(rpc_data, boost::asio::error::invalid_argument);
	}
    }
    while (waiting_count < WAIT_LIMIT) {
	prpcinfo_t rpc_data = request_queue.dequeue_any();
	if (rpc_data) {
	    request_queue.mark_pending(rpc_data);
	    waiting_count++;
	    DBG("[RPC " << rpc_data->id << " " << rpc_data->host_id.first << " " << rpc_data->host_id.second 
		<< "] about create new socket, wait limit: " << waiting_count);
	    host_cache->dispatch(boost::ref(rpc_data->host_id.first), boost::ref(rpc_data->host_id.second),
				 boost::bind(&rpc_client<Transport, Lock>::handle_resolve, this, rpc_data, _1, _2));
	} else
	    break;
    }
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_resolve(prpcinfo_t rpc_data, const boost::system::error_code& error, typename Transport::endpoint end) {
    if (error) {
	if (request_queue.dequeue_pending(rpc_data))
	    handle_callback(rpc_data, error);
	return;
    }
    rpc_data->socket = psocket_t(new typename Transport::socket(*io_service));
    rpc_data->socket->async_connect(end, boost::bind(&rpc_client<Transport, Lock>::handle_connect, this, rpc_data, _1));
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_connect(prpcinfo_t rpc_data, const boost::system::error_code& error) {
    if (!request_queue.dequeue_pending(rpc_data)) {
	// pending connection broken and we are not reusing the socket => abort
	if (error != boost::asio::error::invalid_argument)
	    return;
    } else if (error) {
	handle_callback(rpc_data, error);	
	return;
    }
    DBG("[RPC " << rpc_data->id << " " << rpc_data->host_id.first << " " << rpc_data->host_id.second << "] socket opened, sending header");
    boost::asio::async_write(*(rpc_data->socket), boost::asio::buffer((char *)&rpc_data->header, sizeof(rpc_data->header)),
			     boost::asio::transfer_all(),
			     boost::bind(&rpc_client<Transport, Lock>::handle_header, this, rpc_data, _1, _2));
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_header(prpcinfo_t rpc_data, 
						const boost::system::error_code& error, size_t bytes_transferred) {
    if (error || bytes_transferred != sizeof(rpc_data->header)) {
	handle_callback(rpc_data, error);
	return;
    }
    handle_params(rpc_data, 0);
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_params(prpcinfo_t rpc_data, unsigned int index) {
    if (index < rpc_data->params.size()) {
	rpc_data->header.psize = rpc_data->params[index].size();
	DBG("[RPC " << rpc_data->id << "] sending new param size: " << rpc_data->header.psize);
	boost::asio::async_write(*(rpc_data->socket), boost::asio::buffer((char *)&rpc_data->header.psize, sizeof(rpc_data->header.psize)),
				 boost::asio::transfer_all(),
				 boost::bind(&rpc_client<Transport, Lock>::handle_param_size, this, rpc_data, index, _1, _2));
	return;
    }
    DBG("[RPC " << rpc_data->id << "] finished sending params, now waiting for reply");
    boost::asio::async_read(*(rpc_data->socket), boost::asio::buffer((char *)&rpc_data->header, sizeof(rpc_data->header)),
			    boost::asio::transfer_all(),
			    boost::bind(&rpc_client<Transport, Lock>::handle_answer, this, rpc_data, 0, _1, _2));
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_param_size(prpcinfo_t rpc_data, unsigned int index,
						    const boost::system::error_code& error, size_t bytes_transferred) {
    if (error || bytes_transferred != sizeof(rpc_data->header.psize)) {
	handle_callback(rpc_data, error);
	return;	
    }
    boost::asio::async_write(*(rpc_data->socket), boost::asio::buffer(rpc_data->params[index].get(), rpc_data->params[index].size()),
			     boost::asio::transfer_all(),
			     boost::bind(&rpc_client<Transport, Lock>::handle_param_buffer, this, rpc_data, index, _1, _2));
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_param_buffer(prpcinfo_t rpc_data, unsigned int index,
						      const boost::system::error_code& error, size_t bytes_transferred) {
    if (error || bytes_transferred != rpc_data->params[index].size()) {
	handle_callback(rpc_data, boost::asio::error::invalid_argument);
	return;
    }
    handle_params(rpc_data, index + 1);
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_answer(prpcinfo_t rpc_data, unsigned int index, 
						const boost::system::error_code& error, size_t bytes_transferred) {
    if (index == 0) {
        DBG("[RPC " << rpc_data->id << "] got reply header, now about to transfer answer");
	if (!error && bytes_transferred == sizeof(rpc_data->header))	
	    rpc_data->result.resize(rpc_data->header.psize);
	else {
	    handle_callback(rpc_data, boost::asio::error::invalid_argument);
	    return;
	}
    } 
    if (index < rpc_data->result.size())
	boost::asio::async_read(*(rpc_data->socket), boost::asio::buffer((char *)&rpc_data->header.psize, sizeof(rpc_data->header.psize)),
				 boost::asio::transfer_all(),
				 boost::bind(&rpc_client<Transport, Lock>::handle_answer_size, this, rpc_data, index, _1, _2));
    else
	// got the full answer successfully.
	handle_callback(rpc_data, boost::system::error_code());
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_answer_size(prpcinfo_t rpc_data, unsigned int index,
						     const boost::system::error_code& error, size_t bytes_transferred) {
    if (error || bytes_transferred != sizeof(rpc_data->header.psize)) {
	handle_callback(rpc_data, boost::asio::error::invalid_argument);
	return;	
    }
    bool is_managed = rpc_data->result[index].size() != 0;
    if (rpc_data->header.psize == 0 || (is_managed && rpc_data->result[index].size() != rpc_data->header.psize)) {
	handle_callback(rpc_data, boost::asio::error::invalid_argument);
	return;
    }
    char *result_ptr;
    if (is_managed)
	result_ptr = rpc_data->result[index].get();
    else {
	result_ptr = new char[rpc_data->header.psize];
	rpc_data->result[index] = buffer_wrapper(result_ptr, rpc_data->header.psize);
    }
    boost::asio::async_read(*(rpc_data->socket), boost::asio::buffer(rpc_data->result[index].get(), rpc_data->result[index].size()),
			    boost::asio::transfer_all(),
			    boost::bind(&rpc_client<Transport, Lock>::handle_answer_buffer, this, rpc_data, index, _1, _2));
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_answer_buffer(prpcinfo_t rpc_data, unsigned int index,
						       const boost::system::error_code& error, size_t bytes_transferred) {
    if (error || bytes_transferred != rpc_data->result[index].size()) {
	handle_callback(rpc_data, boost::asio::error::invalid_argument);
	return;
    }
    handle_answer(rpc_data, index + 1, boost::system::error_code(), sizeof(rpc_data->header.psize));
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::handle_callback(prpcinfo_t rpc_data, const boost::system::error_code &error) {
    waiting_count--;
    DBG("[RPC " << rpc_data->id << "] about to run callback, completed with error: " << error);
    if (error)
	rpc_data->header.status = rpcstatus::egen;
    boost::apply_visitor(*rpc_data, rpc_data->callback);
    handle_next_request(rpc_data->socket, rpc_data->host_id);
}

template <class Transport, class Lock>
void rpc_client<Transport, Lock>::on_timeout(const boost::system::error_code& error) {
    if (error != boost::asio::error::operation_aborted) {
	// Timer was not cancelled, take necessary action.
	INFO("wait timeout(" << timeout << " s), handlers have been aborted");
    }
}

template <class Transport, class Lock>
bool rpc_client<Transport, Lock>::run() {
/*
    unsigned int nr = 0;
    do {
	TIMER_START(op);
	nr = io_service->run_one();
	TIMER_STOP(op, "executed next IO handler");
    } while (nr > 0);
*/
    io_service->run();
    bool result = waiting_count == 0;
    waiting_count = 0;
    request_queue.clear();
    io_service->reset();
    return result;
}

#endif
