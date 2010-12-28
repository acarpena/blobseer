#!/bin/bash                                                            
# arguments: number of clients to start, number of runs for each client

#OUTPUT=$HADOOP_HOME/bench-logs/results.txt

head -n $1 $DATA_NODES > $TEST_SLAVES
#sleep 10

#taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "rm -rf $CLIENT_OUTPUT" ]
for i in $(seq 1 $2); do
    echo "Executing wave $i..."
    $SCRIPTS_HOME/hdfs-deploy.sh
    taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_wr_df.sh 20" ]
  #  taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/log_client.sh" ]
    echo "Done, now sleeping for 30 secs..."
    sleep 30
    tar cjf $HADOOP_HOME/results/write-$1-run-$i-`date +%y-%m-%d-%H-%M`.tar.bz2 $HADOOP_HOME/logs 
done
#taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/log_client.sh" ]
#echo testing with $1 clients >> $OUTPUT
#taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/compute_average.sh" ] | grep output >> $OUTPUT
