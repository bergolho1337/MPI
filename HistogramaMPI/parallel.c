/*
 * ==================================================================
 *	Parallel Version:
 *
 * Compile: make
 * Run: mpiexec -n <num_procs> ./parallel
 * <num_procs> = Number of processes
 * ==================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "../timer.h"

const int n = 1024ull * 1024ull * 256ull;
const int max_value = 256;
//const int n = 12;
//const int max_value = 4;

int* allocMemory (int size)
{
    int *buffer = (int*)calloc(size,sizeof(int));
    return buffer;
}

void generateData (int *data)
{
    int i;
    for (i = 0; i < n; i++)
        data[i] = rand() % max_value;
		//data[i] = i+1;
}

void printData (int *data, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%d\n",data[i]);
	printf("\n");
}

void writeHistogram (int *count)
{
    FILE *out = fopen("count.dat","w+");
    int i;
    for (i = 0; i < max_value; i++)
        fprintf(out,"%d %d\n",i,count[i]);
    fclose(out);
}

void processLocalHistogram (int *data, int size, int *count)
{
	int i;
	for (i = 0; i < size; i++)
		count[data[i]]++;
}

int main (int argc, char *argv[])
{
	int my_rank, comm_sz;
	int *data, *local_data, local_n;							
	int *total_count, *local_count;				
	double start, finish, local_elapsed, elapsed;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

	local_n = n / comm_sz;
	data = NULL; total_count = NULL;
	local_data = allocMemory(local_n);
	local_count = allocMemory(max_value);	 

	if (my_rank == 0)
	{
		data = allocMemory(n);
		total_count = allocMemory(max_value);
		generateData(data);
		MPI_Scatter(data,local_n,MPI_INT,local_data,local_n,MPI_INT,0,MPI_COMM_WORLD);
		free(data);
	}
	else
		MPI_Scatter(data,local_n,MPI_INT,local_data,local_n,MPI_INT,0,MPI_COMM_WORLD);
	
	GET_TIME(start);
	processLocalHistogram(local_data,local_n,local_count);
	MPI_Reduce(local_count,total_count,max_value,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	GET_TIME(finish);
	local_elapsed = finish - start;

	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	if (my_rank == 0)
	{
		printf("With n = %d\n",n);
		printf("Time elapsed = %.10lf\n",elapsed);
		writeHistogram(total_count);
		free(total_count);
	}
		
	free(local_data);
	free(local_count);	
	MPI_Finalize();
	return 0;
}
