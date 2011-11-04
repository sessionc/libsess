#!/usr/bin/perl
#
# Perl script to start Session C programs in parallel.

use strict;
use warnings;

use Cwd;

my $conf_file = q{connection.conf};
my $host_count = 0;
my $conn_count = 0;

my $pwd = getcwd;

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

my ($role, $host, $executable_name);
for my $host_idx (1 .. $host_count) {
  if (<$conf> =~ /([a-zA-Z0-9_-]+)\s+([a-zA-Z\.-]+)/) {
    ($role, $host) = ($1, $2);
    $executable_name = lc $role;

    system("ssh $host 'cd $pwd; ./$executable_name -c $conf_file' &")
  }
}
for my $conn_idx (1 .. $conn_count) {
}
close $conf;

0;
