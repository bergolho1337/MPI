#!/bin/bash

function RunSimulation {
    echo "Number of processes $1"
    echo "---------------------------------------------------------"
    echo "[!] Running with 4 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input4
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 8 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input8
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 16 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input16
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 32 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input32
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 64 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input64
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 128 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input128
    echo "---------------------------------------------------------"
    echo "[!] Running with 256 particles ..."
    mpiexec -n $1 ./twoBody < ../Input/input256
    echo "---------------------------------------------------------"
}


# ================== MAIN SCRIPT ===================================
BASIC="Basic"
REDUCE="Reduce"
THREADS=( 1 2 4 8 )

# Run the simulations with the Basic solver first
echo "=========================================================="
echo "[!] BASIC SOLVER"
if [ ! -f $BASIC/twoBody ]; then
    cd $BASIC; make; cd ..
fi

cd $BASIC
for THREAD in ${THREADS[@]}; do
    echo "=========================================================="
    RunSimulation $THREAD
    echo "=========================================================="
done 
cd ..

# Run the simulations with the Reduce solver
#echo "=========================================================="
#echo "[!] REDUCE SOLVER"
#if [ ! -f $REDUCE/twoBody ]; then
#    cd $REDUCE; make; cd ..
#fi

#cd $REDUCE
#for THREAD in ${THREADS[@]}; do
#    echo "=========================================================="
#    RunSimulation $THREAD
#    echo "=========================================================="
#done
#cd ..

