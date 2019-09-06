#!/bin/bash
#First argument is executable name - NAME ONLY, NOT PATH
#Loop over iterations
#Iterations range from 2^14 to 2^23 - 9 measurements
MIN_ITERATIONS=10000
MAX_ITERATIONS=10000
MIN_CIPHERTEXTS=16
MAX_CIPHERTEXTS=8192
for((i=$MIN_ITERATIONS;i<=$MAX_ITERATIONS;i*=2));
do
  echo "Iterations: ${i}"
  for((c=$MIN_CIPHERTEXTS;c<=$MAX_CIPHERTEXTS;c*=2));
  do
    echo -e "\tCiphertexts: ${c}"
    ./profile.sh ./${1} ./results/power/${1}/${1}_i${i}_c${c}.txt ./results/time/${1}/${1}_i${i}_c${c}.csv $i $c
  done
done
