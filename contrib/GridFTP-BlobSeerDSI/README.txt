This project contains BlobSeer DSI for GridFTP. 

Dependencies:

The BlobSeer DSI has been tested with the following external dependencies:

 1. BlobSeer - release 0.3.3
 2. BlobSeer NamespaceManager 
 3. Globus GridFTP 5.0

==================
INSTALLING 
==================
Blobseer DSI for GridFTP is build by using by several ways ; 

1. Using Make 
modifiy the make file and make file header which are coresponding with the enviroment.

2. Using DSI bonnes 
related : http://www.globus.org/toolkit/docs/4.2/4.2.0/data/gridftp/developer/gridftp-developer-dsi.html
Create a BlobSeer DSI bonnes and register itself with the Globus extensions module properly.

Using script of GridFTP to generate C stubs for a DSI with all of the proper shared library hooks and names needed to work with the globus-gridftp-server. 

Go to the location gt5.0.0-all-source-installer/source-trees-thr/gridftp/server/src/dsi_bones
% ./generate-stubs.sh dsi_name flavor

This command will generate the new c source file and make file, make file headers as well. 
   dsi_name = blobseer
   flavor = gcc32dbgpthr / gcc64dbgpthr , since we use the asynchronous event approach, and it will work with pthread flavor. 

Then , overwrite the file names : globus_gridftp_server_blobseer.c and globus_gridftp_server_blobseer.h which have been created with corresponding filename in this package.  

% make

This will compile the DSI and create the dynamically loadable library. 

modify the make file by adding the library lfbs_namespace-c and include path . 

It may look like 
/usr/bin/gcc -g -Wall -I/usr/local/globus5/include/gcc32dbg -I/home/globus/blobseer/contrib/NamespaceManager/FBlobseerClient/clib -lfbs_namespace-c \
		-shared -o libglobus_gridftp_server_blobseer_gcc32dbg.so \
		globus_gridftp_server_blobseer.c \
		-L/usr/local/globus5/lib -L/usr/local/globus5/lib  -L/home/globus/blobseer/contrib/NamespaceManager/lib 

Finally, It will create a shared library which is named like libglobus_gridftp_server_blobseer_gcc32dbgpthr.so

% make install

This will copy the library to $GLOBUS_LOCATION/lib, thereby making it ready for use.
==================
CONFIGURATION
==================

There are 2 configuration file : blobseer configuration & gridftp configuration. 

A blobseer configuration file is required to specify parameters. By default, this file is expected to be at /etc/blobseer.cfg. If you need to point to a file in a different location, specify the path with the -dsi option to globus-gridftp-server : -dsi blobseer:/home/globus/test/blobseer.cfg

A gridftp configuration file will point to load blobseer dsi parameters to the gridftp server. A sample of configuration file is attached in this package: 
	gridftp.conf : a blobseer dsi configuration file in which the block_size parameter is very important related to the throughput.
	gridftp_simple.conf : a sample for normal gridftp configuration 
	

==================
RUNING 
==================
Before running gridftp server blobseer dsi, please make sure that all blobseer services and blobseer namespace service are running. 

set LD_LIBRARY_PATH to the namespace library
ex : 
set LD_LIBRARY_PATH=/home/globus/blobseer/contrib/NamespaceManager/lib/

also set LD_LIBRARY_PATH to the BlobSeer DSI library in $GLOBUS_LOCATION/lib 

Load BlobSeer DSI using configuration file (recommended)
		globus-gridftp-server -c gridftp.conf 
 or using load dsi option 
 		globus-gridfpt-server -dsi blobseer
Note : with the pthread build, using the globus-gridftp-server build binary in $GLOBUS_LOCATION/sbin/cc32dbgpthr/shared/
Then then client can connect to gridftp server with blobseer DSI using some commands such as : globus-url-copy 

