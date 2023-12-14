#!/bin/bash
make -f Makefile

singleN(){
  for j in {1..121}
  do
    ./main $j &
  done
  wait
  for j in {121..242}
  do
    ./main $j &
  done
  wait
}

inner(){
  for j in {1..121}
  do
    ./main $1 $j &
  done
  wait
  for j in {121..242}
  do
    ./main $1 $j &
  done
  wait
}

doubleN(){
  for i in {1..121}
  do
    inner $i
  done
  for i in {121..242}
  do
    inner $i
  done
}

for i in {1..10}
do
  singleN
done
