#!/bin/bash
#HOST_FILE=/tmp/localhost.txt
#TEMPLATE_FILE=$BLOBSEER_HOME/scripts/blobseer-template.cfg
#echo "127.0.0.1" > $HOST_FILE
#$BLOBSEER_HOME/scripts/blobseer-deploy.py --vmgr=`cat $HOST_FILE` --pmgr=`cat $HOST_FILE` --dht=$HOST_FILE --providers=$HOST_FILE --launch=$TEMPLATE_FILE
#rm $HOST_FILE

killall vmanager
killall pmanager
killall sdht
killall provider

$BLOBSEER_HOME/vmanager/vmanager $BLOBSEER_HOME/test/test.cfg  >/tmp/vmanager.log &
$BLOBSEER_HOME/pmanager/pmanager $BLOBSEER_HOME/test/test.cfg >/tmp/pmanager.log &
$BLOBSEER_HOME/provider/sdht $BLOBSEER_HOME/test/test.cfg >/tmp/sdht.log &
$BLOBSEER_HOME/provider/provider $BLOBSEER_HOME/test/test.cfg >/tmp/provider.log &
