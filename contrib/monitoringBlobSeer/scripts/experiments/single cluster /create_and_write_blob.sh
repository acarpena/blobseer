#!/bin/bash

#export LD_LIBRARY_PATH=$HOME/deploy/lib:$HOME/trunk/lib

if [ $# -lt 4 ]
then
    echo "use $0 <blobseer config file> <page size> <stage_size> <stages>"
    exit 1
fi

CONFIG=$1
PAGE_SIZE=$2
STAGE_SIZE=$3
STAGES=$4

echo "Creating blob"
BLOB_ID=`$HOME/deploy/release-0.3/test/create_blob $CONFIG $PAGE_SIZE 1 | head -n 1 | cut -d'=' -f3- | sed s/' '//g`
echo "Writing into blob $BLOB_ID"

$HOME/deploy/release-0.3/test/op_blob W $BLOB_ID $PAGE_SIZE $STAGES $STAGE_SIZE 90 $CONFIG > /tmp/client.log
