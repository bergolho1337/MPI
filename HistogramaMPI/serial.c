#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../timer.h"

const int n = 1024ull * 1024ull * 256ull;
const int max_value = 256;
//const int n = 12;
//const int max_value = 4;

void error (const char *msg)
{
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}

int* allocMemory (int size)
{
    int *buffer = (int*)calloc(size,sizeof(int));
    if (buffer == NULL)
        error("[-] ERROR! Cannot allocate memory!");
    return buffer;
}

void generateData (int *data)
{
    int i;
    for (i = 0; i < n; i++)
        data[i] = rand() % max_value;
}

void printData (int *data)
{
    int i;
    for (i = 0; i < n; i++)
        printf("%d\n",data[i]);
}

void printHistogram (int *count)
{
    int i;
    for (i = 0; i < max_value; i++)
        printf("%d = %d\n",i,count[i]);
}

void writeHistogram (int *count)
{
    FILE *out = fopen("count.dat","w+");
    int i;
    for (i = 0; i < max_value; i++)
        fprintf(out,"%d %d\n",i,count[i]);
    fclose(out);
}

void countHistogram (int *data, int *count)
{
    int i;
    for (i = 0; i < n; i++)
        count[data[i]]++;
}

int main (int argc, char *argv[])
{

	int *data;							
	int *count;				
	double start, finish, elapsed;

    data = allocMemory(n);
    count = allocMemory(max_value);

    generateData(data);
    #ifdef DEBUG
    printData(data);
    #endif

    GET_TIME(start);
    countHistogram(data,count);
    GET_TIME(finish);
    #ifdef DEBUG
    printHistogram(count);
    #endif

    elapsed = finish - start;
    printf("With n = %d\n",n);
    printf("Time elapsed = %.10lf s\n",elapsed);
    writeHistogram(count);

    free(data);
    free(count);
	
	return 0;
}
