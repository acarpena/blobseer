include makefile_header
# include additional makefile headers here

# add needed cflags here
DSI_CFLAGS=$(GLOBUS_CFLAGS)

# add needed includes here
DSI_INCLUDES=$(GLOBUS_INCLUDES)

# added needed ldflags here
DSI_LDFLAGS=$(GLOBUS_LDFLAGS)

#added namespace include here
NS_INCLUDES=-I/home/globus/blobseer/contrib/NamespaceManager/FBlobseerClient/clib

#added namespace library here
NS_LIBS=-L/home/globus/blobseer/contrib/NamespaceManager/lib

# add needed libraries here
DSI_LIBS=

FLAVOR=gcc32dbg

globus_gridftp_server_blobseer.o:
	$(GLOBUS_CC) $(DSI_CFLAGS) $(DSI_INCLUDES) $(NS_INCLUDES) -lfbs_namespace-c \
		-shared -o libglobus_gridftp_server_blobseer_$(FLAVOR).so \
		globus_gridftp_server_blobseer.c \
		$(DSI_LDFLAGS) $(DSI_LIBS) ${NS_LIBS} 

install:
	cp -f libglobus_gridftp_server_blobseer_$(FLAVOR).so $(GLOBUS_LOCATION)/lib

clean:
	rm -f *.so
