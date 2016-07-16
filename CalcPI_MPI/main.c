// Programming Assignment 3.2
// *****************************************************
// Compile: 	mpicc -o main main.c
// Run: 	mpirun -n <number of processors> main
// *****************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int inCircle (double x, double y)
{
	double norm = sqrt(x*x + y*y);
	if (norm <= 1.0)
		return (1);
	else
		return (0);
}

double makeShoot ()
{
	double r, signal;
	signal = rand() % 2;
	if (signal)
		r = rand() / (double)RAND_MAX;
	else
		r = -1*(rand() / (double)RAND_MAX);
	return (r);
}

int main (int argc, char *argv[])
{
	int my_rank, comm_sz;
	long long int num_tosses, local_tosses, i;
	long long int local_hits, total_hits;
	double x, y, signal;
	double start, finish, local_elapsed, elapsed;
	
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

	if (my_rank == 0)
	{
		// Getting number of tosses
		scanf("%lld",&num_tosses);
		// Calculate local tosses
		local_tosses = num_tosses/(long long int)comm_sz;	
	}	
	MPI_Bcast(&local_tosses,1,MPI_LONG_LONG,0,MPI_COMM_WORLD);	
	local_hits = 0;
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();
	for (i = 0; i < local_tosses; i++)
	{
		// Make the shoot
		x = makeShoot();
		y = makeShoot();
		// Check if hits the circle
		if (inCircle(x,y))
			local_hits++;	 
	}
	finish = MPI_Wtime();
	local_elapsed = finish - start;
	//printf("Process %d >> Local hits = %lld\n", my_rank, local_hits);
	MPI_Reduce(&local_hits,&total_hits,1,MPI_LONG_LONG,MPI_SUM,0,MPI_COMM_WORLD);
	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	if (my_rank == 0)
	{
		//printf("\nProcess %d >> Total hits = %lld\n", my_rank, total_hits);
		printf("Pi estimate = %e\n", 4*total_hits / (double)num_tosses);
		printf("Time elapsed: %e (s)\n", elapsed);
	}
	MPI_Finalize();
}
