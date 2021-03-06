#!/usr/bin/perl
use strict;

my %remotes =
(
  schwarz => [\&schwarz, "swia1b1c1000000dddd"] ,
  weiss => [\&weiss,"0c0d01010b0001010001010a"],
  pollin => [\&pollin,"0q0w0e0r0tbbbbbbbbbbaaaa"],
  rsl336t => [\&rsl336t,"switchbutton010101oo"],
);

&main();
exit 0;

sub main
{
  my $remote = shift @ARGV;
  if ($remotes{$remote})
  {
    my $data = $remotes{$remote}[0]->(@ARGV);
    my $base = $remotes{$remote}[1];
    for my $key (keys %$data)
    {
      $base =~ s/$key/$data->{$key}/g;
    }
    print $base;
  } else {
    die "Usage $0 {". join(',',keys %remotes) ."} remote parameters";
  }
}

sub schwarz
{
  my ($switch, $button, $on) = @_ or die "parameters {a|b|c|d} {1|2|3} {0|1}";
  my $switchdata = {
    a => '11010101',
    b => '01110101',
    c => '01011101',
    d => '01010111',
  };
  my $data = {
    swi=> $switchdata->{$switch},
    a=> 0, 
    b=> 0, 
    c=> 0, 
    dddd=> ($on? "1100":"0011")};
  $data->{chr(ord('a')+$button-1)}=1;
  return $data;
}

sub weiss
{
  my ($switch,$taste,$on) = @_ or die "parameters {a|b|c|d} {1|2} {0|1}";
  my $data = {
              a=> ($on?'1':'0'),
              b=> ($taste==1?'0':'1'),
              c=>0,
              d=>0,
              }; 
  if($switch eq 'b')
  {
    $data->{c}=1;
  } elsif ($switch eq 'c') {
    $data->{d}=1;
  } elsif ($switch eq 'd') {
    $data->{c}=1;
    $data->{d}=1;
  }
  return $data;
}

sub pollin
{
  my ($switch,$taste,$on) = @_ or die "parameters 00000 {a|b|c|d} {0|1}";
  my %tasten = (
    a => '0001010101',
    b => '0100010101',
    c => '0101000101',
    d => '0101010001',
    e => '0101010100',
  );
  my $data = {
              aaaa=> ($on?'0001':'0100'),
              bbbbbbbbbb=> $tasten{$taste},
              }; 
  my @switches = split //,$switch;
  @switches =  map { $_?'0':'1'} @switches;
  @{$data}{qw/q w e r t/}=@switches;
#  print join ',',%$data;
  return $data;
}

sub rsl336t
{
  my ($switch,$button,$on) = @_ or die "parameters {a|b|c|d} {a|b|c|d} {0|1}";
  my %tasten = (
    a => '00010101',
    b => '01000101',
    c => '01010001',
    d => '01010100',
  );
  my $data = {
              oo=> ($on?'01':'00'),
              switch=> $tasten{$switch},
              button=> $tasten{$button},
              }; 
  return $data;
}
