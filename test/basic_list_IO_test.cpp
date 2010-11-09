#include <iostream>

#include "client/object_handler.hpp"

#include "common/debug.hpp"

using namespace std;
using namespace boost;

const boost::uint64_t STOP_SIZE = 1024;
const boost::uint64_t TOTAL_SIZE = 1024;
const boost::uint64_t PAGE_SIZE = 1;
const boost::uint64_t READ_SIZE = 2;
const boost::uint64_t REPLICA_COUNT = 1;
const boost::uint64_t LIST_COUNT = 4;

bool test_mem(char *buff, boost::uint64_t size) {
    for (boost::uint64_t i = 0; i < size; i++)
	if (buff[i] != 'a')
	    return false;
    return true;
}

int main(int argc, char **argv) {
    unsigned int obj_id = 1;

    if (argc != 2) {
	cout << "Usage: basic_test <config_file>" << endl;
	return 1;
    }
    
    char *big_zone = (char *)malloc(STOP_SIZE);
    memset(big_zone, 'a', STOP_SIZE);
    for (unsigned int i = 0; i < STOP_SIZE; i++) {
    	big_zone[i] = '0' + i;
    }

    object_handler *my_mem = new object_handler(string(argv[1]));

    if ((!my_mem->get_latest(obj_id) || my_mem->get_page_size() == 0)
	&& !my_mem->create(PAGE_SIZE, REPLICA_COUNT)) {
	ERROR("Could not initialize blob id " << obj_id);
	return 1;
    } else
	INFO("get_latest() successful, now starting...");

    INFO("PAGE SIZE = " << PAGE_SIZE << ", TOTAL WRITE SIZE = " << TOTAL_SIZE);
    
    if (!my_mem->write(0, TOTAL_SIZE, big_zone)) {
	ERROR("Could not write!");
	return 2;
    }

    memset(big_zone, 'b', STOP_SIZE);
    INFO("Testing reading the whole blob...");
    if (!my_mem->get_latest() || !my_mem->read(0, TOTAL_SIZE, big_zone) ) {
	ERROR("FAILED");
	return 7;
    }
    printf("\n%s\n",big_zone);

    memset(big_zone, 'b', STOP_SIZE);
    INFO("Testing read list...");

    boost::uint64_t offsets[LIST_COUNT];
    boost::uint64_t sizes[LIST_COUNT];

    int offset_shift = 0, size_shift = 0;

    for (int i = 0; i < LIST_COUNT; i++) {
    	offsets[i] = offset_shift + i * 2 * PAGE_SIZE;
    	sizes[i] = size_shift + PAGE_SIZE;
    }
    if (!my_mem->get_latest() || !my_mem->read_list(big_zone, LIST_COUNT * (size_shift + PAGE_SIZE), LIST_COUNT, offsets, sizes) ) {
	ERROR("FAILED");
	return 7;
    }
    printf("\n%s\n",big_zone);


    memset(big_zone, 'c', STOP_SIZE);
    INFO("Testing write list...");

    for (int i = 0; i < LIST_COUNT; i++) {
    	offsets[i] = i * 4 * PAGE_SIZE;
    	sizes[i] = PAGE_SIZE;
    }

    if (!my_mem->get_latest() || !my_mem->write_list(big_zone, LIST_COUNT * PAGE_SIZE, LIST_COUNT, offsets, sizes) ) {
	ERROR("FAILED");
	return 7;
    }
    printf("\n%s\n",big_zone);

    memset(big_zone, 'b', STOP_SIZE);
    INFO("Testing reading the whole blob...");
    if (!my_mem->get_latest() || !my_mem->read(0, TOTAL_SIZE, big_zone) ) {
	ERROR("FAILED");
	return 7;
    }
    printf("\n%s\n",big_zone);


    free(big_zone);
    delete my_mem;

    INFO("All tests passed SUCCESSFULLY");
    return 0;
}
