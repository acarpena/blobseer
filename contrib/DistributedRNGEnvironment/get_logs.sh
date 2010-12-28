#!/bin/bash

date +"%y.%m.%d-%H.%M"
DIR=logs/$(date +"%y.%m.%d-%H.%M")
mkdir $DIR
for c in $(cat scripts/nodes1)
do
    mkdir $DIR/$c/
    oarsh $c cp -R /tmp/blobseer $DIR/$c &
    oarsh $c cp /tmp/$1* $DIR/$c &
done
Clients/DumpBlob/dump_blob scripts/blobseer-deploy.cfg 1 $DIR/blobseer.dump
