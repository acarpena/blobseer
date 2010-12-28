vmgr=`sed -n '1p' nodes.txt`
pmgr=`sed -n '2p' nodes.txt`

#############

./blobseer-deploy-grid.py -v $vmgr -m $pmgr -d dhts.txt -p providers.txt -k

