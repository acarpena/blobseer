#!/bin/bash
if [ -n "$1" ];then
	NODE=$1
else 
	echo "Usage: create_tunnel.sh node_name"
	exit 0
fi

echo $NODE



# monalisa.cern.ch Tunnels
#
ssh -R 80:monalisa.cern.ch:80 root@$NODE -N &
sleep 0.1
ssh -R 6001:monalisa.cern.ch:6001 blobmon@$NODE -N &
sleep 0.1
ssh -R 6002:monalisa.cern.ch:6002 blobmon@$NODE -N &
sleep 0.1
ssh -R 6003:monalisa.cern.ch:6003 blobmon@$NODE -N &
sleep 0.1
ssh -R 4160:monalisa.cern.ch:4160 blobmon@$NODE -N &
sleep 0.1
ssh -R 8765:monalisa.cern.ch:8765 blobmon@$NODE -N &
sleep 0.1
#
# monalisa.cacr.caltech.edu  Tunnels
#
ssh -R 8080:monalisa.cacr.caltech.edu:80 root@$NODE -N &
sleep 0.1
ssh -R 8001:monalisa.cacr.caltech.edu:6001 blobmon@$NODE -N &
sleep 0.1
ssh -R 8002:monalisa.cacr.caltech.edu:6002 blobmon@$NODE -N &
sleep 0.1
ssh -R 8003:monalisa.cacr.caltech.edu:6003 blobmon@$NODE -N &
sleep 0.1
ssh -R 8160:monalisa.cacr.caltech.edu:4160 blobmon@$NODE -N &
sleep 0.1
ssh -R 8760:monalisa.cacr.caltech.edu:8765 blobmon@$NODE -N &
sleep 0.1
#
# monalisa.caltech.edu Tunnels
#
ssh -R 5001:monalisa.caltech.edu:6001 blobmon@$NODE -N &
sleep 0.1
ssh -R 5002:monalisa.caltech.edu:6002 blobmon@$NODE -N &
sleep 0.1
ssh -R 5003:monalisa.caltech.edu:6003 blobmon@$NODE -N &
sleep 0.1
#
#
# monalisa2.cern.ch Tunnels
#
ssh -R 5101:monalisa2.cern.ch:6001 blobmon@$NODE -N &
sleep 0.1
ssh -R 5102:monalisa2.cern.ch:6002 blobmon@$NODE -N &
sleep 0.1
ssh -R 5103:monalisa2.cern.ch:6003 blobmon@$NODE -N &
sleep 0.1
#
#
# monalisa-ul.caltech.edu Tunnels
#
ssh -R 5201:monalisa-ul.caltech.edu:6001 blobmon@$NODE -N &
sleep 0.1
ssh -R 5202:monalisa-ul.caltech.edu:6002 blobmon@$NODE -N &
sleep 0.1
ssh -R 5203:monalisa-ul.caltech.edu:6003 blobmon@$NODE -N &
echo 'if no other msgs prompted, the tunnels are working now!!'

