#!/bin/bash                                                            
# arguments: number of clients to start, number of runs for each client, what to deploy, on what node to write the file  

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
    $SCRIPTS_HOME/$3-deploy.sh
    oarsh $4 "$HADOOP_HOME/bin/hadoop WriteBigFile $TEST_FILE $1 $CHUNK_SIZE"
#    echo "Wrote file"
    sleep 10

    taktuk -s -c oarsh -f $TEST_SLAVES broadcast exec [ "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_rd_sf.sh 20" ]
    echo "Done, sleeping for 30 secs..."
    sleep 30
    tar cjf $HADOOP_HOME/results/$3-read_sf-$1-run-$i-`date +%y-%m-%d-%H-%M`.tar.bz2 $HADOOP_HOME/logs
done

