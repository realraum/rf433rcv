#!/usr/bin/env python

import fileinput
import numpy
import pylab
for datastr in fileinput.input():
  data = numpy.array(datastr.split(","))
  pylab.plot(range(len(data)),data)

          
pylab.show()
