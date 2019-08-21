#!/bin/bash
#Arguments are: executable powerstat_outfile time_outfile
#Assumes calc is installed, to do float division for total number of samples
MAXARG=128
#Time to run powerstat for (seconds)
PSTAT_TIME=500
#Measurements must be at least 0.5 sec apart
PSTAT_GRANULARITY=0.5
PSTAT_SAMPLES=$(calc $PSTAT_TIME / $PSTAT_GRANULARITY)
#Check that number of powerstat samples is at least 960
if [ $PSTAT_SAMPLES -lt 960 ]
  then echo "ERROR: need at least 960 samples, have $PSTAT_SAMPLES"
  exit
fi 
#How to run program: -t for time mode, -n for number of iterations
EXEC_MODE=-n
EXEC_ARG=5
#Check if root - can get extra information from powerstat
: '
if [ "$EUID" -ne 0 ]
  then echo "ERROR: need to be root to run"
  exit
fi  
'
#Get two random numbers to argue to the program
A=$RANDOM%$MAXARG
B=$RANDOM%$MAXARG
#Fork off powerstat with 0 delay
powerstat -d 0 -a $PSTAT_GRANULARITY $PSTAT_SAMPLES > $2 2>&1 &
#Run program
$1 $EXEC_MODE $EXEC_ARG $A $B > $3
#Kill powerstat
kill $!
