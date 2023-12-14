#!/bin/bash

max_jobs=128  # Maximum number of parallel jobs
count=0     # Counter for current number of running jobs

inner(){
  for i in {${1}..${2}}; do
    ./main2 $i & 
    ((count++))
    
    while [ $count -ge $max_jobs ]; do
      sleep 1  # Wait a bit before checking again
      # Update count with the number of jobs still running
      count=$(jobs -p | wc -l)
    done
  done
}

doubleN(){
  for i in {${1}..${2}}; do
    for j in {0..${j}}; do
      ./main2 $i $j & 
      ((count++))
      
      while [ $count -ge $max_jobs ]; do
        sleep 1  # Wait a bit before checking again
        # Update count with the number of jobs still running
        count=$(jobs -p | wc -l)
      done
    done
  done
}

tripleN(){
  for i in {${1}..${2}}; do
    for j in {0..${i}}; do
      for k in {0..${j}}; do
        ./main2 $i $j $k & 
        ((count++))
        
        while [ $count -ge $max_jobs ]; do
          sleep 1  # Wait a bit before checking again
          # Update count with the number of jobs still running
          count=$(jobs -p | wc -l)
        done
      done
    done
  done
  wait
}

if [ $3 -eq 3 ]; then
  tripleN
elif [ $3 -eq 2 ]; then
  doubleN
else
  inner
fi