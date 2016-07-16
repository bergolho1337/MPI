/*
  Programa para multiplicacao matriz-vetor utilizando MPI.
    - Matriz = dimensao MxN e Vetor = dimensao N
    - O tamanho da matriz e do vetor podem ser alterados no arquivo header "mpi_mat_vec.h"
  ** Funciona apenas quando a ordem da matriz eh divisivel pelo numero de processos disparados. **
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "mpi_mat_vec.h"

#define MAT_TYPE 1                  // Create a specific matrix. 1 = Identity, 2 = Identity*2

int main (int argc, char *argv[])
{
  int my_rank, comm_sz;
  int local_n, local_m;
  double *local_A, *local_x, *local_y;
  //double local_start, local_finish, local_elapsed, total_elapsed;

  // Initialize MPI
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

  getDimension(&local_m,&local_n,my_rank,comm_sz);
  allocArrays(&local_A,&local_x,&local_y,local_m,local_n);
  readMatrix(local_A,local_m,my_rank,MAT_TYPE);
  readVector(local_x,local_n,my_rank);

  Mat_Vec_Parallel(local_A,local_x,local_y,local_m,local_n);

  //printMatrix(local_A,local_m,my_rank);
  //printLocalMatrix(local_A,local_m,my_rank,comm_sz);
  //printLocalVector(local_x,local_n,my_rank,comm_sz);
  printVector(local_y,local_m,my_rank);

  free(local_A);
  free(local_x);
  free(local_y);
  MPI_Finalize();

  return 0;
}
