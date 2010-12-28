#!/bin/bash
# Test settings

export TEST_NAME="mal_correct"

export UTIL_TESTS="$TEST_SCRIPTS/util_tests"
export ARCHIVES="$G5K_DEPLOY_HOME/results"

#Test configuration
export RUN_STEPS="1"

export CLIENT_TYPE="1"
export CLIENT_OP="W"

export CLIENT_REPLICAS="1"

#export CLIENT_PAGE_SIZE="524288"        # 512 KB
#export CLIENT_PAGE_SIZE="67108864"        # 64 MB
#export CLIENT_PAGE_SIZE="1048576"        # 1 MB
#export CLIENT_PAGE_SIZE="2097152"        # 2 MB
export CLIENT_PAGE_SIZE="8388608"		# 8 MB

export CLIENT_OFFSET="0"                # the beginning
#export CLIENT_WRITE_SIZE="33554432"     # 64 pages -> 32 MB


export CLIENT_WRITE_SIZE="67108864"     # 128 pages -> 64 MB
#export CLIENT_WRITE_SIZE="1073741824" 		    # 1 GB
#export CLIENT_WRITE_SIZE="536870912"          # 512 MB

# Create a list with the number of clients that have to be tested
#export CLIENTS_LIST="10 20 30 40 50 60 70 80 90 100 110 120 130 140"
# 80 90 100 110 120 130 140 160"
#export CLIENTS_LIST="65 60 50 40 30 20 5 3 10"
export CLIENTS_LIST="90"


#Create a file containing the list of clients
export UNUSED_NODES="$G5K_DEPLOY_HOME/conf/unused_nodes.txt" 
export PROVIDERS="$G5K_DEPLOY_HOME/conf/providers.txt"

export CLIENTS_FILE="$G5K_DEPLOY_HOME/conf/clients.txt"



# Create Blobs
export CLIENT_CREATE_BLOBS="true"   # set this variable to enable blob creation


#Program that creates the Blob
export CREATE_BLOBS_PROGRAM="$BLOBSEER_HOME/test/create_blob"
#Parameters
# ./create_blob <config_file> <page_size> <replica_count>
export CREATE_BLOBS_PARAMS="$BS_CONFIG_FILE $CLIENT_PAGE_SIZE $CLIENT_REPLICAS"




#Repeat test
export WAIT_TIME=200
export REPEAT="1"






