#include <stdio.h>
#include <stdlib.h>
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

void allocMemory (vect_t **u, vect_t **v, vect_t **w, uint *N)
{
    scanf("%u",N);
    *u = (vect_t*)malloc(sizeof(vect_t)*(*N));    
    *v = (vect_t*)malloc(sizeof(vect_t)*(*N));
    *w = (vect_t*)malloc(sizeof(vect_t)*(*N));
}

void readInput (vect_t *v, vect_t *u, const uint N)
{
    for (uint i = 0; i < N; i++)
        scanf("%lf %lf %lf %lf %lf %lf",&v[i][0],&v[i][1],&v[i][2],\
                                        &u[i][0],&u[i][1],&u[i][2]);
}

void vectorAdd (const vect_t *v, const vect_t *u, vect_t *w, const uint N)
{
    for (uint i = 0; i < N; i++)
    {
        w[i][0] = u[i][0] + v[i][0];
        w[i][1] = u[i][1] + v[i][1];
        w[i][2] = u[i][2] + v[i][2]; 
    }
}

int main (int argc, char *argv[])
{
    uint N;
    double start, finish, elapsed;
    vect_t *u, *v, *w;

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

    return 0;
}