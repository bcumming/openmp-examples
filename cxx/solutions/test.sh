#!/bin/bash
cc_string=0
for threads in 1 2 3 4 5 6 7 8
do
    echo ========================================
    echo "          $threads threads"
    echo ========================================
    if [ "$threads" != "1" ]
    then
        lim=$[ 2*$[$threads - 1] ]
        cc_string="$cc_string,$lim"
    fi
    cc_string=none
    export OMP_NUM_THREADS=$threads
    echo aprun -cc="$cc_string" ./a.out
    aprun -cc="$cc_string" ./a.out
done

