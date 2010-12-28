#!/bin/bash

GLOBAL=`sh check_global_config.sh`
echo "Setting the global configuration variables: $GLOBAL"
source $GLOBAL


if [ $# -lt 1 ]
then 
    echo "Usage: $0 <test_name>"
    exit 1
fi


# set the environment variables associated with the test
source $BS_SCRIPTS/env.sh
source $BSCLIENTS_SCRIPTS/env.sh
source $TEST_SCRIPTS/$1/env.sh

echo "Executing test $1"

for clients in $CLIENTS_LIST
do
    for repeat in `seq $REPEAT`
	do
		echo "Test $repeat for $clients clients..."

		# config and deploy nodes
		sh $LAUNCH_ALL_SCRIPTS/config_all.sh 

	    sh $LAUNCH_ALL_SCRIPTS/deploy_all.sh 
    
		 # create the list of clients
   		 sh $TEST_SCRIPTS/$1/create_clients_list.sh $clients
    
		 # run the test
     	 sh $UTIL_TESTS/test.sh $clients 
		    
		 echo "Done test $repeat for $clients clients."

		echo ""
 		 echo ""			
	done
done

