#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

if [ $# -lt 4 ]
then 
    echo "Usage: $0 <type> <tag> <script> <clients> [<wr_node>]"
    exit 1
fi

RUNS=5

# Deploy hadoop
rm -rf $HADOOP_HOME/bench-logs
mkdir -p $HADOOP_HOME/bench-logs
#$SCRIPTS_HOME/$1-deploy.sh

# Execute the experiment
#$3 $4 $RUNS $1 $5
$3 $4 $RUNS $5

# Archive the logs
mkdir -p $HADOOP_HOME/results
#tar cjf $HADOOP_HOME/results/$1-logs-$2-`date +%y-%m-%d-%H-%M`.tar.bz2 $HADOOP_HOME/logs $HADOOP_HOME/bench-logs
#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then
    echo "Environment is not configured properly. Check env.sh and source it."
    exit 1
fi
