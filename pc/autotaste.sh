#!/bin/sh

FN=`mktemp`
FN2=`mktemp`
FN3=`mktemp`
echo $FN >&2
echo $FN2 >&2 
echo $FN3 >&2
./rawhid_test > $FN
./decode.pl $FN $1 | xargs ./decode2.pl $FN > $FN2
if [ $1 -eq 1 ]; then
  ./swap.pl < $FN2 >$FN3
else   
  cat $FN2 > $FN3
fi
./plot.py < $FN3
cat $FN3
