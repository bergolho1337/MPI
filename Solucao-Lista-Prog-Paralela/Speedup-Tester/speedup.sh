#!/bin/bash

function RunSimulation {
    echo "Number of processes $1"
    echo "---------------------------------------------------------"
    echo "[!] Running with [n = 1048576 ] ..."
    for i in $(seq 1 5); do
        echo "Run $i ..."
        mpiexec -n $1 ./$2 < ./Inputs/input-1048576 >> ./Outputs/times
    done
    ./average.sh ./Outputs/times >> ./Outputs/output1048576
    rm ./Outputs/times
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with [n = 2097152 ] ..."
    for i in $(seq 1 5); do
        echo "Run $i ..."
        mpiexec -n $1 ./$2 < ./Inputs/input-2097152 >> ./Outputs/times
    done
    ./average.sh ./Outputs/times >> ./Outputs/output2097152
    rm ./Outputs/times
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with [n = 4194311 ] ..."
    for i in $(seq 1 5); do
        echo "Run $i ..."
        mpiexec -n $1 ./$2 < ./Inputs/input-4194311 >> ./Outputs/times
    done
    ./average.sh ./Outputs/times >> ./Outputs/output4194311
    rm ./Outputs/times
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with [n = 8388608 ] ..."
    for i in $(seq 1 5); do
        echo "Run $i ..."
        mpiexec -n $1 ./$2 < ./Inputs/input-8388608 >> ./Outputs/times
    done
    ./average.sh ./Outputs/times >> ./Outputs/output8388608
    rm ./Outputs/times
    echo "---------------------------------------------------------"
}


# ================== MAIN SCRIPT ===================================
EXECUTABLE="trap_time"
THREADS=( 1 2 4 8 )

# Run the simulations with given the executable
echo "========================================================================="
echo "[!] Running speedup simulation for the executable [$EXECUTABLE]"
if [ ! -f $EXECUTABLE ]; then
    make
else
    make clean; make
fi

for THREAD in ${THREADS[@]}; do
    echo "=========================================================="
    echo "$THREAD" >> ./Outputs/output
    RunSimulation $THREAD $EXECUTABLE $NUMRUNS
    echo "=========================================================="
done

CMD=""
FOLDERS=$(ls ./Outputs | sort)
for FOLDER in ${FOLDERS[@]}; do
    CMD=$CMD$FOLDER" "
done
cd ./Outputs
paste $CMD > ../Plot/times.dat
cd ..

make plot