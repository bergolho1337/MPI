#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define m 8           // Numero de linhas da matriz
#define n 8           // Numero de colunas da matriz

// Calcula o tamanho do bloco de linhas da matriz que de cada processo vai pegar
void getDimension (int *local_m, int *local_n, int my_rank, int comm_sz);

// Aloca memoria para as estruturas
void allocArrays (double **local_A, double **local_x, double **local_y, int local_m, int local_n);

// Le a matriz
void readMatrix (double *local_A, int local_m, int my_rank, int MAT_TYPE);

// Le o vetor
void readVector (double *local_x, int local_n, int my_rank);

// Realiza a operacao de multiplicacao matriz-vetor paralelo
void Mat_Vec_Parallel (double *local_A, double *x, double *y, int local_m, int local_n);

// Imprime as partes da matriz A de cada processo
void printLocalMatrix (double *local_A, int local_m, int my_rank, int comm_sz);

// Imprime as partes do vetor x de cada processo
void printLocalVector (double *local_v, int local_n, int my_rank, int comm_sz);

// Imprime a matriz A completa
void printMatrix (double *local_A, int local_m, int my_rank);

// Imprime o vetor
void printVector (double *local_v, int local_m, int my_rank);
