#include <iostream>

// malicious clients
#include "clients/iw_object.hpp"
#include "clients/pnw_object.hpp"
#include "clients/wnp_object.hpp"

// the BlobSeer client
#include "clients/object_handler.hpp"

// the log client
#include "logging_utils/logging_client.hpp"

using namespace std;
using namespace boost;

int main(int argc, char **argv) 
{
	unsigned int obj_id = 0;
	char operation;
	uint64_t page_size, write_size, offset = 0;
    int client_type = 0; // client_type = 0 - normal client
                         // client_type = 1 - publish no write
                         // client_type = 2 - write no publish
                         // client_type = 3 - incorrect writes
                       
    
	// check parameters
    if (argc != 8 
        ||sscanf(argv[1], "%d", &client_type) != 1 
        || sscanf(argv[2], "%c", &operation) != 1 
        || sscanf(argv[3], "%d", &obj_id) != 1
        || sscanf(argv[4], "%lu", &page_size) != 1 
        || sscanf(argv[5], "%u", &offset) != 1 
        ||sscanf(argv[6], "%lu", &write_size) != 1 ) 
	{
		cout << "Usage: client_iw <client_type> <op> <blob id> <page size> <offset> <write size> <config_file>. Create the blob with create_blob first. client_type=0/1/2/3 (normal/pnw/wnp/iw) op=R/W/A (read/write/append)" << endl;
		return 1;
	}
    
    
	char *big_zone = (char *)malloc(write_size); 
	object_handler *my_mem;
    
    // initialize the malicious/normal client according to its type
    switch (client_type) {
        case 1: {
            my_mem = new pnw_object(string(argv[7]));
            break;
        }
        case 2: {
            my_mem = new wnp_object(string(argv[7]));
            break;
        }
        case 3: {
            my_mem = new iw_object(string(argv[7]));
            break;
        }
        default: {
            my_mem = new object_handler(string(argv[7]));
            break;
        }            
    }
    
    // get the latest version
	if (!my_mem->get_latest(obj_id)) {
		cout << "Could not alloc latest version, write test aborting" << endl;
		return 1;
	} else
		cout << "get_latest() successful, now starting..." << endl;
	
    // initialize log object using the configuration file
    std::string conf = argv[7];
    logging_client log(conf.c_str());
    
    // write data
    if (operation == 'W') {
        char c = '0';
        memset(big_zone, c, write_size);
        
        std::string msg = "[Client type " + client_type;
        msg += "] Starting to write: [" + obj_id;
        msg += "][" + page_size;
        msg += "][" + offset;
        msg += "][" + write_size;
        msg += "]";
        // write a messaje in the log before and after the write operation
        log.log(INFO, msg);
        // write to BlobSeer
        bool result = my_mem->write(offset, write_size, big_zone);
        msg = "[Client type " + client_type;
        msg += "] Wrote: [" + obj_id;
        msg += "][" + page_size;
        msg += "][" + offset;
        msg += "][" + write_size;
        msg += "] Result: [" + result;
        msg += "]";
        log.log(INFO, msg);
        
        if (!result)
            cout << "Could not write ("  << offset<< "," << write_size << ")" << endl;
    }
    
    
    free(big_zone);
    delete my_mem;
    
    cout << "End of test" << endl;
    return 0;
}
