#!/bin/bash
cc_string=0
for threads in 1 2 3 4 5 6 7 8
do
    echo ========================================
    echo "          $threads threads"
    echo ========================================
    cc_string=none
    export OMP_NUM_THREADS=$threads
    aprun -cc="$cc_string" ./a.out
done

