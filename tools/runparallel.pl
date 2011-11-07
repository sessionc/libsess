#!/usr/bin/perl
#
# Perl script to start Session C programs in parallel.

use strict;
use warnings;

use Cwd;
use threads;

my $conf_file = q{connection.conf};
my $host_count = 0;
my $conn_count = 0;

my $pwd = getcwd;

sub ssh_thread {
    my $host = shift;
    my $pwd  = shift;
    my $role = shift;
    my $conf = shift;
    print "{ host=$host, role=$role }";
    print " command=`ssh $host 'cd $pwd; ./$role -c $conf'\n";
    system("ssh $host 'cd $pwd; ./$role -c $conf'");
}

#
# Environment.
#
print "Current working directory is $pwd\n";

#
# Read file.
#
open my $conf, '<', $conf_file || die "Unable to open file: $!";
if (<$conf> =~ /(\d+)\s+(\d+)/) {
  ($host_count, $conn_count) = ($1, $2);
}
print "Number of hosts: $host_count\n";
print "Number of connections: $conn_count\n";


print "-----------------------------------------------------------------\n";
my ($role, $host, $role_name);
my @threads;
for my $host_idx (1 .. $host_count) {
  if (<$conf> =~ /([a-zA-Z0-9_-]+)\s+([a-zA-Z\.-]+)/) {
    ($role, $host) = ($1, $2);
    $role_name = lc $role;

    push @threads, threads->create('ssh_thread', $host, $pwd, $role_name, $conf_file);
  }
}

print "-----------------------------------------------------------------\n";

foreach my $thread (@threads) {
    $thread->join();
}

close $conf;

0;
