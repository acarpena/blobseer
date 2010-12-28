#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then
    echo "Environment is not configured properly. Check env.sh and source it."
    exit 1
fi

CONF_DIR=$HADOOP_HOME/conf

oargridstat -l $OAR_JOB_ID | sort | uniq | grep $CLUSTER >$CONF_DIR/ttemp.txt

#cat $OAR_NODEFILE | sort | uniq >$CONF_DIR/ttemp.txt
 
taktuk -c oarsh -s -f $CONF_DIR/ttemp.txt broadcast exec [ "$SCRIPTS_HOME/cleanup.sh; test -e $HADOOP_HOME" ] | grep "status 0" | cut -d '.' -f 1 > $CONF_DIR/tttemp.txt
$SCRIPTS_HOME/add_myri.sh $CONF_DIR/tttemp.txt
taktuk -s -d -1 -c oarsh -f $CONF_DIR/tttemp.txt broadcast exec [ "$SCRIPTS_HOME/cleanup.sh; test -e $HADOOP_HOME" ] | grep "status 0" | cut -d ':' -f 1| cut -d '-' -f 1-3  > $CONF_DIR/temp.txt

LINES=`cat $CONF_DIR/temp.txt | wc -l`
tail -n `expr $LINES - 2` $CONF_DIR/temp.txt >$CONF_DIR/machines.txt
head -n 1 $CONF_DIR/temp.txt >$CONF_DIR/jobtracker.txt
head -n 2 $CONF_DIR/temp.txt | tail -n 1 >$CONF_DIR/client.txt

echo Checking reservation $OAR_JOB_ID: $LINES machines out of `cat $CONF_DIR/ttemp.txt | wc -l` are configured properly and usable.
rm -rf $CONF_DIR/temp.txt $CONF_DIR/ttemp.txt $CONF_DIR/tttemp.txt

# BlobSeer configuration
NODES=`cat $CONF_DIR/machines.txt | wc -l`
head -n `expr $DHT_NO + 2` $CONF_DIR/machines.txt | tail -n $DHT_NO > $CONF_DIR/meta.txt
head -n -1 $CONF_DIR/machines.txt | tail -n `expr $NODES - $DHT_NO - 3` > $CONF_DIR/providers.txt
head -n `expr $DHT_NO + 2` $CONF_DIR/machines.txt >$CONF_DIR/temp.txt
$SCRIPTS_HOME/gen_cfgfile.pl $CONF_DIR/temp.txt $DHT_NO $CONF_DIR/running.cfg
rm -rf $CONF_DIR/temp.txt

# Hadoop configuration
cp $CONF_DIR/providers.txt $CONF_DIR/slaves
tail -n 1 $CONF_DIR/machines.txt  > $CONF_DIR/masters
MASTER=`cat $CONF_DIR/masters`
JOBTRACKER=`cat $CONF_DIR/jobtracker.txt`
perl -pi -e "s/[-\.\w]*:9000/${MASTER}:9000/" $HADOOP_HOME/conf/core-site.xml
perl -pi -e "s/[-\.\w]*:9000/${JOBTRACKER}:9000/" $HADOOP_HOME/conf/mapred-site.xml
perl -pi -e "s/<name>fs.bsfs.metaServerHost<\/name>[\s\n]*<value>[-\.\w]*<\/value>/<name>fs.bsfs.metaServerHost<\/name> <value>${MASTER}<\/value>/" $HADOOP_HOME/conf/core-site.xml

# Other cleanup: remove ~/.ssh/known_hosts on all nodes
taktuk -s -c oarsh -f $CONF_DIR/machines.txt broadcast exec [ "rm -rf $HOME/.ssh/known_hosts" ]
