#include <iostream>

#include "blobseer_fs.hpp"

using namespace std;

int main (int argc, char **argv)
{
      unsigned int pg, repl, buf_size;
      
      /*arguments: 
      * fuse mounting point
      * BS configuration file
      * page size (in Kbytes)
      * replica count
      * buffer size (in Kbytes)
      * fuse options
      */
      
      if (argc < 6 || sscanf(argv[3], "%u", &pg) != 1 || sscanf(argv[4], "%u", &repl) != 1 || sscanf(argv[5], "%u", &buf_size) != 1) {
	  cout << "Usage: create_blob <config_file> <page_size> <replica_count>" << endl;
	  return 1;
      }
      
      //arguments to pass to fuse
      char **fuse_argv;
      fuse_argv = new char*[argc-4];

      fuse_argv[0] = argv[0];
      fuse_argv[1] = argv[1];

      for (int i = 6; i < argc; i++)
	  fuse_argv[i-4] = argv[i];

      BlobSeerOps bs_ops(string(argv[2]), (pg * (1 << 10)), repl, (buf_size * (1 << 10)));
      return bs_ops.main (argc - 4, fuse_argv, NULL, &bs_ops);
}
