#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

# Entities
VMANAGER=`head $HADOOP_HOME/conf/machines.txt -n 1`
PMANAGER=`head $HADOOP_HOME/conf/machines.txt -n 2 | tail -n 1`
HMANAGER=`tail $HADOOP_HOME/conf/machines.txt -n 1`

echo $HMANAGER >> namespace

# Clean logs & stuff
taktuk -s -c oarsh -f $HADOOP_HOME/conf/machines.txt broadcast exec [ "$SCRIPTS_HOME/cleanup.sh" ]
rm -rf $HADOOP_HOME/logs

# Start BlobSeer
mkdir -p $HADOOP_HOME/logs
oarsh $VMANAGER "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_and_log.sh $BLOBSEER_HOME/vmanager/vmanager $HADOOP_HOME/conf/running.cfg"
oarsh $PMANAGER "source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_and_log.sh $BLOBSEER_HOME/pmanager/pmanager $HADOOP_HOME/conf/running.cfg"

oarsh $HMANAGER "source $SCRIPTS_HOME/env.sh; cd $HADOOP_HOME/blobseer-fsmeta; $SCRIPTS_HOME/run_and_log.sh $JAVA_HOME/bin/java -cp build/:../build/classes/:lib/ NManager 9000 $HADOOP_HOME/conf/running.cfg"

#oarsh $HMANAGER "source $SCRIPTS_HOME/env.sh; cd $HADOOP_HOME/blobseer-fsmeta; $SCRIPTS_HOME/run_and_log.sh ruby server.rb 9000 8080 $HADOOP_HOME/conf/running.cfg"

CMD="source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_and_log.sh $BLOBSEER_HOME/provider/provider $HADOOP_HOME/conf/running.cfg"
taktuk -s -c oarsh -f $HADOOP_HOME/conf/providers.txt broadcast exec [ $CMD ]
CMD="source $SCRIPTS_HOME/env.sh; $SCRIPTS_HOME/run_and_log.sh $BLOBSEER_HOME/provider/sdht $HADOOP_HOME/conf/running.cfg"
taktuk -s -c oarsh -f $HADOOP_HOME/conf/meta.txt broadcast exec [ $CMD ]

perl -pi -e "s/\w\wfs:\/\//bsfs:\/\//" $HADOOP_HOME/conf/core-site.xml

sleep 20
echo 'Deployment complete'
