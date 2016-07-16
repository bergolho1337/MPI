#include "mpi_mat_vec.h"

void getDimension (int *local_m, int *local_n, int my_rank, int comm_sz)
{
  // Calcula o tamanho dos subintervalos
  *local_m = m / comm_sz;
  *local_n = n / comm_sz;
}

void allocArrays (double **local_A, double **local_x, double **local_y, int local_m, int local_n)
{
  *local_A = malloc(sizeof(double)*local_m*n);
  *local_x = malloc(sizeof(double)*local_n);
  *local_y = malloc(sizeof(double)*local_m);
}

void readMatrix (double *local_A, int local_m, int my_rank, int MAT_TYPE)
{
  int i, j;
  double *A = NULL;       // Armazena a matriz completa
  // Processo 0 le a matriz completa
  if (my_rank == 0)
  {
    A = (double*)malloc(sizeof(double)*m*n);
    // ** Colocar em uma funcao **
    switch (MAT_TYPE)
    {
      // Identity = I
      case 1:
      {
        for (i = 0; i < m; i++)
        {
          for (j = 0; j < n; j++)
          {
            if (i == j)
              A[i*n+j] = 1;
            else
              A[i*n+j] = 0;
          }
        }
        break;
      }
      case 2:
      {
        for (i = 0; i < m; i++)
        {
          for (j = 0; j < n; j++)
            A[i*n+j] = (i+1)*(j+1);
        }
        break;
      }
    }
    MPI_Scatter(A,local_m*n,MPI_DOUBLE,local_A,local_m*n,MPI_DOUBLE,0,MPI_COMM_WORLD);
  }
  else
    MPI_Scatter(A,local_m*n,MPI_DOUBLE,local_A,local_m*n,MPI_DOUBLE,0,MPI_COMM_WORLD);
}

void readVector (double *local_x, int local_n, int my_rank)
{
  int i;
  double *x = NULL;
  if (my_rank == 0)
  {
    x = malloc(sizeof(double)*n);
    for (i = 0; i < n; i++)
      x[i] = i+1;
    MPI_Scatter(x,local_n,MPI_DOUBLE,local_x,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    free(x);
  }
  else
    MPI_Scatter(x,local_n,MPI_DOUBLE,local_x,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);

}

void Mat_Vec_Parallel (double *local_A, double *local_x, double *local_y, int local_m, int local_n)
{
  int i, j;
  double *x = NULL;
  x = malloc(sizeof(double)*n);
  // Reconstroi o vetor x
  MPI_Allgather(local_x,local_n,MPI_DOUBLE,x,local_n,MPI_DOUBLE,MPI_COMM_WORLD);
  for (i = 0; i < local_m; i++)
  {
    local_y[i] = 0.0;
    for (j = 0; j < n; j++)
      local_y[i] += local_A[i*n+j]*x[j];
    //printf("%lf\n",local_y[i]);
  }
  free(x);
}

void printLocalVector (double *local_v, int local_n, int my_rank, int comm_sz)
{
  int i;
  char message[100005], str[10];
  if (my_rank != 0)
  {
    sprintf(message,"Hello I'm process %d my local_x is:\n",my_rank);
    for (i = 0; i < local_n; i++)
    {
      sprintf(str,"%lf\n",local_v[i]);
      strcat(message,str);
    }
    // Envia uma mensagem para o processo 0 contendo uma string contendo o "local_A" do processo "my_rank"
    MPI_Send(message,strlen(message)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
  }
  // Processo 0 recebe as mensagens contendo as matrizes local_A de cada processo
  else
  {
    int q;
    printf("Hello I'm process %d my local_x is:\n",my_rank);
    for (i = 0; i < local_n; i++)
    {
      printf("%lf\n",local_v[i]);
    }
    printf("\n");
    for (q = 1; q < comm_sz; q++)
    {
      MPI_Recv(message,100005,MPI_CHAR,q,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("%s\n",message);
    }

  }
}

void printLocalMatrix (double *local_A, int local_m, int my_rank, int comm_sz)
{
  int i, j;
  char message[100005], str[10];
  if (my_rank != 0)
  {
    sprintf(message,"Hello I'm process %d my local_A is:\n",my_rank);
    for (i = 0; i < local_m; i++)
    {
      for (j = 0; j < n; j++)
      {
        sprintf(str,"%lf ",local_A[i*n+j]);
        strcat(message,str);
      }
      sprintf(str,"\n");
      strcat(message,str);
    }
    // Envia uma mensagem para o processo 0 contendo uma string contendo o "local_A" do processo "my_rank"
    MPI_Send(message,strlen(message)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
  }
  // Processo 0 recebe as mensagens contendo as matrizes local_A de cada processo
  else
  {
    int q;
    printf("Hello I'm process %d my local_A is:\n",my_rank);
    for (i = 0; i < local_m; i++)
    {
      for (j = 0; j < n; j++)
        printf("%lf ",local_A[i*n+j]);
      printf("\n");
    }
    printf("\n");
    for (q = 1; q < comm_sz; q++)
    {
      MPI_Recv(message,100005,MPI_CHAR,q,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("%s\n",message);
    }

  }
}

void printMatrix (double *local_A, int local_m, int my_rank)
{
  int i, j;
  double *A = NULL;
  if (my_rank == 0)
  {
    A = (double*)malloc(sizeof(double)*m*n);
    MPI_Gather(local_A,local_m*n,MPI_DOUBLE,A,local_m*n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    printf("The matrix A\n");
    for (i = 0; i < m; i++)
    {
      for (j = 0; j < n; j++)
        printf("%lf ",A[i*m+j]);
      printf("\n");
    }
  }
  else
    MPI_Gather(local_A,local_m*n,MPI_DOUBLE,A,local_m*n,MPI_DOUBLE,0,MPI_COMM_WORLD);
}

void printVector (double *local_v, int local_m, int my_rank)
{
  int i;
  double *v = NULL;
  if (my_rank == 0)
  {
    v = malloc(sizeof(double)*m);
    MPI_Gather(local_v,local_m,MPI_DOUBLE,v,local_m,MPI_DOUBLE,0,MPI_COMM_WORLD);
    printf("Result vector is:\n");
    for (i = 0; i < m; i++)
      printf("%lf\n",v[i]);
    free(v);
  }
  else
    MPI_Gather(local_v,local_m,MPI_DOUBLE,v,local_m,MPI_DOUBLE,0,MPI_COMM_WORLD);
}
