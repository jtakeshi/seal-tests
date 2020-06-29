#!/bin/bash

#$ -M jtakeshi@nd.edu
#$ -m abe
#$ -q *@@jung
#$ -pe smp 1
#$ -N seal-tests
#

./prof_batch_time add
./prof_batch_time sub
./prof_batch_time mult
./prof_batch_time encrypt
./prof_batch_time decrypt

#Might as well have the CRC do the stats work as well
python stats.py ./results/time/SAC/*/*.csv > ./crc_report.txt

./prof_batch_time cachetest
./prof_batch_time cachetest_baseline



