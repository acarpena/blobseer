#!/bin/bash

export LD_LIBRARY_PATH=$HOME/deploy/lib
PATH=Clients/RandomBS:$PATH
randomBS scripts/blobseer-deploy.cfg $1 > /tmp/randomBS.stdout 2> /tmp/randomBS.stderr
