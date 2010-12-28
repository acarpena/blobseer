#include <client/object_handler.hpp>
#include <iostream>
//#include <HavegeInline.h>
#include "HAVEGE2.0.h"

int main(int argc, char* argv[]) {
    int blob_id;
    if (argc != 3 || sscanf(argv[2],"%d",&blob_id) != 1) {
        std::cerr << "Usage: ./havegeBS.py <config_bs> <blob_id>" << std::endl;
        exit(1);
    }

    object_handler mem(argv[1]);
    mem.get_latest(blob_id);

    unsigned int page_size = mem.get_page_size();
    char* buffer = new char[page_size];

    while (true) {
        for (unsigned int i = 0; i < page_size; i += sizeof(int)) {
          int tmp = cryptondrand();
            for (unsigned int j = 0; j < sizeof(int) && i + j < page_size; j++)
              buffer[i + j] = (char) (tmp >> (8 * j));
        }
        while (!mem.append(page_size,buffer));
    }

    exit(1);
}
