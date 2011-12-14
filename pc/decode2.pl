#!/usr/bin/perl
use strict;
print STDERR $ARGV[2]-$ARGV[1], "samples\n";
my @data =  split /,/,<>;
my @samples = $ARGV[0]..$ARGV[1];
print join '',@data[@samples];
print STDERR "\n";
