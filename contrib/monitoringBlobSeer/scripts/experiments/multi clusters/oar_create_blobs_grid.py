#!/usr/bin/python
import sys
import time
import random

import oar_tools_grid

# --------------------------------------------------------------------------------

if (len(sys.argv) == 7) :

    #slice_name = sys.argv[1]
    #slice_key = sys.argv[2]
    nodes_file = sys.argv[1]
    bs_file = sys.argv[2]
    num_blobs = int(sys.argv[3])
    page_size = int(sys.argv[4])
    stage_size = int(sys.argv[5])
    stages = int(sys.argv[6])

    #oar_tools.ssh_key = slice_key
    #oar_tools.slice_name = slice_name

    # Reading node list
    nodes = oar_tools_grid.read_nodes(nodes_file)

    # Determining random nodes for clients
    #clients = []
    #all_nodes_index = range(len(nodes))
    #while (len(all_nodes_index) > 0) and (len(clients) < num_blobs) :
    #    pos = int(random.random()*(len(all_nodes_index)-1))
    #    node_index = all_nodes_index.pop(pos)
    #    clients.append(nodes[node_index])

    # Starting clients
    print "Starting clients in "+str(nodes)
    oar_tools_grid.execute_command(nodes, ["/home/rennes/jcai/deploy/workload/create_and_write_blob.sh", bs_file,
                                       str(page_size), str(stage_size), str(stages)])
    exit
else :
    print "use: "+ sys.argv[0] + " <nodes list file> <blobseer config file> <num blobs> <page size> <stage size> <stages>"

