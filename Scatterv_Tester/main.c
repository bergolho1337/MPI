#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define n 100     // Tamanho do vetor

void readInput (double *local_v, int local_n, int r, int my_rank, int comm_sz)
{
  int *scounts, *displs, i;
  double *v;

  scounts = (int*)malloc(sizeof(int)*comm_sz);          // Quantidade de cada processo
  displs = calloc(comm_sz,sizeof(int));                 // Deslocamento de cada processo
  scounts[0] = local_n + r;                             // Processo 0 pega as sobras
  for (i = 1; i < comm_sz; i++)
  {
      scounts[i] = local_n;
      displs[i] = displs[i-1] + scounts[i-1];
  }
  // Aloca memoria para o vetor local
  local_v = (double*)malloc(sizeof(double)*scounts[my_rank]);
  if (my_rank == 0)
  {
    v = (double*)malloc(sizeof(double)*n);
    for (i = 0; i < n; i++)
      v[i] = (double)i;
  }
  MPI_Scatterv(v,scounts,displs,MPI_DOUBLE,local_v,scounts[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
  if (my_rank == 0) free(v);

  sleep(my_rank*2);
  printf("Proc %d\n",my_rank);
  for (i = 0; i < scounts[my_rank]; i++)
    printf("%lf\n",local_v[i]);
  printf("\n");
}

int main ()
{
  int my_rank, comm_sz;
  int local_n, r;
  double *local_v;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

  local_n = n / comm_sz;
  r = n % comm_sz;
  readInput(local_v,local_n,r,my_rank,comm_sz);

  MPI_Finalize();
  return 0;
}
