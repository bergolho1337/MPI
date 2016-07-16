#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "vec_sum.h"

int main (int argc, char *argv[])
{
    int my_rank, comm_sz;
    int local_n;
    double *local_a, *local_b, *local_c;
    double local_start, local_finish, local_elapsed, elapsed;
    // Initialize MPI
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    // Passa o endereco para alterar o valor e manter a mudanca na variavel
    readSize(&local_n,my_rank,comm_sz);

    // Aloca e recebe o vetor "local_a"
    allocVector(&local_a,local_n);
    //readVector(local_a,local_n,my_rank,comm_sz);
    generateVector(local_a,local_n,my_rank,comm_sz);

    // Aloca e recebe o vetor "local_b"
    allocVector(&local_b,local_n);
    //readVector(local_b,local_n,my_rank,comm_sz);
    generateVector(local_b,local_n,my_rank,comm_sz);

    // Aloca o vetor "local_c"
    allocVector(&local_c,local_n);

    // Barreira para disparar o cronometro
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
        // Faz a operacao de soma de vetor paralela
        parallelVectorSum(local_a,local_b,local_c,local_n);
        // Imprime o vetor completo
        //printResult(local_c,local_n,my_rank,comm_sz);
    local_finish = MPI_Wtime();
    local_elapsed = local_finish - local_start;
    // Calcula o maior tempo de execucao de todos os processos
    MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if (my_rank == 0)
      printf("With %d processes\nTime elapsed: %e (s)\n",comm_sz,elapsed);

    //printVector(local_a,local_n,my_rank,comm_sz,"a");
    //printVector(local_b,local_n,my_rank,comm_sz,"b");

    MPI_Finalize();
    return 0;
}
