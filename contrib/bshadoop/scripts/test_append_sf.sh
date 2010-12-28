#!/bin/bash                                                            
# arguments: number of clients to start, number of runs for each client

#OUTPUT=$HADOOP_HOME/bench-logs/results.txt

head -n $1 $DATA_NODES > $TEST_SLAVES
sleep 10

for i in $(seq 1 $2); do
    echo "Executing wave $i..."
    $SCRIPTS_HOME/blobseer-deploy.sh
    NODE=`head -n 1 $DATA_NODES`
    oarsh $NODE "$HADOOP_HOME/bin/hadoop fs -touchz $TEST_FILE > /dev/null"

    taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_ap_sf.sh 20" ]
    echo "Done, now sleeping for 30 secs..."
    sleep 30
    tar cjf $HADOOP_HOME/results/append-$1-run-$i-`date +%y-%m-%d-%H-%M`.tar.bz2 $HADOOP_HOME/logs

done
#echo "Testing with $1 clients" >> $OUTPUT
#taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/compute_average.sh" ] | grep output >> $OUTPUT
