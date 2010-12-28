#!/bin/bash

for c in $(cat $1)
do
    echo "oarsh $USER@$c Clients/EgdBS/start_client.sh $2 &"
    oarsh $USER@$c Clients/EgdBS/start_client.sh $2 &
done
