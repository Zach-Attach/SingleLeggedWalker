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
  for j in {1..121}
  do
    inner $j
  done
  for j in {121..242}
  do
    inner $j
  done
}

for i in {1..100}
do
  singleN
done

for i in {1..100}
do
  doubleN
done
