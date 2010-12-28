#!/bin/bash                                                            
# arguments: number of clients to start, number of runs for each client

OUTPUT=$HADOOP_HOME/bench-logs/results.txt
oarsh $3 "source $SCRIPTS_HOME/env.sh; rm -rf $CLIENT_OUTPUT"
#for i in $(seq 1 $2); do
#    echo "Executing wave $i..."
    oarsh $3 "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop WriteBigFile $TEST_FILE $1 $CHUNK_SIZE >> $CLIENT_OUTPUT"
    echo "Done"
#    sleep 3
#done
echo "Testing with $1 clients" >> $OUTPUT
oarsh $3 "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/compute_average.sh" >> $OUTPUT
