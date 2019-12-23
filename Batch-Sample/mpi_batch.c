/*
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Author: Lucas Berg

Program that demonstrate how to use MPI to perform a distributed-batch simulation. 

Particularly, the current problem evaluates several types of linear functions by varying the coeffients 'a' and 'b' (General form: f(x) = a*x + b), where
the evaluation and the writing of the outputs files are done in parallel. Each process is responsible for a set of simulations, which are evenly divided among
the prescribed cores. If the division is not even, the last process will run any remaining simulation. 
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
COMPILATION:
    Just run the script for compilation, which builds the binary file using CMake.

    $ ./recompile_project.sh
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
EXECUTE:
    
    $ mpiexec -n <num_process> ./bin/mpi_batch
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
PLOT RESULTS:
    $ cd scripts; python plot_functions.py; cd ..
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define PRINT_LINE "==================================================================================================================================="

const int TOTAL_NUMBER_SIMULATIONS = 16;        // Total number of simulations
const double MAX_X_AXIS = 1.0;                  // Limit of the x-axis
const double DX = 0.01;                         // Size of the discretization

struct simulation_data
{
    int id;                     // Simulation index
    double a;                   // Coefficient 'a' of the linear function
    double b;                   // Coefficient 'b' of the linear function
};

double f (const double x, const double a, const double b) { return (a*x + b); }  // Linear function

struct simulation_data* generate_all_simulations (const int total_number_simulations)
{
    struct simulation_data *result = (struct simulation_data*)malloc(sizeof(struct simulation_data)*total_number_simulations);

    for (int i = 0; i < total_number_simulations; i++)
    {
        result[i].id = i;

        // Linear function form: f(x) = 1.0 * x + (i+1) --> Shift over the y-axis
        result[i].a = 1.0;
        result[i].b = (double)(i+1);
    }

    return result;
}

void solve_simulation (struct simulation_data s, const int rank, const int num_max_proc)
{
    FILE *file;
    char filename[100];
    
    int np = nearbyint(MAX_X_AXIS/DX);
    sprintf(filename,"outputs/simulation-%d.dat",s.id);
    file = fopen(filename,"w+");

    // DEBUG
    printf("[DEBUG] Hello I am process %d of %d ---- Solving linear function: f(x) = %g.x + %g (Simulation %d)\n",rank,num_max_proc,s.a,s.b,s.id);

    // Calculate the image of the function
    for (int i = 0; i < np; i++)
    {
        double x = i*DX;

        // Call the linear function using the parameters from the current simulation
        double y = f(x,s.a,s.b);

        // Write the output into a file
        fprintf(file,"%g %g\n",x,y);
    }
    fclose(file);
}

void print_all_simulations (struct simulation_data *the_simulations, const int total_number_simulations)
{
    for (int i = 0; i < total_number_simulations; i++)
    {
        printf("%s\n",PRINT_LINE);
        printf(">> SIMULATION %d\n",the_simulations[i].id);
        printf("Parameters:\n");
        printf(">>>> a = %g\n",the_simulations[i].a);
        printf(">>>> b = %g\n",the_simulations[i].b);
        printf("%s\n",PRINT_LINE);
    }
}

int main(int argc, char** argv) 
{
    int rank, num_proccess, num_max_proc;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proccess);


    // Generate all the simulations
    int total_number_simulations = TOTAL_NUMBER_SIMULATIONS;
    
    struct simulation_data *the_simulations = generate_all_simulations(total_number_simulations);
    if (rank == 0)
        print_all_simulations(the_simulations,total_number_simulations);

    // Dividing the simulations to each process
    int num_simulations_per_process, last_rank_extra_simulations;
    if (num_proccess > total_number_simulations)
        num_max_proc = total_number_simulations;
    else
        num_max_proc = num_proccess;
    
    if (rank < num_max_proc)
        num_simulations_per_process = total_number_simulations / num_max_proc;

    last_rank_extra_simulations = total_number_simulations % num_max_proc;

    // Start and end simulation number of each process
    int simulation_start_number = rank * num_simulations_per_process;
    int simulation_end_number = simulation_start_number + num_simulations_per_process;

    // The last process will run any remaining simulation
    if (rank == num_max_proc-1)
        num_simulations_per_process += last_rank_extra_simulations;

    // Sync everyone from here ...
    MPI_Barrier(MPI_COMM_WORLD);
    for (int s = simulation_start_number; s < simulation_end_number; s++)   // Simulation loop
    {
        solve_simulation(the_simulations[s],rank,num_max_proc);
    }

    free(the_simulations);      // Free memory
    MPI_Finalize();         

    return EXIT_SUCCESS;

}

