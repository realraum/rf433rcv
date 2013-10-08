#!/usr/bin/env python

import fileinput
import numpy
import pylab
for datastr in fileinput.input():
  data=numpy.array(map(int,datastr))
  pylab.plot(range(len(datastr)),data)
  pylab.hold(True)
  pylab.axis([0, len(datastr),-0.5, 3.5])
          
pylab.show()
