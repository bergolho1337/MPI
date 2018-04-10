// Programming Assignment 3.2
// **********************************************************
// Compile: 	make
// Run: 		mpirun -n <number of processors> ./parallel
// **********************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "timer.h"

typedef unsigned long long int ulli;

const int num_tosses = 1024 * 1024 * 1024;

int inCircle (double x, double y)
{
	return (sqrt(x*x + y*y) <= 1.0);
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

ulli Calc_PI (ulli tosses)
{
    ulli hits;
    ulli i;
    double x, y;
    for (i = 0, hits = 0; i < tosses; i++)
	{
		// Make the shoot
		x = makeShoot();
		y = makeShoot();
		// Check if hits the circle
		if (inCircle(x,y))
			hits++;	 
	}
    return hits;
}

int main (int argc, char *argv[])
{
	int my_rank, comm_sz;
	ulli local_tosses;
	ulli local_hits, total_hits;
	double start, finish, local_elapsed, elapsed;
	
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    // Calculate local tosses
	local_tosses = num_tosses / (ulli)comm_sz;

    MPI_Barrier(MPI_COMM_WORLD);
	GET_TIME(start);
    local_hits = Calc_PI(local_tosses);
	GET_TIME(finish);
	local_elapsed = finish - start;
	
    sleep(my_rank*2);
    printf("Process %d >> Local hits = %llu\n", my_rank, local_hits);
	MPI_Reduce(&local_hits,&total_hits,1,MPI_UNSIGNED_LONG_LONG,MPI_SUM,0,MPI_COMM_WORLD);
	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	if (my_rank == 0)
	{
		printf("\nProcess %d >> Total hits = %llu\n", my_rank, total_hits);
		printf("Pi estimate = %.10lf\n", 4*total_hits / (double)num_tosses);
		printf("Time elapsed: %.10lf (s)\n", elapsed);
	}
	MPI_Finalize();

    return 0;
}
