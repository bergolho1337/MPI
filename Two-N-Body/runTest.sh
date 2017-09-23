#!/bin/bash

function RunSimulation {
    echo "---------------------------------------------------------"
    echo "[!] Running with 10 particles ..."
    ./twoBody < ../Input/input10
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 20 particles ..."
    ./twoBody < ../Input/input20
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 40 particles ..."
    ./twoBody < ../Input/input40
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 80 particles ..."
    ./twoBody < ../Input/input80
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 160 particles ..."
    ./twoBody < ../Input/input160
    echo "---------------------------------------------------------"
    echo "---------------------------------------------------------"
    echo "[!] Running with 320 particles ..."
    ./twoBody < ../Input/input320
    echo "---------------------------------------------------------"
    echo "=========================================================="
}


# ================== MAIN SCRIPT ===================================
BASIC="Serial-Basic"
REDUCE="Serial-Reduce"

# Run the simulations with the Basic solver first
echo "=========================================================="
echo "[!] BASIC SOLVER"
if [ ! -f $BASIC/twoBody ]; then
    cd $BASIC; make;
fi

cd $BASIC
RunSimulation
cd ..

# Run the simulations with the Reduce solver
echo "=========================================================="
echo "[!] REDUCE SOLVER"
if [ ! -f $REDUCE/twoBody ]; then
    cd $REDUCE; make;
fi

cd $REDUCE;
RunSimulation
cd ..

