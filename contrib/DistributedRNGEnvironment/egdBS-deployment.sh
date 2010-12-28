#!/bin/bash

NB_PROVIDERS=5
NB_SDHTS=3
PAGE_SIZE=1024
NB_REPLICA=1
GET_LOG_FREQ="1h"
CLIENT_SCRIPT=Clients/EgdBS/deploy_clients.sh

./Softwares/egd-0.9/egd.pl $SOCKET

./deploy-all.sh $NB_PROVIDERS $NB_SDHTS

BLOB_ID=$(Softwares/blobseer-0.3/test/create_blob scripts/blobseer-deploy.cfg $PAGE_SIZE $NB_REPLICA | sed -e "s/.*id = \(\d*\)/\1/" | head -n1)
echo egdBS blob: $BLOB_ID

$CLIENT_SCRIPT scripts/clients $BLOB_ID

while true; do
    sleep $GET_LOG_FREQ
    ./get_logs.sh egdBS $BLOB_ID
done
