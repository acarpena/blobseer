/*
 * ClientInterface.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: acarpena
 */
#include "libconfig.h++"

#include "common/debug.hpp"

#include "FBlobseerClient/NamespaceClient.hpp"

const uint64_t PAGE_SIZE = 1 << 3; // 64 KB
const uint64_t START_SIZE = 1 << 3; // 1 MB
const uint64_t STOP_SIZE = 1 << 16; // 256 MB

using namespace std;

void printResult(std::vector<FileMetadata> &mdvect)
{
	INFO("Result:");
	for (int i=0;i<mdvect.size();i++)
	{
		INFO(mdvect[i].getPath()<<"  "<<mdvect[i].getType());
	}
	INFO("End Result");
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
    	cout << "Usage: " << argv[0] << " <config_file>" << endl;
    	cout << "Config file:\nNamespaceServer: {\n\thost = <host>\n\tport = <port_no>\n};\n";
    	return 1;
    }

    INFO ("Initializing client");
    NamespaceClient nsClient(argv[1]);
    FileHandler* handler;

    std::vector<FileMetadata> mdvect;

    handler = nsClient.createFile("/dirbla/fis", PAGE_SIZE, 1);

//	try {
//		 //handler = nsClient.createFile("/dirbla/fis",65536, 1);
//		object_handler *oh = new object_handler(argv[1]);
//		oh->create(1024,1);
//	}
//	catch (std::runtime_error &e)
//	{
//		ERROR("exception "<<e.what());
//	}

    INFO("after create");
    if (!handler)
    {
    	INFO("Cannot create file");
    }
    else
    {
        char *big_zone = (char *)malloc(STOP_SIZE); 
        memset(big_zone, 40, STOP_SIZE);
        
    	INFO("created: "<<handler->get_page_size());
    	handler->write( 0,  STOP_SIZE, big_zone);
        
        INFO("wrote into the blob  ");
        INFO("new size: "<<handler->get_size());
        nsClient.destroyFileHandler(handler);

    }

  /*  nsClient.mkdir("/dir1");
    nsClient.mkdir("/dir2");
    nsClient.mkdir("/dir3");
    nsClient.mkdir("/dir4");
    nsClient.mkdir("/dir3/dir5");
    nsClient.mkdir("/dir3/dir6");
    nsClient.mkdir("/dir3/dir7");
    nsClient.mkdir("/dir3/dir5/dir8");
    nsClient.mkdir("/dir3/dir5/dir9");


    nsClient.listDir("/", mdvect);
    printResult(mdvect);

    nsClient.listDir("/dir1",mdvect);
    printResult(mdvect);

    nsClient.listDir("/dir3",mdvect);
    printResult(mdvect);

    nsClient.listDir("/dir3/dir5",mdvect);
    printResult(mdvect);

    printResult(mdvect);

    nsClient.rename("/dir3/dir5", "/dir3/newdir");

    nsClient.listDir("/dir3/newdir",mdvect);
    printResult(mdvect);

    nsClient.listDir("/dir3",mdvect);
    printResult(mdvect);

    nsClient.deleteFile("/dir3/newdir");

    nsClient.listDir("/dir3/newdir",mdvect);
    printResult(mdvect);

    nsClient.listDir("/dir3",mdvect);
    printResult(mdvect);*/
    return 0;
}
