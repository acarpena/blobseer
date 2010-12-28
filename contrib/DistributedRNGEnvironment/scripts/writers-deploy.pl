#!/usr/bin/perl -w
use File::Path;
use File::Basename;
use Getopt::Long;

# global settings
$WORKING_DIR = $ENV{'PWD'};
$LOGIN_NAME = $ENV{'LOGNAME'};
$HOME_DIR = $ENV{'HOME'};

# run configuration 
$BLOBSEER_HOME = "$HOME_DIR/blobseer-0.3";
$TEST_RUN = "$BLOBSEER_HOME/test/multiple_writers";
$DEPLOY_SCRIPT = "$HOME_DIR/scripts/templates/deploy-process.sh";
$CFG_FILE = "$BLOBSEER_HOME/test/blobseer.cfg";

# additional parameters
# ---------------------
# the chunk size, 2^26 = 64MB
$CHUNK_SIZE = 26;
# the blob id created with create_blob and the number of passes
$ID = 1;
$PASSES = 50;

############################################# HELPER FUNCTIONS #####################################

# get the host list from the reservation
sub get_hosts {
    my @clusters = split(/,/, $_[0]);
    my $no_clusters = @clusters;
    open(OARSTAT_PIPE, $host_list);
    my @hosts = grep(/(\w|\.)+/, <OARSTAT_PIPE>);
    close(OARSTAT_PIPE);

    map($_ =~ s/\n//, @hosts);
    my @new_hosts = ();
    if ($no_clusters > 0) {
	foreach(@clusters) {
	    $cluster_name = $_;
	    push(@new_hosts, grep(/$cluster_name/, @hosts));
	}
    } else {
	@new_hosts = @hosts;
    }
    return @new_hosts; 
}

#############################################  DIRECT DEPLOYMENT  ##############################################

sub deploy_process {
    my $hostname = $_[0];
    my($filename, $pathname, $suffix) = fileparse($_[1]);
    my $cmdname = $filename.$suffix;
    my $args = "\'$_[2]\'";
    my $dest_dir = $_[3];
    
    `oarsh $hostname \"env CLASSPATH=$ENV{'CLASSPATH'} LD_LIBRARY_PATH=$ENV{'LD_LIBRARY_PATH'} $DEPLOY_SCRIPT $pathname $cmdname $args $dest_dir\" >/dev/null`;
    # Testing
    # `ssh $hostname \"$DEPLOY_SCRIPT $dirname $basename $args $dest_dir\" >/dev/null`;
    $? == 0 || print "WARNING: command failed: oarsh $hostname \"$DEPLOY_SCRIPT $pathname $cmdname $args $dest_dir\" >/dev/null";

}

sub recover_logs {
    my $job_id = $_[0];
    my @hosts = @{$_[1]};
    my $no_hosts = @hosts;
    my($filename, $pathname, $suffix) = fileparse($_[2]);
    my $cmdname = $filename.$suffix;
    `echo '# Merged output of $cmdname' > $cmdname.log`;
    for (my $i = 0; $i < $no_hosts; $i++) {
	my $hostname = $hosts[$i];
	print "Now recovering logs from host $hosts[$i] ($i/$no_hosts)...";
	for (my $j = 0; $j < $no_instances; $j++) {
	    `oarcp $hostname:/tmp/$LOGIN_NAME/$job_id/test$i/$cmdname.std* . >/dev/null`;
	    $? == 0 || print "WARNING: could not retrieve log files for instance $i\n";
	    `echo '# STDOUT of $hostname, instance $j' >> $cmdname.log`;
	    `cat $cmdname.stdout >> $cmdname.log`;
	    `echo '# STDERR of $hostname, instance $j' >> $cmdname.log`;
	    `cat $cmdname.stderr >> $cmdname.log`;
	}
	print "OK!\n";
    }
    `rm -f $cmdname.std*`;
}

sub deploy_manually {
    my $job_id = $_[0];
    my @hosts = @{$_[1]};
    my $no_hosts = @hosts; 
    for (my $i = 0; $i < $no_hosts; $i++) {
	print "Now processing host $hosts[$i] ($i/$no_hosts)...";
	# print "args: /tmp/general.cfg $HOME_DIR/barrier $i $CHUNK_SIZE $ID $PASSES...";
	for (my $j = 0; $j < $no_instances; $j++) {
	    my $off = ($i * $no_instances) + $j;
	    deploy_process($hosts[$i], $TEST_RUN, "$CFG_FILE $HOME_DIR/barrier $i $CHUNK_SIZE $ID $PASSES", "/tmp/$LOGIN_NAME/$job_id/test$i");
	}
	print "success\n";
    }
}

sub kill_manually {
    my @hosts = @{$_[0]};
    my $no_hosts = @hosts;
    for(my $i = 0; $i < $no_hosts; $i++) {
	print "Now processing host $hosts[$i] ($i/$no_hosts)...";
	`oarsh $hosts[$i] \"killall -u $LOGIN_NAME\"`;
	if ($? == 0) { print "OK!\n"; } else { print "no owned processes\n"; }
    }
}

sub getstatus_manually {
    my @hosts = @{$_[0]};
    my $no_hosts = @hosts;
    for(my $i = 0; $i < $no_hosts; $i++) {
	print "Processes running on $hosts[$i] ($i/$no_hosts)...\n";
	my $pscmd = "ps aux | grep multiple_writers | grep -v grep";
	print `oarsh $hosts[$i] \"$pscmd\"`;
    }
}

#################################################  MAIN PROGRAM  ###############################################

# get params 
$usage = "Usage: writers-deploy.pl -hostlist <host_list_file> [-kill | -status | -logs] [-clusters <cluster_list>] [-instances <default=1>]";
$job_id = 0;
$clusters = '';
$no_instances = 1;
GetOptions('hostlist=s' => \$host_list, 
           'kill' => \$kill_flag, 
	   'status' => \$check_flag,
	   'logs' => \$log_flag,
	   'clusters=s' => \$clusters,
	   'instances=i' => \$no_instances
	  ) || die $usage;
#if ($job_id == 0) { die $usage; }
#$ENV{'OAR_JOB_KEY_FILE'} = "$HOME_DIR/keys/oargrid_ssh_key_".$LOGIN_NAME."_$job_id";

# Get hosts
@hosts = get_hosts($clusters);
#@hosts = ("paramount1", "paramount2", "paramount3");
if (@hosts == 0) { die "Could not parse the reservation host list\nMake sure job ID is valid & you are running on frontend"; }
$no_hosts = @hosts;
printf "oar2 reports $no_hosts reserved nodes for $job_id\n";
if ($no_hosts < 1) { die "FATAL: Must reserve at least 1 nodes"; } 

if ($kill_flag) {
    kill_manually(\@hosts);
    exit(0);
}
if ($check_flag) {
    getstatus_manually(\@hosts);
    exit(0);
}

if ($log_flag) {
    recover_logs($job_id, \@hosts, $TEST_RUN);
    exit(0); 
}
# Deployment
print "Starting deployment...\n";
deploy_manually($job_id, \@hosts);
print "Deployment suceessful\n";
