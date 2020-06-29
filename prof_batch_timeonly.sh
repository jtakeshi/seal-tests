#!/bin/bash
#First argument is executable name - NAME ONLY, NOT PATH
#Second arg is iterations
#MIN_ITERATIONS=10000
#MAX_ITERATIONS=10000
#MIN_CIPHERTEXTS=16
#MAX_CIPHERTEXTS=8192

#Change this to 1000 for mult. and 200000 for others
if [ -z $2 ]
then
  ITERATIONS=50000
else
  ITERATIONS=$2
fi

if [ $1 = "mult" ]
then
  ITERATIONS=1000
fi
#IFS=' '
#PARAMS_LIST=("8192 128" "16384 128" "32768 128" "8192 192" "16384 256")
PARAMS_LIST=("16384 128")
EXT=baseline
TRIAL=SAC

for i in "${PARAMS_LIST[@]}"
do
  IFS=' ' read -ra SPLIT_ARG <<< "$i"
  #SPLIT_ARG[0] is poly. degree, SPLIT_ARG[1] is security level
  #No flags when passing, as profile.sh handles adding flags
  ./${1} > ./results/time/${TRIAL}/${1}/${SPLIT_ARG[0]}_${SPLIT_ARG[1]}.csv 2>&1 -n $ITERATIONS -d ${SPLIT_ARG[0]} -s ${SPLIT_ARG[1]}
  #sleep 5
  #Send output to nowhere - we don't care about time for baseline
  #Don't do second test, as powerstat doesn't like it.
  #./profile.sh ./${1}_${EXT} ./results/power/sweep/${1}/${SPLIT_ARG[0]}_${SPLIT_ARG[1]}_${EXT}.txt ./results/time/sweep/${1}/${SPLIT_ARG[0]}_${SPLIT_ARG[1]}_${EXT}.txt $ITERATIONS $i
done

: '
for((i=$MIN_ITERATIONS;i<=$MAX_ITERATIONS;i*=2));
do
  echo "Iterations: ${i}"
  for((c=$MIN_CIPHERTEXTS;c<=$MAX_CIPHERTEXTS;c*=2));
  do
    echo -e "\tCiphertexts: ${c}"
    ./profile.sh ./${1} ./results/power/${1}/${1}_i${i}_c${c}.txt ./results/time/${1}/${1}_i${i}_c${c}.csv $i $c
  done
done
'
