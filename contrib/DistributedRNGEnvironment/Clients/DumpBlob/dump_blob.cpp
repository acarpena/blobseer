#include <client/object_handler.hpp>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    int blob_id;
    if (argc != 4 || sscanf(argv[2],"%d",&blob_id) != 1) {
        std::cerr << "Usage: ./randomBS.py <config_bs> <blob_id> <dump_file>" << std::endl;
        exit(1);
    }


    std::ofstream dump_file2(argv[3],std::ios::binary);
    if (!dump_file2.is_open()) {
        std::cerr << "Bad dump_file: " << argv[3] << std::endl;
        exit(1);
    }
    object_handler mem(argv[1]);
    mem.get_latest(blob_id);


    boost::uint64_t size = mem.get_size(), page_size = mem.get_page_size();

    char* buffer = new char[page_size];
    for (boost::uint64_t offset = 0; offset < size; offset += page_size) {
        mem.read(offset,page_size,buffer);
        dump_file2.write(buffer,page_size);
    }
    dump_file2.close();

    exit(0);
}
