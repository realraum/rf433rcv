#!/usr/bin/perl
use strict;

my %remotes =
(
  schwarz => [\&schwarz, "01110101a1b1c1000000dddd"] ,
  weiss => [\&weiss,"0c0d01010b0001010001010a"],
  pollin => [\&pollin,"010101010100010101010001"],
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
  my ($swicht_cnt, $on) = @_ or die "parameters swicht_cnt {0|1}";
  my $data = {a=>0, b=> 0, c=> 0, dddd=> ($on? "1100":"0011")};
  $data->{chr(ord('a')+$swicht_cnt-1)}=1;
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

}
