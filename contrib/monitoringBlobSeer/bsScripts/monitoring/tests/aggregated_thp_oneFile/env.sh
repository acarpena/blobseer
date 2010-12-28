#!/bin/bash
# Test settings

export TEST_NAME="aggregated_thp_oneFile"
export UTIL_TESTS="$TEST_SCRIPTS/util_tests"
export ARCHIVES="$G5K_DEPLOY_HOME/results"

#Test configuration
export CLIENT_TYPE="0"
export CLIENT_OP="W"
export CLIENT_BLOB="1" 
export CLIENT_REPLICAS="1"

export CLIENT_PAGE_SIZE="524288"        # 512 KB

export CLIENT_OFFSET="0"                # the beginning

export CLIENT_WRITE_SIZE="33554432"     # 32 MB
#export CLIENT_WRITE_SIZE="67108864"     # 64 MB

# Create a list with the number of clients that have to be tested
#export CLIENTS_LIST="10 20 30 40 50 60 70 80 90 100 120 140"
export CLIENTS_LIST="1"



#Create a file containing the list of clients
export UNUSED_NODES="$G5K_DEPLOY_HOME/conf/unused_nodes.txt" 
export PROVIDERS="$G5K_DEPLOY_HOME/conf/providers.txt"

export CLIENTS_FILE="$G5K_DEPLOY_HOME/conf/clients.txt"




# Create Blobs
export CLIENT_CREATE_BLOBS="true"   # set this variable to enable blob creation
#Number of blobs
export BLOBS="1"

#Client machine for creating the BLOBs
export CREATE_NODE=`head -n 1 $G5K_DEPLOY_HOME/conf/clients.txt`

#Program that creates the Blob
export CREATE_BLOBS_PROGRAM="$BLOBSEER_HOME/test/create_blob"
#Parameters
# ./create_blob <config_file> <page_size> <replica_count>
export CREATE_BLOBS_PARAMS="$BS_CONFIG_FILE $CLIENT_PAGE_SIZE $CLIENT_REPLICAS"




#Repeat test
export WAIT_TIME=20
export REPEAT="5"

# Client to run
export CLIENT_PROGRAM="$BSCLIENTS_TESTCLIENT/malicious_client"
# Parameters
if [ $CLIENT_TYPE == "" ]
then
    CLIENT_TYPE="0"
fi
# ./malicious_client <client_type> <op> <blob id> <page size> <offset> <write size> <config_file>
export CLIENT_PARAMS="$CLIENT_TYPE $CLIENT_OP $CLIENT_BLOB $CLIENT_PAGE_SIZE $CLIENT_OFFSET $CLIENT_WRITE_SIZE $BS_CONFIG_FILE"





