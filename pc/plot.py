#!/usr/bin/env python

import fileinput
import numpy
import pylab
for datastr in fileinput.input():
  data=numpy.array(list(datastr))
  pylab.plot(range(len(datastr)),data)
  pylab.axis([0, 2000,-0.5, 1.5])
          
pylab.show()
