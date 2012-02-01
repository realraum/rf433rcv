#!/usr/bin/perl
use strict;
my $base=shift @ARGV;

my @input= split //,<>;

print 1 x $base;
print 0 x (31 * $base);

for my $current (@input)
{
  if ($current)
  {
    print 1 x (3*$base);
    print 0 x $base;
  } else {
    print 1 x $base;
    print 0 x (3*$base);
  }
}
