#!/bin/bash


# Client to run
export CLIENT_PROGRAM="$BSCLIENTS_TESTCLIENT/malicious_client"
# Parameters
if [ "$CLIENT_TYPE" == "" ]
then
    CLIENT_TYPE="0"
fi

HOST=`hostname`
export CLIENT_BLOB=`cat $HOME/bsScripts/conf/clients.txt | grep -n "$HOST\([^0-9].*\)\?\$" | cut -d ':' -f 1`
 #$HOME/bsScripts/conf/clients.txt | grep -n $HOST`  



echo  "client blob: $HOST $CLIENT_BLOB"

# ./malicious_client <client_type> <op> <blob id> <page size> <offset> <write size> <config_file>
export CLIENT_PARAMS="$CLIENT_TYPE $CLIENT_OP $CLIENT_BLOB $CLIENT_PAGE_SIZE $CLIENT_OFFSET $CLIENT_WRITE_SIZE $BS_CONFIG_FILE"

echo  "client params: $CLIENT_PARAMS"

$BSCLIENTS_TESTCLIENT/malicious_client $CLIENT_PARAMS
