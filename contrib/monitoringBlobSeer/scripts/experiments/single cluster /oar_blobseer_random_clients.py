#!/usr/bin/python
import sys
import time
import random

import oar_tools

# --------------------------------------------------------------------------------

if (len(sys.argv) == 6) :

    #slice_name = sys.argv[1]
    #slice_key = sys.argv[2]
    nodes_file = sys.argv[1]
    bs_file = sys.argv[2]
    num_clients = int(sys.argv[3])
    num_blobs = int(sys.argv[4])
    write_count=int(sys.argv[5])
    #oar_tools.ssh_key = slice_key
    #oar_tools.slice_name = slice_name

    # Reading node list
    nodes = oar_tools.read_nodes(nodes_file)

    # Determining random nodes for clients
    clients = []
    all_nodes_index = range(len(nodes))
    while (len(all_nodes_index) > 0) and (len(clients) < num_clients) :
        pos = int(random.random()*(len(all_nodes_index)-1))
        node_index = all_nodes_index.pop(pos)
        clients.append(nodes[node_index])

    # Starting clients
    print "Starting clients in "+str(clients)
    oar_tools.execute_command(clients, ["/home/rennes/jcai/deploy/workload/random_client.sh", bs_file,
                                       str(num_blobs),str(write_count)])

else :
    print "use: "+ sys.argv[0] + " <nodes list file> <blobseer config file> <num clients> <num blobs> <write time>"
