#ifndef __STRUCTURES
#define __STRUCTURES

#include "provider/provider_adv.hpp"
#include <boost/serialization/vector.hpp>
#include <ostream>

namespace metadata {

static const unsigned int ROOT = 0, LEFT_CHILD = 1, RIGHT_CHILD = 2;

class query_t {
public:
    uint32_t id, version;
    uint64_t offset, size;

    friend std::ostream &operator<<(std::ostream &out, const query_t &query) {
	out << "(" << query.id << ", " << query.version << ", " 
	    << query.offset << ", " << query.size << ")";
	return out;
    }

    query_t(uint32_t i, uint32_t v, uint64_t o, uint64_t s) :
	id(i), version(v), offset(o), size(s) { }

    query_t() : id(0), version(0), offset(0), size(0) { }

    bool intersects(const query_t &second) const {
	/*
	  if (id != second.id && version != second.version)
	      return false;
	*/
	if (offset <= second.offset)
	    return second.offset < offset + size;
	else
	    return offset < second.offset + second.size;
    }
    bool operator<(const query_t &second) const {
	return version < second.version;
    }
    bool operator==(const query_t &second) const {
	return id == second.id && version == second.version && 
	    offset == second.offset && size == second.size;
    }
    bool empty() const {
	return id == 0 && version == 0 && offset == 0 && size == 0;
    }
    unsigned int getParent(query_t &dest) const {
	dest.id = id;
	dest.version = version;
	dest.size = 2 * size;
	if (offset % dest.size == 0) {
	    dest.offset = offset;
	    return LEFT_CHILD;
	} else {
	    dest.offset = offset - size;
	    return RIGHT_CHILD;
	}
    }
  
    template <class Archive> void serialize(Archive &ar, unsigned int) {
	ar & id & version & offset & size;
    }
};

class root_t {
public:
    query_t node;
    uint64_t page_size;

    root_t(uint32_t i, uint32_t v, uint64_t ps, uint64_t ms) :
	node(i, v, 0, ms), page_size(ps) { }
    const query_t &get_node() const {
	return node;
    }
    uint64_t get_page_size() const {
	return page_size;
    }
    template <class Archive> void serialize(Archive &ar, unsigned int) {
	ar & node & page_size;
    }
};

class dhtnode_t {
public:
    query_t left, right;    
    provider_adv leaf;

    friend std::ostream &operator<<(std::ostream &out, const dhtnode_t &node) {
	out << "(left = " << node.left << ", right = " << node.right << ")";
	return out;
    }
    template <class Archive> void serialize(Archive &ar, unsigned int) {
	ar & left & right & leaf;
    }
};

}

class lockmgr_reply {    
public:
    typedef std::vector<metadata::query_t> siblings_enum_t;
    siblings_enum_t left, right;
    uint32_t ticket;
    uint64_t root_size;
    metadata::root_t stable_root;

    lockmgr_reply() : stable_root(0, 0, 0, 0) { }
    
    static metadata::query_t search_list(siblings_enum_t &siblings, uint64_t offset, uint64_t size) {
	for (unsigned int i = 0; i < siblings.size(); i++)
	    if (siblings[i].offset == offset && siblings[i].size == size)
		return siblings[i];
	return metadata::query_t();
    }

    template <class Archive> void serialize(Archive &ar, unsigned int) {
	ar & stable_root & left & right & root_size & ticket;
    }
};

#endif
