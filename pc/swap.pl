#!/usr/bin/perl

while(<>)
{
  s/1/x/g;
  s/0/1/g;
  s/x/0/g;
  print;
}
