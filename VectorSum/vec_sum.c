#include "vec_sum.h"

// Recebe o tamanho dos vetores da operacao e ja calcula o valor dos sub-vetores com as somas parciais
void readSize (int *local_n, int my_rank, int comm_sz)
{
    int n;
    // Processo 0 recebe o tamanho do vetor e calcula o tamanho dos sub-vetores
    if (my_rank == 0)
    {
      scanf("%d",&n);
      *local_n = n / comm_sz;
    }
    // Distribui para todos os processos o valor de "local_n"
    MPI_Bcast(local_n,1,MPI_INT,0,MPI_COMM_WORLD);
    //printf("Hello from process %d! My local_n is = %d\n",my_rank,*local_n);
}

// Aloca memoria para o vetor contendo a soma parcial de cada processo
void allocVector (double **local_v, int local_n)
{
  // Aloca memoria para o sub-vetor de cada processo. Passa um ponteiro duplo para alterar o valor e permanecer a mudanca
  *local_v = (double*)malloc(sizeof(double)*local_n);
}

// Recebe um vetor vindo da entrada padrao "stdin"
void readVector (double *local_v, int local_n, int my_rank, int comm_sz)
{
  double *v = NULL;
  if (my_rank == 0)
  {
    int i;
    int n = local_n*comm_sz;
    v = (double*)malloc(sizeof(double)*n);
    // Recebe os valores do vetor
    for (i = 0; i < n; i++)
      scanf("%lf",&v[i]);
    // Quebra o vetor "v" em tamanhos de "local_n" e distribui esses pedacos para os processos na variavel "local_v"
    MPI_Scatter(v,local_n,MPI_DOUBLE,local_v,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    free(v);
  }
  else
    MPI_Scatter(v,local_n,MPI_DOUBLE,local_v,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
}

// Generate a vector of size "n", where each element is in the range [-1000,1000]
void generateVector (double *local_v, int local_n, int my_rank, int comm_sz)
{
  double *v = NULL;
  if (my_rank == 0)
  {
    int i, sign;
    int n = local_n*comm_sz;
    v = (double*)malloc(sizeof(double)*n);
    for (i = 0; i < n; i++)
    {
      v[i] = rand() % 1000;
      sign = rand() % 2;
      if (sign)
        v[i] *= -1;
    }
    // Quebra o vetor "v" em tamanhos de "local_n" e distribui esses pedacos para os processos na variavel "local_v"
    MPI_Scatter(v,local_n,MPI_DOUBLE,local_v,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    free(v);
  }
  else
    MPI_Scatter(v,local_n,MPI_DOUBLE,local_v,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
}

// Realiza a operacao soma entre dois vetores contendo os pedacos parciais de cada processo
void parallelVectorSum (double *local_a, double *local_b, double *local_c, int local_n)
{
  int i;
  for (i = 0; i < local_n; i++)
    local_c[i] = local_a[i] + local_b[i];
}

// Imprime o vetor resultante da operacao de soma
void printResult (double *local_c, int local_n, int my_rank, int comm_sz)
{
  int i, n;
  double *c = NULL;
  // Processo 0 recebe os pedacos dos outros processos e imprime o vetor completo
  if (my_rank == 0)
  {
    n = local_n*comm_sz;
    c = (double*)malloc(sizeof(double)*n);
    MPI_Gather(local_c,local_n,MPI_DOUBLE,c,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    printf("Result of the sum:\n");
    for (i = 0; i < n; i++)
        printf("%lf\n",c[i]);
    printf("\n");
    free(c);
  }
  // Outros processos mandam seus pedacos com a soma parcial
  else
  {
    MPI_Gather(local_c,local_n,MPI_DOUBLE,c,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);
  }
}

// Imprime o vetor "local_v" de cada processo
void printVector (double *local_v, int local_n, int my_rank, int comm_sz, char *name)
{
  int i;
  char message[10000], str[10];
  // Processos que nao sao o 0 enviando uma mensagem contendo o valor de "local_v" como uma string
  if (my_rank != 0)
  {
    sprintf(message,"Hello, I'm process %d and this is my local vector\n",my_rank);
    for (i = 0; i < local_n; i++)
    {
      sprintf(str,"%lf\n",local_v[i]);
      //printf("%lf\n",local_v[i]);
      strcat(message,str);

    }
    // Envia uma mensagem para o processo 0 contendo uma string contendo o "local_v" do processo "my_rank"
    MPI_Send(message,strlen(message)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);

  }
  // Processo 0 recebe as mensagens dos outros processos e imprime em ordem os valores de "local_v"
  else
  {
    int q;
    printf("%s :\n",name);
    for (q = 1; q < comm_sz; q++)
    {
      MPI_Recv(message,10000,MPI_CHAR,q,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("%s\n",message);
    }
    printf("Hello, I'm process %d and this is my local vector\n",my_rank);
    for (i = 0; i < local_n; i++)
      printf("%lf\n",local_v[i]);
    printf("\n");
  }
}
