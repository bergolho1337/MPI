// Versao 2 - da multiplicacao Matriz-vetor usando Scatterv e Gatherv
// A*x = b

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define n 10     // Numero de linhas da matriz
#define m 10     // Numero de colunas da matriz
#define DEBUG

double* getLocalA (int local_n, int r, int my_rank, int comm_sz)
{
  int *scounts, *displs, i, j;
  double *a, *local_a;

  // Leitura da matriz
  scounts = (int*)malloc(sizeof(int)*comm_sz);            // Quantidade de cada processo
  displs = calloc(comm_sz,sizeof(int));                   // Deslocamento de cada processo
  scounts[0] = (r+local_n)*m;                             // Processo 0 pega as sobras
  for (i = 1; i < comm_sz; i++)
  {
      scounts[i] = local_n*m;
      displs[i] = displs[i-1] + scounts[i-1];
  }

  // Aloca memoria para a matriz local
  local_a = (double*)malloc(sizeof(double)*scounts[my_rank]);

  // Processo 0 le os dados
  if (my_rank == 0)
  {
    a = (double*)malloc(sizeof(double)*n*m);
    for (i = 0; i < n; i++)
    {
      for (j = 0; j < m; j++)
        a[i*m+j] = (double)(i+1);
    }
  }
  // Distribuir os pedacos da matriz
  MPI_Scatterv(a,scounts,displs,MPI_DOUBLE,local_a,scounts[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
  if (my_rank == 0) free(a);

  #ifdef DEBUG
  sleep(my_rank*2);
  printf("Proc %d\tlocal_a\n",my_rank);
  for (i = 0; i < scounts[my_rank]/m; i++)
  {
    for (j = 0; j < m; j++)
      printf("%lf ",local_a[i*m+j]);
    printf("\n");
  }
  printf("\n");
  #endif

  free(scounts);
  free(displs);
  return local_a;
}

double* getVectorX (int my_rank)
{
  int i;
  double *x;
  // Aloca memoria para o vetor x inteiro
  x = (double*)malloc(sizeof(double)*m);
  if (my_rank == 0)
  {
    for (i = 0; i < m; i++)
      x[i] = (double)i;
  }
  // Distribuir o vetor x
  MPI_Bcast(x,m,MPI_DOUBLE,0,MPI_COMM_WORLD);

  #ifdef DEBUG
  sleep(my_rank*2);
  printf("Proc %d\tx\n",my_rank);
  for (i = 0; i < m; i++)
    printf("%lf ",x[i]);
  printf("\n");
  #endif

  return x;
}

void parallelMatrixVecMult (double *local_a, double *x, int local_n, int r, int my_rank, int comm_sz)
{
  int i, j, size;
  int *scounts, *displs;
  double *local_b, *b;
  b = (double*)malloc(sizeof(double)*n);
  // Calcula o tamanho de cada bloco
  if (my_rank == 0)
    size = local_n+r;
  else
    size = local_n;
  // Aloca  memoria para o resultado local
  local_b = calloc(size,sizeof(double));
  // Realiza a operacao local
  for (i = 0; i < size; i++)
  {
    for (j = 0; j < m; j++)
      local_b[i] += local_a[i*m+j]*x[j];
  }
  // Processo 0 junta as partes locais e distribui o vetor completo para todo mundo
  scounts = calloc(comm_sz,sizeof(int));
  displs = calloc(comm_sz,sizeof(int));
  scounts[0] = (r+local_n);
  for (i = 1; i < comm_sz; i++)
  {
    scounts[i] = local_n;
    displs[i] = displs[i-1] + scounts[i-1];
  }
  MPI_Allgatherv(local_b,scounts[my_rank],MPI_DOUBLE,b,scounts,displs,MPI_DOUBLE,MPI_COMM_WORLD);

  #ifdef DEBUG
  sleep(my_rank*2);
  printf("Proc %d\tb\n",my_rank);
  for (i = 0; i < n; i++)
    printf("%lf ",b[i]);
  printf("\n");
  #endif

}

int main ()
{
  int my_rank, comm_sz;
  int local_n, r;
  int *scounts, *displs;
  double *local_a, *x, *b;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

  // Definindo os tamanhos de cada processo
  local_n = n / comm_sz;
  r = n % comm_sz;

  local_a  = getLocalA(local_n,r,my_rank,comm_sz);
  x = getVectorX(my_rank);

  parallelMatrixVecMult(local_a,x,local_n,r,my_rank,comm_sz);

  MPI_Finalize();
  return 0;
}
