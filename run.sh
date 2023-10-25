#!/bin/bash
make -f Makefile

command(){
  echo "Running $1"
  ./main $1
  echo "Done $1"
}

for i in {1..50}
do
  for j in {1..250}
  do
    command $j &
  done
  wait
done
