/*
------------------------------------------------------------------------------------------------
    Basic Parallel Version of the Two-N-Body algorithm
        Autor: Lucas Berg

- Specify the number of process on the command line

- It is possible to build an input with executing the Python script (makeInput.py)

- After an input has been made, you can execute the program by passing the generated file as:

    $ mpiexec -n <num_procs> ./twoBody < "name_input_file"

    "name_input_file" = ../Input/in10
------------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/twobody.h"
#include "../include/timer.h"

int main (int argc, char *argv[])
{
    double start, finish, elapsed;
    
    buildModel(argc,argv);

    GET_TIME(start);
    //solveModel(model);
    GET_TIME(finish);
    elapsed = finish - start;
    printf("Elapsed time = %.10lf\n",elapsed);

    MPI_Finalize();

    return 0;
}