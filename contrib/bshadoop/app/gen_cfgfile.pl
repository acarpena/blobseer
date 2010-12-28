#!/usr/bin/perl -w

if ($#ARGV + 1 != 3) { die "Usage: ./gen-cfgfile.pl <host_file> <num_dhts> <config_file>\n" }

#$WORKING_DIR = $ENV{'PWD'};
#$TEMPLATE_DIR = "$WORKING_DIR";
$DHT_TEMPLATE_FILE = "$ENV{'SCRIPTS_HOME'}/wr_template.cfg";
$DHT_MAX_NUM = $ARGV[1];

sub replace_var {
    $_[0] =~ s/\$\{$_[1]\}/$_[2]/g || print 'WARNING: parameter ${', $_[1], '} not found', "\n";
}

sub read_template {
    my $template = "";
    open(F_IN, "<$_[0]") || die "Configuration template $_[0] missing";
    $template = join('', <F_IN>);
    close(F_IN);
    return $template;
} 

# get the host list from the reservation
sub get_hosts {
    open(OARSTAT_PIPE, $_[0]);   

    my @hosts = grep(/(\w|\.)+/, <OARSTAT_PIPE>);
    map($_ =~ s/\n//, @hosts);

    close(OARSTAT_PIPE);    
    return @hosts;
}

sub gen_dht_config {
    my $template = read_template($DHT_TEMPLATE_FILE);
    my @gwl = @{$_[0]};
    my $no_gw = @gwl;
    my $CFG_FILE = $_[1];
    #my $temp = $DHT_MAX_NUM;
    if ($no_gw > (${DHT_MAX_NUM} + 2)) { $no_gw = (${DHT_MAX_NUM} + 2) }
    #if ($no_gw > 22) { $no_gw = 22 }
    my $gateways = "\n\t\t".join(",\n\t\t", map("\"$_\"", @gwl[2..$no_gw - 1]))."\n\t";
    #my $gateways = "\n\t\t".join(",\n\t\t", map("\"$_\"", @gwl[2..21]))."\n\t";

    replace_var($template, 'gateways', $gateways);
    replace_var($template, 'pmanager', "\"".$gwl[1]."\"");
    replace_var($template, 'vmanager', "\"".$gwl[0]."\"");

    open(TEST_OUT, ">$CFG_FILE");
    print TEST_OUT $template;
    close(TEST_OUT);
}

@hosts = get_hosts($ARGV[0]);
gen_dht_config(\@hosts, $ARGV[2]);
print "Generated configuration file successfully\n";
