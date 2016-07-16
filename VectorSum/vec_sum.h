#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void readSize (int *local_n, int my_rank, int comm_sz);
void readVector (double *local_v, int local_n, int my_rank, int comm_sz);
void generateVector (double *local_v, int local_n, int my_rank, int comm_sz);
void printVector (double *local_v, int local_n, int my_rank, int comm_sz, char *name);
void allocVector (double **local_v, int local_n);
void parallelVectorSum (double *local_a, double *local_b, double *local_c, int local_n);
void printResult (double *local_c, int local_n, int my_rank, int comm_sz);
