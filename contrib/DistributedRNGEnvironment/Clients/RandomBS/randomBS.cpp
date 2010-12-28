#include <client/object_handler.hpp>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    int blob_id;
    if (argc != 3 || sscanf(argv[2],"%d",&blob_id) != 1) {
        std::cerr << "Usage: ./pyRandomBS.py <config_bs> <blob_id>" << std::endl;
        exit(1);
    }

    object_handler mem(argv[1]);
    mem.get_latest(blob_id);

    std::ifstream random_seed("/dev/random",std::ios::binary);

    char* buffer = new char[mem.get_page_size()];
    while (true) {
        random_seed.read(buffer,mem.get_page_size());
        while (!mem.append(mem.get_page_size(),buffer));
    }

    exit(1);
}
