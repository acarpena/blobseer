#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

SLAVES=`cat $HADOOP_HOME/conf/slaves | wc -l`

count=$SLAVES
#while [ $count -gt $SLAVES ]
#do 
#    count=`expr $count - 20`
#done


echo Starting with $count clients
#while [ $count -lt $SLAVES ]

while [ $count -gt 0 ]
do
    # concurrent reads from the same file
 #   $SCRIPTS_HOME/hadoop-execrun.sh hdfs READ_SF_LOCAL-$count $SCRIPTS_HOME/test_read_sf.sh $count `head -n 1 $DATA_NODES`

    # concurrent reads from the same file
#    $SCRIPTS_HOME/hadoop-execrun.sh hdfs READ_SF_REMOTE-$count $SCRIPTS_HOME/test_read_sf.sh $count `cat $HADOOP_HOME/conf/masters`

    # concurrent writes to different files
#    $SCRIPTS_HOME/hadoop-execrun.sh hdfs WRITE-$count $SCRIPTS_HOME/test_write_df.sh $count

    # concurrent reads from different files
#    $SCRIPTS_HOME/hadoop-execrun.sh hdfs READ-$count $SCRIPTS_HOME/test_read_df.sh $count

    # concurrent writes to different files
#    $SCRIPTS_HOME/hadoop-execrun.sh blobseer WRITE-$count $SCRIPTS_HOME/test_write_df.sh $count
    
    # concurrent reads from different files
    $SCRIPTS_HOME/hadoop-execrun.sh blobseer READ-$count $SCRIPTS_HOME/test_read_df.sh $count

    # concurrent reads from the same file
#    $SCRIPTS_HOME/hadoop-execrun.sh blobseer READ_SF-$count $SCRIPTS_HOME/test_read_sf.sh $count `head -n 1 $DATA_NODES`

    # concurrent appends to the same file
#    $SCRIPTS_HOME/hadoop-execrun.sh blobseer APPEND_SF-$count $SCRIPTS_HOME/test_append_sf.sh $count

    # single write to file (of size count * 64MB)
  #   $SCRIPTS_HOME/hadoop-execrun.sh hdfs WRITE_SF-$count $SCRIPTS_HOME/test_run_sw.sh $count `head -n 1 $DATA_NODES`

   # single write to file (of size count * 64MB)
    # $SCRIPTS_HOME/hadoop-execrun.sh blobseer WRITE_SF-$count $SCRIPTS_HOME/test_run_sw.sh $count `head -n 1 $DATA_NODES`

    
    # wordcount, data written locally - hdfs
    #$SCRIPTS_HOME/hadoop-execrun.sh hdfs WC_LOCAL-$count $SCRIPTS_HOME/test_wordcount.sh $count `head -n 1 $DATA_NODES`
    
    # wordcount, data written remotely - hdfs 
    #$SCRIPTS_HOME/hadoop-execrun.sh hdfs WC_REMOTE-$count $SCRIPTS_HOME/test_wordcount.sh $count `cat $HADOOP_HOME/conf/masters`

    # wordcount, blobseer
    #$SCRIPTS_HOME/hadoop-execrun.sh blobseer WC-$count $SCRIPTS_HOME/test_wordcount.sh $count `cat $HADOOP_HOME/conf/masters`
    
    if [ $count -gt 16 ]
   then 
	count=`expr $count - 16`
    else
	count=`expr $count - 1`
    fi
#	count=`expr $count + 1`
done
