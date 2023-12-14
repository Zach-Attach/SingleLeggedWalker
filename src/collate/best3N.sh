#!/bin/bash

outputFile="../../out/3/best.txt"
inputPath="../../out/3/evol/"
rm -f $outputFile  # Remove the output file if it exists
for n1 in $(seq 1 242); do
    for n2 in $(seq 1 $n1); do
        for n3 in $(seq 1 $n2); do
            bestValue=0
            bestTimestamp=""
    
            # Find all files matching the pattern for the current number
            for file in ${inputPath}*_${n1}_${n2}_${n3}.dat; do
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
            echo "$n1 $n2 $n3 $bestTimestamp $bestValue" >> $outputFile
        done
    done
done