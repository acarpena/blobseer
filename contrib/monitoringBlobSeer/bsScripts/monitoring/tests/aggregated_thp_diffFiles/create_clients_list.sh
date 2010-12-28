#!/bin/bash

if [ $# -lt 1 ]
then 
    echo "Usage: $0 <number_of_clients>"
    exit 1
fi

CLIENTS_NO=$1

# delete the old clients file
rm -rf $CLIENTS_FILE

# the clients are the unused nodes
cat $UNUSED_NODES | head -n $CLIENTS_NO > $CLIENTS_FILE

# rsync with other sites
echo "Copying configuration files to all the sites..."
$UTIL_SCRIPTS/sendConfigurationFile.sh
