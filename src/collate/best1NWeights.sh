#!/bin/bash

outputFile="../../out/1/best1.txt"
inputPath="../../out/1/best/fit/"
# rm -f $outputFile  # Remove the output file if it exists

for number in $(seq 1 242); do
    for file in ${inputPath}*_${number}.dat; do
        while read -r line; do
            echo "$number $foot $angle $line" >> $outputFile
        done < $file
    done
done
