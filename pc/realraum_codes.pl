#!/usr/bin/perl
use strict;

my %remotes =
(
  schwarz => [\&schwarz, "01110101a1b1c1000000dddd"] ,
  weiss => \&weiss,
  pollin => \&pollin,
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

}

sub pollin
{

}
