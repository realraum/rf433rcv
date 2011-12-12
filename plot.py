#!/usr/bin/python

import fileinput
import numpy
import pylab

for line in fileinput.input():
  line = line[0:-1]
  data = numpy.array(line.split(","))
  pylab.plot(range(len(data)),data)
  pylab.show()

