#!/bin/bash
$ML_CONFIG/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi


#delete database

DELT="rm -rf $ML_HOME/Service/myFarm/*.log"
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $DELT ] > /dev/null


DELT="cd $ML_HOME/Service/myFarm/pgsql; export LD_LIBRARY_PATH=$ML_HOME/Service/myFarm/pgsql/lib:$LD_LIBRARY_PATH; export PGLIB=$ML_HOME/Service/myFarm/pgsql/lib; export PATH=\"$ML_HOME/Service/myFarm/pgsql/bin:$PATH\"; ./bin/psql -U mon_user -h 127.0.0.1 -p `cat pgsql.port` mon_data -c\"DROP TABLE monitor_s_10hour;\" "
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $DELT ] > /dev/null

DELT="cd $ML_HOME/Service/myFarm/pgsql; export LD_LIBRARY_PATH=$ML_HOME/Service/myFarm/pgsql/lib:$LD_LIBRARY_PATH; export PGLIB=$ML_HOME/Service/myFarm/pgsql/lib; export PATH=\"$ML_HOME/Service/myFarm/pgsql/bin:$PATH\"; ./bin/psql -U mon_user -h 127.0.0.1 -p `cat pgsql.port` mon_data -c\"DROP TABLE monitor_s_3hour_backup_rrd; \" "
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $DELT ] > /dev/null

DELT="cd $ML_HOME/Service/myFarm/pgsql; export LD_LIBRARY_PATH=$ML_HOME/Service/myFarm/pgsql/lib:$LD_LIBRARY_PATH; export PGLIB=$ML_HOME/Service/myFarm/pgsql/lib; export PATH=$ML_HOME/Service/myFarm/pgsql/bin:$PATH; ./bin/psql -U mon_user -h 127.0.0.1 -p `cat pgsql.port` mon_data -c\"DROP TABLE monitor_s_3hour;\" "
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $DELT ] > /dev/null

DELT="cd $ML_HOME/Service/myFarm/pgsql; export LD_LIBRARY_PATH=$ML_HOME/Service/myFarm/pgsql/lib:$LD_LIBRARY_PATH; export PGLIB=$ML_HOME/Service/myFarm/pgsql/lib; export PATH=$ML_HOME/Service/myFarm/pgsql/bin:$PATH; ./bin/psql -U mon_user -h 127.0.0.1 -p `cat pgsql.port` mon_data -c\"DROP TABLE monitor_s_10hour_backup_rrd ;\" "
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $DELT ] > /dev/null




# Stop MonALISA
CMD="$ML_HOME/Service/CMD/ML_SER stop"
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $CMD ]

# Start the MonALISA services
CMD="$ML_HOME/Service/CMD/ML_SER start"
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $CMD ]

#wait for the service to start
echo 'Waiting for the service to start...'
sleep 20

#check if MonALISA is running
CMD="$ML_HOME/Service/CMD/ML_SER status | grep 'PID'"
RUNNING=`taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ $CMD ] | \
		grep "status 0" | wc -l`

echo "MonALISA services Deployment complete: $RUNNING running MonALISA services"
