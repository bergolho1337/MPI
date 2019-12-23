#!/bin/bash

# User-spacified variables and constants
NUMBER_OF_PROCESSES=12
HOSTFILE_PATH=my_hosts
PROGRAM_PATH=./bin/hello

# Run the command
mpirun --hostfile $HOSTFILE_PATH -np $NUMBER_OF_PROCESSES $PROGRAM_PATH
