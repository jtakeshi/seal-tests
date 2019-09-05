#!/usr/bin/python
from __future__ import print_function
import numpy as np
import sys

for infile in sys.argv[1:]:
  nums = [float(i) for i in open(infile, 'r').read().strip().split()]
  #Print filename
  print("Stats for " + infile + ":")  
  #Set size
  print("\tSize: " + str(len(nums)))
  #Set avg
  print("\tAvg: " + str(np.average(nums)))
  #Set stddev
  print("\tStdev: " + str(np.std(nums)))
  #Set sum
  print("\tSum: " + str(np.sum(nums)))
  #Print empty line
  print("\n")
