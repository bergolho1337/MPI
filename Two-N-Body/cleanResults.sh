#!/bin/bash

function clean {
    echo "----------------------------------------------------"
    echo "[!] Cleaning $1 ..."
    cd $1; make clean; cd ..
    echo "----------------------------------------------------"
}

# MAIN SCRIPT
PARALLEL_BASIC="Basic"
#PARALLEL_REDUCE="Reduce"

clean $PARALLEL_BASIC
#clean $PARALLEL_REDUCE