#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

inline double f (double x) { return x*x; }

int main (int argc, char *argv[])
{
  int my_rank, comm_sz, i;
  double local_I, I;
  double local_a, local_b, local_n;
  double a, b, n, h, x;
  a = 0;
  b = 3;
  n = 2048;
  h = (b-a) / n;

  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

  if (my_rank == 0)
    local_n = n / comm_sz;
  MPI_Bcast(&local_n,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
  sleep(my_rank);

  local_I = 0;
  local_a = a + my_rank*local_n;
  local_b = local_a + local_n;
  //printf("Proc %d -- local_a = %lf -- local_b = %lf\n",my_rank,local_a,local_b);

  for (i = local_a; i < local_b; i++)
  {
    x = i*h;
    local_I += f(x)*h;
  }
  MPI_Reduce(&local_I,&I,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  if (my_rank == 0)
    printf("Integral value = %e\n",I);

  MPI_Finalize();
  return 0;
}
