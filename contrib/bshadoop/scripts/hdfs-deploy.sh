#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

# Cleanup
MASTER=`cat $HADOOP_HOME/conf/masters`
taktuk -s -c oarsh -f $HADOOP_HOME/conf/machines.txt broadcast exec [ "$SCRIPTS_HOME/cleanup.sh" ]
rm -rf $HADOOP_HOME/logs

# Start HDFS
oarsh $MASTER "$HADOOP_HOME/bin/hadoop namenode -format"
oarsh $MASTER "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/start-dfs.sh"

perl -pi -e "s/\w\wfs:\/\//hdfs:\/\//" $HADOOP_HOME/conf/core-site.xml

sleep 20
echo "Deployment complete..."
