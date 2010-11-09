#include <iostream>
#include "bindings/blob-mpi/BFS_interface.hpp"
#include "common/debug.hpp"
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {
  unsigned int page_size, replica_count;
  std::string blob_name;
    if (argc != 5 || sscanf(argv[3], "%u", &page_size) != 1 || sscanf(argv[4], "%u", &replica_count) != 1) {
	cout << "Usage: create_blob <config_file> <blob_name> <page_size> <replica_count>. To be used in empty deployment to generate ID 1." << endl;
	return 1;
    }

    BFS_interface *my_mem;
    my_mem = new BFS_interface(string(argv[1]));

    if (!my_mem->fcreate(string(argv[2]), page_size, replica_count))
	cout << "Error: could not create the blob!" << endl;	
    else
	cout << "Blob created successfully." << endl;

    delete my_mem;
    
    cout << "End of test" << endl;
    return 0;
}
