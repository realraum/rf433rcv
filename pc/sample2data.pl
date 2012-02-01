#!/usr/bin/perl
use strict;
my @input= split //,<>;

my $zero=0;
my $one=0;
my $last=0;
my $timebase;
for my $current (@input)
{
  if (($current != $last) and ($current == 1) )
  {
    print (($zero>$one)?"0":"1") if $one; #skip first=sync
    $timebase = (($zero>$one)?$one:$zero);
    #print "$zero,$one\n";
    $zero=0;
    $one=0;
  }
  if ($current)
  {
    $one++ 
  } else { 
    $zero++ 
  }
  $last=$current;
}
#print "\n";
warn "Base $timebase\n";
