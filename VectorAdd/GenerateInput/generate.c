#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

int main (int argc, char *argv[])
{
    char filename[100];
    uint N = atoi(argv[1]);
    sprintf(filename,"in%u",N);
    FILE *in = fopen(filename,"w+");
    fprintf(in,"%u\n",N);
    for (uint i = 0; i < N; i++)
        fprintf(in,"%d %d %d %d %d %d\n",i,i,i,i+10,i+10,i+10);
    fclose(in);
}