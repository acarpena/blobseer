/*
 * testFBlobseer.c
 *
 *  Created on: Mar 26, 2010
 *      Author: acarpena
 */

#include "FBlobseerClient/clib/NamespaceClientC.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WRITESIZE 1048576
#define PAGESIZE 65536

void printResult(fmetadata_list_t mdvect)
{
	int i;

	printf("Result %d\n",mdvect.size);
	for (i=0;i<mdvect.size;i++)
	{
		printf("%s - %d\n", mdvect.md_list[i].path,mdvect.md_list[i].type);
	}
	printf("End Result\n");
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
    	printf( "Usage: %d <config_file>\n",argv[0]);
    	printf("Config file:\nNamespaceServer: {\n\thost = <host>\n\tport = <port_no>\n};\n");
    	return 1;
    }

    printf("Initializing client\n");
    blob_env_t env;
    ns_handler_t ns;
    fblob_t* fileHandler;
    int i;

    
    // write into the file
    char * buffer;
    buffer = (char*)malloc(WRITESIZE*sizeof(char));
    for (i=0;i<WRITESIZE;i++)
    {
        buffer[i] = i%256;
    }
    
    ///////////////////////////////////
    // initialization phase for the namespace manager
    ///   parameters: blboseer configuration file, blob_env_t variable
    ns_init(argv[1], &env);
    //    namespace init
    ns_initializeNamespaceHandler(&env, &ns);

    ///////////////////////////////////
    // create file handler
    // parameters: ns_handler_t, new file path, page size, number of replicas
    fileHandler = ns_createFile(&ns, "/dirbla/fis",PAGESIZE, 1);

    ///////////////////////////////////
    // file handler operations
    if (fileHandler)
    {

    	// write parameters: file handler, offset, write size, buffer to be written
    	if (!fblob_write(fileHandler, 0, WRITESIZE, buffer))
    	{
    		printf("ERROR: Could not write into file\n");
    	}
    	else
    	{
    		printf("Write successful\n");
    	}

    	//append parameters: file handler, append size, buffer to be appended
    	if (!fblob_append(fileHandler, WRITESIZE, buffer))
    	{
    		printf("ERROR: Could not append to the file\n");
    	}
    	else
    	{
    		printf("Append successful\n");
    	}

    	// read parameters: file handler, version, offset, size, buffer
    	if (!fblob_read(fileHandler, 1, 0, WRITESIZE, buffer))
    	{
    		printf("ERROR: Could not read from the file\n");
    	}
    	else
    	{
    		printf("Read successful\n");
    	}


    	///////////////////////////////////
    	// delete file handler when it is not needed anymore
    	ns_destroyFileHandler(&ns, fileHandler);
    }


    ///////////////////////////////////
    ///////////////////////////////////
    // obtain file handler for an existing file
    // parameters: ns_handler_t, file path
    fileHandler = ns_getFileHandler(&ns, "/dirbla/fis");

    ///////////////////////////////////
    // file handler operations
    if (fileHandler)
    {
    	// write into the file

    	// write parameters: file handler, offset, write size, buffer to be written
    	if (!fblob_write(fileHandler, 0, WRITESIZE, buffer))
    	{
    		printf("ERROR: Could not write into file\n");
    	}
    	else
    	{
    		printf("Write successful\n");
    	}

    	//append parameters: file handler, append size, buffer to be appended
    	if (!fblob_append(fileHandler, WRITESIZE, buffer))
    	{
    		printf("ERROR: Could not append to the file\n");
    	}
    	else
    	{
    		printf("Append successful\n");
    	}

    	// read parameters: file handler, version, offset, size, buffer
    	if (!fblob_read(fileHandler, 1, 0, WRITESIZE, buffer))
    	{
    		printf("ERROR: Could not read from the file\n");
    	}
    	else
    	{
    		printf("Read successful\n");
    	}


    	///////////////////////////////////
    	// delete file handler when it is not needed anymore
    	ns_destroyFileHandler(&ns, fileHandler);
    }



    ///////////////////////////////////
    //namespace operations
    ns_mkdir( &ns, "/dir1");
    ns_mkdir( &ns, "/dir2");
    ns_mkdir( &ns, "/dir3");
    ns_mkdir( &ns, "/dir4");
    ns_mkdir( &ns, "/dir3/dir5");
    ns_mkdir( &ns, "/dir3/dir6");
    ns_mkdir( &ns, "/dir3/dir7");
    ns_mkdir( &ns, "/dir3/dir5/dir8");
    ns_mkdir( &ns, "/dir3/dir5/dir9");



    fmetadata_list_t list_result;

    ns_listDir(&ns, "/", &list_result);
    printResult(list_result);
    if (list_result.size !=0 ) free(list_result.md_list);

    ns_listDir(&ns, "/dir1",&list_result);
    printResult(list_result);
    if (list_result.size !=0 ) free(list_result.md_list);

    ns_listDir(&ns, "/dir3",&list_result);
    printResult(list_result);
    if (list_result.size !=0 ) free(list_result.md_list);

    ns_listDir(&ns, "/dir3/dir5",&list_result);
    printResult(list_result);
    if (list_result.size !=0 ) free(list_result.md_list);

    ns_rename(&ns, "/dir3/dir5", "/dir3/newdir");

    ns_listDir(&ns, "/dir3/newdir",&list_result);
    printResult(list_result);
    if (list_result.size !=0 ) free(list_result.md_list);


    free(buffer);
    ///////////////////////////////////
    // cleanup phase
    ns_free(&env, &ns);
    ns_finalize(&env);

    return 0;
}
