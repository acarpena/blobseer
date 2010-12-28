#!/bin/bash

#arguments: current_run
CURRENT_RUN=$1


# Client to run
export CLIENT_PROGRAM="$BSCLIENTS_TESTCLIENT/malicious_client"
# Parameters
if [ "$CLIENT_TYPE" == "" ]
then
    CLIENT_TYPE="0"
fi

HOST=`hostname`
export CLIENT_BLOB=`cat $HOME/bsScripts/conf/clients.txt | grep -n "^$HOST\([^0-9].*\)\?\$" | cut -d ':' -f 1`
 #$HOME/bsScripts/conf/clients.txt | grep -n $HOST`  

export NO_CLIENTS=`cat $HOME/bsScripts/conf/clients.txt | wc -l `
export CLIENT_TYPE=$(($CLIENT_BLOB % 3))

echo  "client blob: $HOST $CLIENT_BLOB"

# ./malicious_client <client_type> <op> <blob id> <page size> <offset> <write size> <config_file>
export CLIENT_PARAMS="$CLIENT_TYPE $CLIENT_OP $CLIENT_BLOB $CLIENT_PAGE_SIZE $CLIENT_OFFSET $CLIENT_WRITE_SIZE $BS_CONFIG_FILE"

#echo  "client params: $CLIENT_PARAMS"

# number of concurrent clients in the same run
MAX_CONC_CLIENTS=$(($NO_CLIENTS / $RUN_STEPS))
# the id of the client (between 0 and NO_CLIENTS-1)
CLIENT_ID=$(($CLIENT_BLOB - 1))

# check if the client belongs to the range of MAX_CONC_CLIENTS that have to be executed in the CURRENT_RUN
CLIENT_RANGE=$(($CLIENT_ID / $MAX_CONC_CLIENTS))
CLIENT_RANGE=$(($CLIENT_RANGE + 1))

echo "Client $CLIENT_ID range: $CLIENT_RANGE current run:$CURRENT_RUN "

if [ $CLIENT_RANGE -eq $CURRENT_RUN ]; then

	$BSCLIENTS_TESTCLIENT/malicious_client $CLIENT_PARAMS
fi
