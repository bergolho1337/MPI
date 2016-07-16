// Programa paralelo que calcula o produto escalar entre dois vetores: v3 = v1.v2
// E calcula o vetor resultante da multiplicação dele por um escalar: v1 = v1*scalar
// Neste caso tratamos o caso em que TAM % comm_sz != 0

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

const int TAM = 12;

int main() 
{
	int my_rank, comm_sz, i, local_n;
	int *displ;						// Vetor de displacements entre os elementos que serão mandados
	int *scount;						// Vetor com as quantidades que cada processo vai receber
	int rem;						// Resto da divisão entre tamanho do vetor original e o numero de processos
	int sum;						// Variável que controla quantos elementos já foram enviados, usado no displ	
	double v1[TAM], v2[TAM], v3[TAM], dot_prod;
	double *local_v1, *local_v2, *local_v3;
	double scalar;
	
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
	
	displ = (int*)malloc(sizeof(int)*comm_sz);
	scount = (int*)malloc(sizeof(int)*comm_sz);
	rem = TAM % comm_sz;
	local_n = TAM/comm_sz;
	sum = 0;

	// Calcula quanto devo mandar para cada processo
	for (i = 0; i < comm_sz; i++)
	{
		scount[i] = local_n;
		if (rem > 0)
		{
			scount[i]++;
			rem--;
		}
		displ[i] = sum;					// Quantos elementos já enviei
		sum = sum + scount[i];				// Incrementa para o processo que vem depois saber onde o último terminou
	}

	if (my_rank == 0)
	{
		// Leitura
		printf("Lendo vetor 1:\n");
		for (i = 0; i < TAM; i++)
			scanf("%lf",&v1[i]);		
		printf("Lendo vetor 2:\n");
		for (i = 0; i < TAM; i++)
			scanf("%lf",&v2[i]);
		printf("Lendo escalar:\n");
		scanf("%lf",&scalar);

		dot_prod = 0;
		local_v1 = (double*)malloc(sizeof(double)*scount[my_rank]);
		local_v2 = (double*)malloc(sizeof(double)*scount[my_rank]);
		local_v3 = (double*)malloc(sizeof(double)*scount[my_rank]);		
		// Distribuir o vetor em local_n partes ao longo dos processos
		MPI_Scatterv(v1,scount,displ,MPI_DOUBLE,local_v1,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Scatterv(v2,scount,displ,MPI_DOUBLE,local_v2,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
				
		for (i = 1; i < comm_sz; i++)
			MPI_Send(&scalar,1,MPI_DOUBLE,i,i,MPI_COMM_WORLD);
	
		// Realizar as operações em cada parte 
		for (i = 0; i < scount[my_rank]; i++)
		{
			local_v3[i] = local_v1[i]*local_v2[i];
			local_v1[i] = local_v1[i]*scalar;
			local_v2[i] = local_v2[i]*scalar;
		}
		
		// Juntar as partes no processo 0
		MPI_Gatherv(local_v1,scount[my_rank],MPI_DOUBLE,v1,scount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Gatherv(local_v2,scount[my_rank],MPI_DOUBLE,v2,scount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Gatherv(local_v3,scount[my_rank],MPI_DOUBLE,v3,scount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);	
		printf("\nVetor 3:\n");		
		for (i = 0; i < TAM; i++)
		{
			printf("%e\n",v3[i]);
			dot_prod += v3[i];		
		}

		// Imprime o resultado
		printf("\n\nVetor 1:\tVetor 2:\n");
		for (i = 0; i < TAM; i++)
			printf("%e\t%e\n",v1[i],v2[i]);
		printf("\n\nResultado do produto escalar entre v1 e v2:\n%e\n",dot_prod);
	}
	else
	{
		local_v1 = (double*)malloc(sizeof(double)*scount[my_rank]);
		local_v2 = (double*)malloc(sizeof(double)*scount[my_rank]);
		local_v3 = (double*)malloc(sizeof(double)*scount[my_rank]);
		// Receber as partes do vetor
		MPI_Scatterv(v1,scount,displ,MPI_DOUBLE,local_v1,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Scatterv(v2,scount,displ,MPI_DOUBLE,local_v2,scount[my_rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Recv(&scalar,1,MPI_DOUBLE,0,my_rank,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		// Realizar as operações em cada parte 
		for (i = 0; i < scount[my_rank]; i++)
		{
			local_v3[i] = local_v1[i]*local_v2[i];
			local_v1[i] = local_v1[i]*scalar;
			local_v2[i] = local_v2[i]*scalar;
		}
		
		// Juntar para cada processo suas partes contendo o resultado, enviando para o processo 0 todos os pedaços 	
		MPI_Gatherv(local_v1,scount[my_rank],MPI_DOUBLE,v1,scount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Gatherv(local_v2,scount[my_rank],MPI_DOUBLE,v2,scount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
		MPI_Gatherv(local_v3,scount[my_rank],MPI_DOUBLE,v3,scount,displ,MPI_DOUBLE,0,MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
} 
