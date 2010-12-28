id=`basename $1`
echo $id
source env.sh $id
echo $OAR_JOB_ID
