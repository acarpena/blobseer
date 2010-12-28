#!/bin/bash

export LD_LIBRARY_PATH=$HOME/deploy/lib
PATH=Clients/HavegeBS:$PATH
havegeBS scripts/blobseer-deploy.cfg $1 > /tmp/havegeBS.stdout 2> /tmp/havegeBS.stderr
