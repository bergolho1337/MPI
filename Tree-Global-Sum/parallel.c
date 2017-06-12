/*
 * ==================================================================
 *	Programming Assignments 3.3:
 *      Tree-Structure Global Sum
 *
 * Compile: make
 * Run: mpiexec -n <num_procs> ./parallel
 * <num_procs> = Number of processes
 * ==================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../timer.h"

const int n = 8;

int* allocMemory (int size)
{
    int *buffer = (int*)calloc(size,sizeof(int));
    return buffer;
}

void generateData (int *data)
{
    int i;
    for (i = 0; i < n; i++)
        //data[i] = rand() % max_value;
		data[i] = i+1;
}

void printData (int *data, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%d\n",data[i]);
	printf("\n");
}

int computeGlobalSum (int *data, int size)
{
    int i, sum;
    for (i = 0, sum = 0; i < size; i++)
        sum += data[i];
    return sum;
}

void My_Reduce_Sum (int *local_sum, int *global_sum, int my_rank, int comm_sz)
{
    //sleep(my_rank*2);
    //printf("Process %d\n",my_rank);
    //printf("Local_sum = %d\n",*local_sum);

    int num_proc = n / 2 - 1;
    int offset = comm_sz / 2;
    int recv_sum;
    while (offset != 0)
    {
        // Send my local_sum to the firsts processes
        if (my_rank >= offset)
        {   
            // Let only the half of the processes do this step
            if (my_rank <= num_proc+offset)
                MPI_Send(local_sum,1,MPI_INT,my_rank-offset,my_rank,MPI_COMM_WORLD);
        }
        // Receive local_sum from the ahead processes
        else
        {
            MPI_Recv(&recv_sum,1,MPI_INT,my_rank+offset,my_rank+offset,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            *local_sum += recv_sum;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        offset /= 2;
        num_proc /= 2;
    }
    if (my_rank == 0)
            printf("Local_sum = %d\n",*local_sum);
}

int main (int argc, char *argv[])
{
	int my_rank, comm_sz;
	int *data, *local_data, local_n;							
	int global_sum, local_sum;				
	double start, finish, local_elapsed, elapsed;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

	local_n = n / comm_sz;
	data = NULL;
	local_data = allocMemory(local_n);	 

	if (my_rank == 0)
	{
		data = allocMemory(n);
		generateData(data);
		MPI_Scatter(data,local_n,MPI_INT,local_data,local_n,MPI_INT,0,MPI_COMM_WORLD);
		free(data);
	}
	else
		MPI_Scatter(data,local_n,MPI_INT,local_data,local_n,MPI_INT,0,MPI_COMM_WORLD);

	GET_TIME(start);

	local_sum = computeGlobalSum(local_data,local_n);
	//MPI_Reduce(&local_sum,&global_sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    My_Reduce_Sum(&local_sum,&global_sum,my_rank,comm_sz);
	
    GET_TIME(finish);
	local_elapsed = finish - start;

    //sleep(my_rank*2);
    //printf("Process %d\n",my_rank);
    //printf("Local_sum = %d\n",local_sum);
    //printData(local_data,local_n);

	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	if (my_rank == 0)
	{
		printf("With n = %d\n",n);
        printf("Global sum = %d\n",global_sum);
		printf("Time elapsed = %.10lf\n",elapsed);
	}
		
	free(local_data);	
	MPI_Finalize();
	return 0;
}
