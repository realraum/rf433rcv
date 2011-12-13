#!/usr/bin/perl
use strict;
my @data =  split /,/,<>;

my @statistics;

my $start = $data[0];
my $current = $start;
my @lengths;
my $count=0;
for my $input (@data)
{
  $count++;
  if ($input != $current)
  {
    push @lengths,[$current,$count];
    $statistics[$current]{$count}++;
    $count=0;
    $current=$input;
  }
}

for my $i (0..1)
{
  print "$i:\n";
  my $stat = $statistics[$i];
  for my $key (sort {$a <=> $b} keys %$stat)
  {
    print "$key $stat->{$key}\n";
  }
}
my $seq;
if (my $delemiter = $ARGV[0])
{
  print "Starting decoding: $delemiter\n";
  my ($state,$time)=split /,/,$delemiter;
  my $start = 0;
  for my $data (@lengths)
  {
    if ($start)
    {
      print $data->[0] . " => " . $data->[1] ."\n";
      $seq.= ($data->[0].',') x $data->[1];
    }
    if ($data->[0]==$state && $data->[1]==$time)
    {
      if ($start)
      { last;
      } else {
        $start =1;
      }
    }
  }
  chop $seq;
  # print "$seq\n";
  my $bitcount=0;
  my @binarydata;
  for my $bits (reverse split /,/,$seq)
  {
    $binarydata[int($bitcount/8)]<<=1;
    $binarydata[int($bitcount/8)]|=$bits;
    $bitcount++;
  }
  $binarydata[int($bitcount/8)]<<= 8 - ($bitcount % 8) if $bitcount % 8;
  my @chars = map { chr($_) } @binarydata; 
  print STDERR join '',@chars;
#  print STDERR "\ns\x04"; # send 4 times
#  print STDERR chr(8-($bitcount % 8)) if $bitcount % 8;
  print "$bitcount total, offset ".($bitcount % 8)." bits\n";
  print ($bitcount/8 ," bytes (must be <= 63)\n");
}

