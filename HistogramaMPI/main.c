#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void geraGrafico (int *count, double local_m, int M)
{
	FILE *arq = fopen("data.dat","w+");
	int i, max = 0;
	char str[50];
	// Escreve dados	
	for (i = 0; i < M; i++)
	{
		if (count[i] > max)
			max = count[i];
		sprintf(str,"%lf-%lf",i*local_m,i*local_m+local_m);
		fprintf(arq,"%s\t%d\n",str,count[i]);
	}
	fclose(arq);
	// Escreve arquivo de plot
	arq = fopen("graph.plt","w+");
	fprintf(arq,"set terminal png\n");
	fprintf(arq,"set output \"histogram.png\"\n");
	fprintf(arq,"set xlabel \"Subintervals\"\n");
	fprintf(arq,"set ylabel \"Frequency\"\n");
	fprintf(arq,"set xrange [-1:%d]\n",M);
	fprintf(arq,"set yrange [0:%d]\n",max+max/2);
	fprintf(arq,"set grid\n");
	fprintf(arq,"unset key\n");
	fprintf(arq,"set xtics rotate out\n");
	fprintf(arq,"set style data histogram\n");	
	fprintf(arq,"set style fill solid border\n");
	fprintf(arq,"set style histogram clustered\n");
	fprintf(arq,"set boxwidth 0.5\n");
	fprintf(arq,"plot for [COL=2:2] \"data.dat\" using COL:xticlabels(1)\n");
	fclose(arq);
	if (system("gnuplot graph.plt") == 0)
		printf("\n[+]Grafico plotado com sucesso!\n");
	else
		printf("\n[-]ERRO!\n");
}

// Calcula o tamanho do subintervalo ligado ao histograma
double intervalLength (int *data, int n, int M)
{
	int i;
	int limits[2];
	double local_m;
	limits[0] = limits[1] = data[0];
	for (i = 0; i < n; i++)
	{
		if (data[i] > limits[0])
			limits[0] = data[i];
		if (data[i] < limits[1])
			limits[1] = data[i];
	}
	local_m = (double)(limits[0])/(double)(M);
	return (local_m);
}

int main ()
{
	int my_rank, comm_sz;
	int *data, n, i, j, M;							// M = número de subintervalos
	int *limits;								// Vetor de limites - limits[0] = maior, limits[1] = menor
	int *count, *local_count;						// Vetor que conta quantos números estão no subintervalo
	int local_n, *local_data;
	double local_m;
	double start, finish, local_elapsed, elapsed;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

	data = NULL;	 

	if (my_rank == 0)
	{
		/* Setando valores predefinidos */
		n = pow(2,25);						// Tamanho do vetor
		data = (int*)malloc(sizeof(int)*n);
		for (i = 0; i < n; i++)		
			data[i] = rand() % 1000;			// Vetor vai estar entre 0 e 1000
		M = 5;							// Número de subintervalos

		/* Leitura pelo teclado */		
		// Leitura e cálculo dos parâmetros
		//printf("Tamanho do vetor:\n");
		//scanf("%d",&n);
		//data = (int*)malloc(sizeof(int)*n);
		//printf("Lendo vetor:\n");
		//for (i = 0; i < n; i++)
		//	scanf("%d",&data[i]);
		//printf("Numero de subintervalos:\n");
		//scanf("%d",&M);

		/* Calcula o tamanho do vetor que cada processo irá analisar */
		local_n = n/comm_sz;
		local_data = (int*)malloc(sizeof(int)*local_n);
		count = (int*)malloc(sizeof(int)*M);
		local_count = (int*)malloc(sizeof(int)*M);
		/* Calcula o tamanho do intervalo */
		local_m = intervalLength(data,n,M);
		for (i = 0; i < M; i++)
		{
			count[i] = 0;
			local_count[i] = 0;
		}

		/* Distribui as variáveis para todo mundo */
		MPI_Bcast(&local_n,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&local_m,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Bcast(&M,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Scatter(data,local_n,MPI_INT,local_data,local_n,MPI_INT,0,MPI_COMM_WORLD);
		
		
		MPI_Barrier(MPI_COMM_WORLD);
		start = MPI_Wtime();
		// Varrer o vetor de cada processo e vendo em qual subintervalo o valor está ...
		for (i = 0; i < local_n; i++)
		{
			for (j = 0; j < M; j++)
			{
				if (j == M-1)
				{
					if (local_data[i] >= j*local_m && local_data[i] <= j*local_m + local_m)
						local_count[j]++;
				}
				else
				{
					if (local_data[i] >= j*local_m && local_data[i] < j*local_m + local_m)
						local_count[j]++;
				}
			}
		}
		// Somar as somas de cada histograma e mandar para o processo 0
		MPI_Reduce(local_count,count,M,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		finish = MPI_Wtime();
		local_elapsed = finish - start;

		free(data);
		free(local_data);
		free(local_count);
		free(count);
		//geraGrafico(count,local_m,M);
		
	}
	else
	{
		/* Recebe as variaveis do processo 0 */
		MPI_Bcast(&local_n,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&local_m,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Bcast(&M,1,MPI_INT,0,MPI_COMM_WORLD);
		local_data = (int*)malloc(sizeof(int)*local_n);
		count = (int*)malloc(sizeof(int)*M);
		local_count = (int*)malloc(sizeof(int)*M);
		MPI_Scatter(data,local_n,MPI_INT,local_data,local_n,MPI_INT,0,MPI_COMM_WORLD);
		for (i = 0; i < M; i++)
		{
			count[i] = 0;
			local_count[i] = 0;
		}

		MPI_Barrier(MPI_COMM_WORLD);
		start = MPI_Wtime();
		// Varrer o vetor de cada processo e vendo em qual subintervalo o valor está ...
		for (i = 0; i < local_n; i++)
		{
			for (j = 0; j < M; j++)
			{
				if (j == M-1)
				{
					if (local_data[i] >= j*local_m && local_data[i] <= j*local_m + local_m)
						local_count[j]++;
				}
				else
				{
					if (local_data[i] >= j*local_m && local_data[i] < j*local_m + local_m)
						local_count[j]++;
				}
			}
		}
		// Somar as somas de cada histograma e mandar para o processo 0
		MPI_Reduce(local_count,count,M,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		finish = MPI_Wtime();
		local_elapsed = finish - start;
		
		free(local_data);
		free(local_count);
		free(count);
	}

	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	if (my_rank == 0)
		printf("Time elapsed: %e (ms)\n",elapsed);	
	MPI_Finalize();
	return 0;
}