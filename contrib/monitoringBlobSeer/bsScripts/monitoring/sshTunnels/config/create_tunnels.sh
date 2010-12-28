#!/bin/bash

if [ $# -ne 2 ] ; then        # if the number of params is not 2
    echo Usage: $0 node first_port
    exit 1
fi

NODE=$1
PORT=$2

echo "Creating tunnels to $NODE - starting from port $PORT"



# monalisa.cern.ch Tunnels
# -o stricthostkeychecking=no 
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cern.ch:80 root@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cern.ch:8765 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cern.ch:4160 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cern.ch:6002 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cern.ch:6001 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cern.ch:6003 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))

#
# monalisa.cacr.caltech.edu  Tunnels
#
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cacr.caltech.edu:80 root@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cacr.caltech.edu:8765 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cacr.caltech.edu:4160 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cacr.caltech.edu:6002 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cacr.caltech.edu:6001 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.cacr.caltech.edu:6003 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))

#
# monalisa.caltech.edu Tunnels
#
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.caltech.edu:6002 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.caltech.edu:6001 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa.caltech.edu:6003 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))

#
#
# monalisa2.cern.ch Tunnels
#
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa2.cern.ch:6002 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa2.cern.ch:6001 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa2.cern.ch:6003 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))

#
#
# monalisa-ul.caltech.edu Tunnels
#
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa-ul.caltech.edu:6002 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa-ul.caltech.edu:6001 blobmon@$NODE -N &
sleep 0.1
PORT=$(($PORT + 1))
ssh  -o stricthostkeychecking=no   -R $PORT:monalisa-ul.caltech.edu:6003 blobmon@$NODE -N &
sleep 0.1

echo "Tunnels created - last port is $PORT"




