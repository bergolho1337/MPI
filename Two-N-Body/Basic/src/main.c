/*
------------------------------------------------------------------------------------------------
    Basic Parallel Version of the Two-N-Body algorithm
        Autor: Lucas Berg

- Specify the number of processes on the command line

- It is possible to build an input with executing the Python script (makeInput.py)

- After an input has been made, you can execute the program by passing the generated file as:

    $ mpiexec -n <num_procs> ./twoBody < "name_input_file"
------------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include "../include/twobody.h"

int main (int argc, char *argv[])
{
    // Build the structures of the model
    Build_Model(argc,argv);

    // Solve the model
    Solve_Model();
    
    // Free memory
    Free_Model();

    return 0;
}