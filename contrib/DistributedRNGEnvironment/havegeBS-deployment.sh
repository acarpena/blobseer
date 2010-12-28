#!/bin/bash

NB_PROVIDERS=5
NB_SDHTS=3
PAGE_SIZE=1048576
NB_REPLICA=1
GET_LOG_FREQ="2m"
CLIENT_SCRIPT=Clients/HavegeBS/deploy_clients.sh

./deploy-all.sh $NB_PROVIDERS $NB_SDHTS

BLOB_ID=$(Softwares/blobseer-0.3/test/create_blob scripts/blobseer-deploy.cfg $PAGE_SIZE $NB_REPLICA | sed -e "s/.*id = \(\d*\)/\1/" | head -n1)
echo havegeBS blob: $BLOB_ID

$CLIENT_SCRIPT scripts/clients $BLOB_ID

while true; do
    sleep $GET_LOG_FREQ
    ./get_logs.sh havegeBS $BLOB_ID
done
