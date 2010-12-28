#!/bin/bash                                                            
# arguments: number of clients to start, number of runs for each client

#OUTPUT=$HADOOP_HOME/bench-logs/results.txt

head -n $1 $DATA_NODES > $TEST_SLAVES

#MASTER=`cat $HADOOP_HOME/conf/masters`
#oarsh $MASTER "$HADOOP_HOME/bin/stop-dfs.sh"
#sleep 10
#rm -rf $HADOOP_HOME/logs
#oarsh $MASTER "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/start-dfs.sh"
#sleep 10

#taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "rm -rf $CLIENT_OUTPUT" ]

for i in $(seq 1 $2); do
    echo "Executing wave $i..."
    $SCRIPTS_HOME/blobseer-deploy.sh
    taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_wr_df.sh 1" ]
   # $SCRIPTS_HOME/hadoop-deploy.sh
    $SCRIPTS_HOME/write_all.sh $1
    taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_rd_df.sh 20" ]
    echo "Done, sleeping for 30 secs..."
    sleep 30
    tar cjf $HADOOP_HOME/results/read-$1-run-$i-`date +%y-%m-%d-%H-%M`.tar.bz2 $HADOOP_HOME/logs
done
#echo "Testing with $1 clients" >> $OUTPUT
#taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/compute_average.sh" ] | grep output >> $OUTPUT
