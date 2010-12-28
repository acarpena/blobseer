#include <iostream>
#include "clients/pnw_object.hpp"

using namespace std;
using namespace boost;

const uint64_t PAGE_SIZE = 1 << 10; //  1 KB
const uint64_t START_SIZE = 1 << 10; // 1 kB
const uint64_t STOP_SIZE = 1 << 15; // 32 kB

int main(int argc, char **argv) 
{
	unsigned int obj_id = 0 , offset = 0;
	char operation;
	unsigned long int page_size, stage_size;
    
	if (argc != 7 || sscanf(argv[1], "%c", &operation) != 1 || sscanf(argv[2], "%d", &obj_id) != 1
        || sscanf(argv[3], "%lu", &page_size) != 1 || sscanf(argv[4], "%u", &offset) != 1 ||sscanf(argv[5], "%lu", &stage_size) != 1 ) 
	{
		cout << "Usage: test_grid <op> <blob id> <page size> <offset> <stage size> <config_file>. Create the blob with create_blob first. op=R/W/A (read/write/append)" << endl;
		return 1;
	}
    
	char *big_zone = (char *)malloc(STOP_SIZE); 
    
	pnw_object *my_mem = new pnw_object(string(argv[6]));
    
	if (!my_mem->get_latest(obj_id)) {
		cout << "Could not alloc latest version, write test aborting" << endl;
		return 1;
	} else
		cout << "get_latest() successful, now starting..." << endl;
    
    if (operation == 'W') {
        char c = '0';
        memset(big_zone, c++, stage_size);
        if (!my_mem->write(offset, stage_size, big_zone))
            cout << "Could not write ("  << offset<< "," << stage_size << ")" << endl;
    } 
    
    free(big_zone);
    delete my_mem;
    
    cout << "End of test" << endl;
    return 0;
}
