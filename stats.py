#!/usr/bin/python
from __future__ import print_function
import numpy as np
import sys

infile = sys.argv[1]
nums = [float(i) for i in open(infile, 'r').read().strip().split()]
#Set size
print("Size: " + str(len(nums)))
#Set avg
print("Avg: " + str(np.average(nums)))
#Set stddev
print("Stdev: " + str(np.std(nums)))
#Set sum
print("Sum: " + str(np.sum(nums)))
