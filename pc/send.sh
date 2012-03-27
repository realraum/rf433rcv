#!/bin/sh
cat $1 | ./compress | ./rawhid_test -
