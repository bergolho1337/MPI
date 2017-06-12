// Programming Assignment 3.2
// *****************************************************
// Compile: 	mpicc -o main main.c
// Run: 		mpirun -n <number of processors> main
// *****************************************************

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
	ulli hits;
	double start, finish, elapsed;

	GET_TIME(start);
	hits = Calc_PI(num_tosses);
	GET_TIME(finish);
	elapsed = finish - start;	

	printf("Total hits = %llu\n", hits);
	printf("Pi estimate = %.10lf\n", 4*hits / (double)num_tosses);
	printf("Time elapsed: %.10lf (s)\n", elapsed);

	return 0;
}
