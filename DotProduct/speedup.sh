#!/bin/bash

if [ ! -f main4 ]; then
    mpicc -o main4 main4.c
fi

rm -f *.dat

#N=( 1048576 2097152 4194304 8388608 16777216 33554432 67108864 134217728 )
N=( 1048576 2097152 4194304 8388608 16777216 33554432 )
#N=( 10 20 30 40 50 )
PROC=( 1 2 4 )

for proc in "${PROC[@]}"; do
    for n in "${N[@]}"; do
        echo "Running with: [N = $n, PROC = $proc]"
        mpiexec -n $proc ./main4 $n >> time$n.dat
    done
    echo "======================================================="
done

CMD=""
for n in "${N[@]}"; do
    CMD=$CMD"time$n.dat "
done
#echo $CMD
paste $CMD > times.dat