// *******************************************************************************************
// Programa paralelo que calcula o produto escalar entre dois vetores: v3 = v1.v2
// E calcula o vetor resultante da multiplicação dele por um escalar: v1 = v1*scalar
// Neste caso tratamos o caso em que TAM % comm_sz != 0
// Retiramos o MPI_Scatter de dentro do 'if else'
// Incluiu-se o calculo do tempo com MPI_Wtime()
// *******************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Usage (const char pname[])
{
    printf("=========================================================================\n");
    printf("Usage:> %s <n>\n",pname);
    printf("<n> = Size of the vectors\n");
    printf("=========================================================================\n");
}

void Get_Sizes (char *argv[], int scount[], int displ[], int *n, const int my_rank, const int comm_sz)
{
    if (my_rank == 0)
    {
        int local_n, resto, elems;
        *n = atoi(argv[1]);
        local_n = (*n) / comm_sz;
        resto = (*n) % comm_sz;
        elems = 0;

        //printf("local_n = %d || resto = %d\n",local_n,resto);

        for (int i = 0; i < comm_sz; i++)
        {
            scount[i] = local_n;
            if (resto > 0)
            {
                scount[i]++;
                resto--;
            }
            displ[i] = elems;
            elems += scount[i];
        }
    }
        
    MPI_Bcast(scount,comm_sz,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(displ,comm_sz,MPI_INT,0,MPI_COMM_WORLD);
}

void Read_Vectors (double *local_v1, double *local_v2, \
                  int *scount, int *displ, const int n, const int my_rank)
{
    double *v1 = NULL, *v2 = NULL;

    // Allocate global vector on process 0
    if (my_rank == 0)
    {
        v1 = malloc(sizeof(double)*n);
        v2 = malloc(sizeof(double)*n);

        printf("Vector v1\n");
        for (int i = 0; i < n; i++)
        {
            //printf("Position %d: ",i);
            scanf("%lf",&v1[i]);
        }
        printf("Vector v2\n");
        for (int i = 0; i < n; i++)
        {
            //printf("Position %d: ",i);
            scanf("%lf",&v2[i]);
        }
    }
    
    // Scatter global vectors on its local vectors among the other processes
    MPI_Scatterv(v1,scount,displ,MPI_DOUBLE,local_v1,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatterv(v2,scount,displ,MPI_DOUBLE,local_v2,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);

    // Free the memory of the global vector on process 0
    if (my_rank == 0)
    {
        free(v1);
        free(v2);
    }
}

void Generate_Vectors (double *local_v1, double *local_v2, \
                  int *scount, int *displ, const int n, const int my_rank)
{
    double *v1 = NULL, *v2 = NULL;

    // Allocate global vector on process 0
    if (my_rank == 0)
    {
        v1 = malloc(sizeof(double)*n);
        v2 = malloc(sizeof(double)*n);

        for (int i = 0; i < n; i++)
        {
            //printf("Position %d: ",i);
            v1[i] = rand() % 2;
        }
        for (int i = 0; i < n; i++)
        {
            //printf("Position %d: ",i);
            v2[i] = rand() % 2;
        }
    }
    
    // Scatter global vectors on its local vectors among the other processes
    MPI_Scatterv(v1,scount,displ,MPI_DOUBLE,local_v1,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatterv(v2,scount,displ,MPI_DOUBLE,local_v2,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);

    // Free the memory of the global vector on process 0
    if (my_rank == 0)
    {
        free(v1);
        free(v2);
    }
}

void AllocVectors (double **local_v1, double **local_v2, const int local_n)
{
    *local_v1 = (double*)malloc(sizeof(double)*local_n);
    *local_v2 = (double*)malloc(sizeof(double)*local_n);
}

void Dot_Product (double *dot_prod, double local_v1[], double local_v2[], const int scount[], const int my_rank)
{
    double local_dot = 0.0;
    
    for (int i = 0; i < scount[my_rank]; i++)
        local_dot += local_v1[i] * local_v2[i];
    
    MPI_Reduce(&local_dot,dot_prod,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    
}

int main (int argc, char *argv[]) 
{
	int my_rank, comm_sz, n;
	int *displ, *scount;
	double *v1, *v2, dot_prod;
	double *local_v1, *local_v2, local_dot;
    double start, finish, local_elapsed, total_elapsed;
	
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
	
    scount = malloc(sizeof(int)*comm_sz);
    displ = malloc(sizeof(int)*comm_sz);

    Get_Sizes(argv,scount,displ,&n,my_rank,comm_sz);
    AllocVectors(&local_v1,&local_v2,scount[my_rank]);
    //Read_Vectors(local_v1,local_v2,scount,displ,n,my_rank);
    Generate_Vectors(local_v1,local_v2,scount,displ,n,my_rank);

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    
    Dot_Product(&dot_prod,local_v1,local_v2,scount,my_rank);
    
    finish = MPI_Wtime();
    local_elapsed = finish - start;
    MPI_Reduce(&local_elapsed,&total_elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        printf("Dot product = %.10lf\n",dot_prod);
        printf("Time elapsed = %.10lf\n",total_elapsed);
    }
        

    free(local_v1);
    free(local_v2);
    free(scount);
    free(displ);

    MPI_Finalize();
	return 0;
} 
