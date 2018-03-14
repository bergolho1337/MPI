#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Usage (char *argv[], int my_rank);


double fx (double x)
{
	return (x*x);
}

double Trap (double a, double b, double n, double h)
{
	int i;
	double aprox, x_i;
	aprox = (fx(a) + fx(b))/2.0;
	for (i = 1; i < n-1; i++)
	{
		x_i = a + i*h;
		aprox += fx(x_i);
	}
	return (h*aprox);
}

void readInput (char *argv[], int my_rank, double *a, double *b, int *n)
{
    if (my_rank == 0)
    {
        *a = atof(argv[1]);
        *b = atof(argv[2]);
        *n = atoi(argv[3]);
    }
    MPI_Bcast(a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(n,1,MPI_INT,0,MPI_COMM_WORLD);
}

void Usage (char *argv[], int my_rank)
{
    if (my_rank == 0)
    {
        printf("====================================================\n");
        printf("Usage:> %s <a> <b> <n>\n",argv[0]);
        printf("<a> = Lower limit of the integral\n");
        printf("<b> = Upper limit of the integral\n");
        printf("<n> = Number of trapezius used for the integral\n");
        printf("====================================================\n");
    }
}

void calcSizes (int my_rank, int comm_sz, double a, double b, int n, \
                double *local_a, double *local_b, double *h, int *local_n, int *resto)
{
    *local_n = n / comm_sz;
    *resto = n % comm_sz;
    *h = (b-a) / n;

    *local_a = a + my_rank*(*local_n)*(*h);
	*local_b = (*local_a) + (*local_n)*(*h);
    
}

int main (int argc, char *argv[]) 
{
	int my_rank, comm_sz, n, local_n, resto;
	double a, b, h, local_a, local_b;
	double local_int, total_int;
	int source;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    if (argc-1 == 3)
    {
        readInput(argv,my_rank,&a,&b,&n);
        calcSizes(my_rank,comm_sz,a,b,n,&local_a,&local_b,&h,&local_n,&resto);
        
        #ifdef DEBUG
        sleep(my_rank*2);
        printf("Process %d:> local_a = %lf\n",my_rank,local_a);
        printf("Process %d:> local_b = %lf\n",my_rank,local_b);
        printf("Process %d:> local_n = %d\n",my_rank,local_n);
        printf("Process %d:> h = %lf\n",my_rank,h);
        printf("Process %d:> resto = %d\n",my_rank,resto);
        #endif
        //calcIntegral();
        //printResult();
        

    }        
    else
    {
        Usage(argv,my_rank);
    }
    MPI_Finalize();

    /*
	h = (b-a)/n;
	local_n = n/comm_sz;
	resto = n % comm_sz;

	//printf("Valor de h = %e\n",h);
	//printf("Valor de local_n = %d\n",local_n);

	local_a = a + my_rank*local_n*h;
	local_b = local_a + local_n*h;
	local_int = Trap(local_a,local_b,local_n,h);
	
	// Se temos resto e sou o último processo, acrescentar o resto no último processo
	if (my_rank == comm_sz-1 && resto != 0)
	{
		local_a = local_b;
		local_b = local_a + resto*h;
		local_int += Trap(local_a,local_b,resto,h);
	}
	
	printf("Processo %d -- local_a = %e\n",my_rank,local_a);
	printf("Processo %d -- local_b = %e\n",my_rank,local_b);
	printf("Processo %d -- local_int = %e\n",my_rank,local_int);

	if (my_rank != 0)
	{
		MPI_Send(&local_int,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
	}
	else
	{
		total_int = local_int;
		for (source = 1; source < comm_sz; source++)
		{
			MPI_Recv(&local_int,1,MPI_DOUBLE,source,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			total_int += local_int;
		}
	}

	if (my_rank == 0)
	{
		printf("With n = %d trapezoids, our estimate ",n);
		printf("of the integral from %f to %f = %e\n",a,b,total_int);
	}	
	MPI_Finalize();
    */
	return 0;
} 
