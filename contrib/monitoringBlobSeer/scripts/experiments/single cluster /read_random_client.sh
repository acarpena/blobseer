#!/bin/bash

#export LD_LIBRARY_PATH=$HOME/deploy/lib:$HOME/trunk/lib

if [ $# -lt 3 ]
then
    echo "use $0 <blobseer config file> <number of blobs> <write times>"
    exit 1
fi

CONFIG=$1
MAX_BLOBS=$2
WRITE_TIMES=$3

COUNT=0;

while true
do
    COUNT=$(($COUNT+1))
    if [ $COUNT -gt $WRITE_TIMES ]
    then
   	 exit 2
    fi

    OP=W
    OP_COD=$(($RANDOM%2))
    if [ $OP_COD = 1 ]
    then
	OP=R
	#OP=W
    fi
    BLOB_ID=$((($RANDOM%$MAX_BLOBS)+1))
    PAGES=$((($RANDOM%1024)+1025))
    MAX_OFFSET=$((1024*20-9*100))
    OFFSET=$((($RANDOM%$MAX_OFFSET)+1))
    echo $OFFSET
    PAGE_SIZE=$((1024*1024))
    OFFSET=$(($OFFSET*$PAGE_SIZE))
    STAGE_SIZE=$(($PAGES*$PAGE_SIZE))
    OP=R
    echo "Performing $OP operation over blob $BLOB_ID"
    echo "blobid: $BLOB_ID stage_Size: $PAGES max_offset: $MAX_OFFSET offset:$OFFSET page-size $PAGE_SIZE"
    $HOME/deploy/release-0.3/test/op $OP $BLOB_ID 1048576 1 $PAGES $OFFSET $CONFIG 2>&1 >> /tmp/client.log

    TIME=$(($RANDOM%6))

    echo "Sleeping for $TIME seconds"
    sleep $TIME

done
