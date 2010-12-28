vmgr=`sed -n '1p' nodes`
pmgr=`sed -n '2p' nodes`

#############

./blobseer-deploy.py -v $vmgr -m $pmgr -d dhts -p providers -s

