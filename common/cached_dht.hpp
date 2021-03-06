#ifndef __CACHED_DHT
#define __CACHED_DHT

#include <boost/function.hpp>

#include "cache_mt.hpp"

template<class DHT, class HashFcn>
class cached_dht {
public:      
    typedef typename DHT::pkey_t pkey_t;
    typedef typename DHT::pvalue_t pvalue_t;    
    typedef typename DHT::mutate_callback_t mutate_callback_t;
    typedef typename DHT::get_callback_t get_callback_t;
    typedef typename DHT::lock_t lock_t;

    cached_dht(boost::asio::io_service &io_service, unsigned int r = 1, unsigned int t = 10, unsigned int size = 1 << 20);
    ~cached_dht();
    void addGateway(const std::string &host, const std::string &service);
    void put(pkey_t key, pvalue_t value, int ttl, const std::string &secret, mutate_callback_t put_callback);
    void get(pkey_t key, get_callback_t get_callback);
    void remove(pkey_t key, pvalue_t value, int ttl, const std::string &secret, mutate_callback_t remove_callback);
    void wait();

private:
    typedef cache_mt<pkey_t, pvalue_t, lock_t, HashFcn, cache_mt_LRU<pkey_t, HashFcn> > cache_t;
    DHT dht;
    cache_t cache;
};

template<class DHT, class HashFcn>
inline void cached_dht<DHT, HashFcn>::wait() {
    dht.wait();
}

template<class DHT, class HashFcn>
inline cached_dht<DHT, HashFcn>::cached_dht(boost::asio::io_service &io_service, unsigned int r, unsigned int t, unsigned int size) 
    : dht(io_service, r, t), cache(size) { }

template<class DHT, class HashFcn>
inline cached_dht<DHT, HashFcn>::~cached_dht() { }

template<class DHT, class HashFcn>
inline void cached_dht<DHT, HashFcn>::addGateway(const std::string &host, const std::string &service) {
    dht.addGateway(host, service);
}

template<class DHT, class HashFcn>
void cached_dht<DHT, HashFcn>::put(pkey_t key, pvalue_t value, int ttl, const std::string &secret, mutate_callback_t put_callback) {    
    cache.write(key, value);
    dht.put(key, value, ttl, secret, put_callback);
}

template<class DHT, class HashFcn>
void cached_dht<DHT, HashFcn>::remove(pkey_t key, pvalue_t value, int ttl, const std::string &secret, mutate_callback_t remove_callback) {
    cache.free(key);
    dht.remove(key, value, ttl, secret, remove_callback);
}

template<class DHT, class HashFcn>
void cached_dht<DHT, HashFcn>::get(pkey_t key, get_callback_t getvalue_callback) {
    pvalue_t result;
    if (cache.read(key, &result))
	getvalue_callback(result);
    else
	dht.get(key, getvalue_callback);
}

#endif
