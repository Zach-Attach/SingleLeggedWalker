#!/bin/bash

outputFile="../../out/2/best.txt"
inputPath="../../out/2/evol/"
rm -f $outputFile  # Remove the output file if it exists
for i in $(seq 1 242); do
    for j in $(seq 0 $i); do
        bestValue=0
        bestTimestamp=""
  
        # Find all files matching the pattern for the current number
        for file in "${inputPath}*_${i}_${j}.dat"; do
            while read -r line; do
                # Extract the second number from the line
                value=$(echo $line | cut -d ' ' -f 2)
  
                # Check if this value is greater than the current best
                if (( $(echo "$value > $bestValue" | bc -l) )); then
                    bestValue=$value
                    bestTimestamp=$(basename $file | cut -d '_' -f 1)
                fi
            done < $file
        done
  
        # Write the best result for the current number to the output file
        echo "$i $j $bestTimestamp $bestValue" >> $outputFile
    done
done