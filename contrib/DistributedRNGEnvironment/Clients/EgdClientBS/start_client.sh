#!/bin/bash

Softwares/egd-0.9/egd.pl /tmp/egd.socket
export LD_LIBRARY_PATH=$HOME/deploy/lib
PATH=Clients/EgdBS:$PATH
egdBS scripts/blobseer-deploy.cfg $1 /tmp/egd.socket > /tmp/egdBS.stdout 2> /tmp/egdBS.stderr
