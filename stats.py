#!/usr/bin/python
from __future__ import print_function
import numpy as np
import sys

infile = sys.argv[1]
nums = [float(i) for i in open(infile, 'r').read().strip().split()]
#Set size
print(str(len(nums)))
#Set avg
print(str(np.average(nums)))
#Set stddev
print(str(np.std(nums)))
