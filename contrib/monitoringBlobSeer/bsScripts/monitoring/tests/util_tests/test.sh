#!/bin/bash

if [ $# -lt 1 ]
then
    echo "Usage: $0 <number_of_clients>"
    exit 1
fi

CLIENTS=$1


    # create the necessary blobs 
    if [ "$CLIENT_CREATE_BLOBS" = "true" ]
    then
        echo "Creating $BLOBS blobs..."
        source $TEST_SCRIPTS/$TEST_NAME/conf_create_blobs.sh $CLIENTS
		sh $UTIL_TESTS/create_blob.sh
        echo "done"
    fi
	sleep 5
    
    # run the client:
    echo "Running test $TEST_NAME for $CLIENTS clients..."
    sh $TEST_SCRIPTS/$TEST_NAME/run_client.sh
	echo "done"
  
	# wait for the write to end
	echo "Waiting for $WAIT_TIME seconds..."
    sleep $WAIT_TIME
    echo "done"

	# archive the results
    echo "Archiving results for  $CLIENTS clients..."
	sh $UTIL_TESTS/archive_logs.sh $TEST_NAME
    echo "done"

	# clean
	echo "Cleaning..."
	sh $UTIL_TESTS/cleanup.sh
	echo "done"
   
sleep 5
echo 'Test completed'
