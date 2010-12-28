#include <client/object_handler.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char* argv[]) {
    int blob_id;
    if (argc != 4 || sscanf(argv[2],"%d",&blob_id) != 1) {
        std::cerr << "Usage: ./egdBS <config_bs> <blob_id> <socket_file>" << std::endl;
        exit(1);
    }

    object_handler mem(argv[1]);
    mem.get_latest(blob_id);

    struct sockaddr_un addr;
    int s, len;
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Can't open a new socket!" << std::endl;
        exit(1);
    }
    memset(&addr,0,sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path,argv[3]);
    len = std::strlen(addr.sun_path) + sizeof(addr.sun_family);
    if (connect(s, (struct sockaddr *) &addr, len) == -1) {
        std::cerr << "Can't connect to " << argv[3] << " socket file!" << std::endl;
        std::cerr << "Usage: ./egdBS <config_bs> <blob_id> <socket_file>" << std::endl;
        exit(1);
    }
    std::cout << "Connected to egd." << std::endl;

    int page_size = mem.get_page_size();
    char* msg = new char[3];
    char* buffer = new char[page_size];
    while (true) {
        for (int cur_offset = 0, cur_size = 255; cur_offset < page_size; cur_offset += cur_size) {
            if (cur_offset + cur_size > page_size)
                cur_offset = page_size - cur_offset;
            sprintf(msg, "%c%c",(char) 2,(char) cur_size);
            if (send(s, msg, 2, 0) == -1) {
                std::cerr << "Socket error: unable to send data!" << std::endl;
                exit(1);
            }
            len = recv(s, buffer + cur_offset, cur_size, 0);
            if (len == 0) {
                std::cerr << "Socket error: egd has been closed!" << std::endl;
                exit(1);
            }
            if (len < 0) {
                std::cerr << "Socket error: unable to retrieve data!" << std::endl;
                exit(1);
            }
        }

        while (!mem.append(page_size,buffer));
    }

    exit(1);
}
