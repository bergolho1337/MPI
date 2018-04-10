#include <stdio.h>
#include <stdlib.h>

int is_sorted (double *v, int n)
{
	int i, tick = 1;
	for (i = 0; i < n-1; i++)
	{
		// Não está na ordem
		if (v[i] > v[i+1])
			tick = 0;
	}
	return (tick);
} 

int main ()
{
	double *V;
	int N, i;
	printf("Tamanho do vetor:\n");
	scanf("%d",&N);
	V = (double*)malloc(sizeof(double)*N);
	printf("Lendo vetor:\n");
	for (i = 0; i < N; i++)
		scanf("%lf",&V[i]);
	if (is_sorted(V,N) == 1)
		printf("[+] Ordenado!\n");
	else
		printf("[-] Desordenado!\n");
	free(V);
}
