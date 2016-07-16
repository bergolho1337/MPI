#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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

int main() 
{
	int my_rank, comm_sz, n = 1000, local_n, resto; // Onde n é quantidade de avaliações
	double a = 0.0, b = 3.0, h, local_a, local_b;
	double local_int, total_int;
	int source;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

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
	return 0;
	return 0;
} 
