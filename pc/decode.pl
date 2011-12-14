#!/usr/bin/perl
use strict;
my @data =  split //,<>;

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
  print STDERR "$i:\n";
  my $stat = $statistics[$i];
  for my $key (sort {$a <=> $b} keys %$stat)
  {
    if ($stat->{$key+1})
    {
      if ($stat->{$key+1}>$stat->{$key})
      {
        $stat->{$key+1}+=$stat->{$key};
        delete $stat->{$key};
      } else {
        $stat->{$key}+=$stat->{$key+1};
        delete $stat->{$key+1};
      }
    }
    print STDERR "$key $stat->{$key}\n";
  }
}
my $seq;
if (defined $ARGV[0])
{
  my ($state)=$ARGV[0];
  my ($time) = reverse sort {$a <=> $b}  keys %{$statistics[$state]} ;
  my $start = 0;
  my $counter=0;
  for my $data (@lengths)
  {
    if ($data->[0]==$state && $data->[1]==$time)
    {
      if ($start)
      { 
        print $counter-2."\n";
        last;
      } else {
        $start =1;
        print $counter-1." ";
      }
    }
    $counter+=$data->[1];
  }
}

