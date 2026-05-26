#!/bin/bash

PROGRAM="./ping_pong_cpu_opt"
ITERATIONS=1000000
RUNS=100

echo "Running $PROGRAM $ITERATIONS iterations for $RUNS runs..."

for ((i=1; i<=RUNS; i++)); do
    $PROGRAM $ITERATIONS
done | LC_ALL=C awk '
BEGIN {
    min = 999999.9
    max = 0.0
    sum = 0.0
    count = 0
}
{
    
    if ($0 ~ /Avg per ping-pong:/) {
        val = $NF
        
        if (val < min) min = val
        if (val > max) max = val
        sum += val
        count++
    }
}
END {
    if (count > 0) {
        printf "\n=== LATENCY DISTRIBUTION (%d RUNS) ===\n", count
        printf "MIN:      %.6f us (%d ns)\n", min, (min * 1000)
        printf "MAX:      %.6f us (%d ns)\n", max, (max * 1000)
        printf "AVG:      %.6f us (%d ns)\n", (sum / count), ((sum / count) * 1000)
        printf "JITTER:   %.6f us (%d ns)\n", (max - min), ((max - min) * 1000)
    } else {
        print "Error: No valid metrics parsed."
    }
}'