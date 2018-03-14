#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "timer.h"

const int DIM = 3;

// typedef MACRO: vect_t is equal to an array of size DIM.
typedef double vect_t[DIM];
typedef unsigned int uint;
 
uint N;

void printVector (const char msg[], const vect_t *v, const uint N)
{
    printf("%s\n",msg);
    for (uint i = 0; i < N; i++)
        printf("Point %u: (%.2lf,%.2lf,%.2lf)\n",i,v[i][0],v[i][1],v[i][2]);
}

void readInput (vect_t *v, vect_t *u, const int N, const int local_N, const int my_rank)
{
    if (my_rank == 0)
    {
        vect_t *v_full, *u_full;
        v_full = (vect_t*)malloc(sizeof(vect_t)*N);
        u_full = (vect_t*)malloc(sizeof(vect_t)*N);
        for (int i = 0; i < N; i++)
            scanf("%lf %lf %lf %lf %lf %lf",&v_full[i][0],&v_full[i][1],&v_full[i][2],\
                                        &u_full[i][0],&u_full[i][1],&u_full[i][2]);
        MPI_Scatter(v_full,local_N,MPI_DOUBLE,local_x,local_n,MPI_DOUBLE,0,MPI_COMM_WORLD);                            
    }

}

void allocMemory (vect_t **u, vect_t **v, vect_t **w, const int local_N)
{
    *u = (vect_t*)malloc(sizeof(vect_t)*(local_N));    
    *v = (vect_t*)malloc(sizeof(vect_t)*(local_N));
    *w = (vect_t*)malloc(sizeof(vect_t)*(local_N));
}

void vectorAdd (const vect_t *v, const vect_t *u, vect_t *w, const int N)
{
    for (uint i = 0; i < N; i++)
    {
        w[i][0] = u[i][0] + v[i][0];
        w[i][1] = u[i][1] + v[i][1];
        w[i][2] = u[i][2] + v[i][2]; 
    }
}

void getDimension (int *N, int *local_N, int my_rank, int comm_sz)
{
    if (my_rank == 0)
    {
        scanf("%d",N);
        *local_N = (*N) / comm_sz;
    }
    MPI_Bcast(N,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(local_N,1,MPI_INT,0,MPI_COMM_WORLD);
}

int main (int argc, char *argv[])
{
    int N, local_N;
    int my_rank, comm_sz;
    double start, finish, elapsed;
    vect_t *u, *v, *w;

     // Initialize MPI
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    getDimension(&N,&local_N,my_rank,comm_sz);
    allocMemory(&u,&v,&w,local_N);
    readInput(u,v,N,local_N);

    /*
    allocMemory(&u,&v,&w,&N);
    readInput(u,v,N);
    
    GET_TIME(start);
    vectorAdd(u,v,w,N);
    GET_TIME(finish);
    elapsed = finish - start;

    #ifdef DEBUG
    printVector("Vector u",u,N);
    printVector("Vector v",v,N);
    printVector("Vector w",w,N);
    #endif
    
    printf("Time elapsed = %.10lf\n",elapsed);
    
    free(u);
    free(v);
    free(w);
    */

    MPI_Finalize();
    return 0;
}