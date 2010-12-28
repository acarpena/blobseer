
#usage: ./MLserviceConfig.sh MonALISA_home
if [ $# -ne 1 ] ; then        # if the number of params is not 1
    echo Usage: $0 MonALISA_home
    exit 1
fi

ML_HOME=$1


#start MonALISA
cd $ML_HOME
./Service/CMD/ML_SER start


#wait for the service to start
echo 'Waiting for the service to start...'
sleep 20


#check if MonALISA is running
cd $ML_HOME
RUNNING=`./Service/CMD/ML_SER status | grep "MonaLisa is NOT running"`

if [ -z "$RUNNING" ] ; then
    echo "The MonALISA service BlobMon-$HOSTNAME is running"  
else
    echo "Could not start the MonALISA service BlobMon-$HOSTNAME"
    exit 1  
fi

