// Example program that shows how to build a MPI type struct of heterogenous types
// And how to use it with communication functions

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Number of elements of the global array
const int n = 8;

// This structure has 1 integer and 3 double variables
// The order which the parameters are set matters
struct Point
{ 
    double x, y, z;         // 3d coordinates
    int id;                 // Identifier
}typedef Point;

int main ()
{
    int my_rank, comm_sz;
    int local_n;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    // Setup the MPI_Type for the Point structure
    MPI_Datatype pointType, types[2];
    // MPI_Aint is the return value of MPI_Type_extent, which gives the size of a type
    MPI_Aint offsets[2], extent;        
    int blockcounts[2];
    
    // The struct will be divided in two blocks
    // The first is the integer part and second is the double part
    
    // First part: a block consisting of 3 double
    offsets[0] = 0;
    types[0] = MPI_DOUBLE;
    blockcounts[0] = 3;

    // Second part: a block of 1 int 
    // Need to first figure offset by getting size of MPI_DOUBLE
    MPI_Type_extent(MPI_DOUBLE, &extent);
    offsets[1] = 3 * extent;
    types[1] = MPI_INT;
    blockcounts[1] = 1;

    // Now define a structured type and commit it
    MPI_Type_struct(2, blockcounts, offsets, types, &pointType);
    MPI_Type_commit(&pointType);

    local_n = n / comm_sz;
    Point *points = NULL;
    Point *local_points = (Point*)malloc(sizeof(Point)*local_n);
    if (my_rank == 0)
    {
        // Fill the structure with data
        points = (Point*)malloc(sizeof(Point)*n);
        for (int i = 0; i < n; i++)
        {
            points[i].id = i;
            points[i].x = i; points[i].y = i; points[i].z = i;
        }
        MPI_Scatter(points,local_n,pointType,local_points,local_n,pointType,0,MPI_COMM_WORLD);
    }
    else
        MPI_Scatter(points,local_n,pointType,local_points,local_n,pointType,0,MPI_COMM_WORLD);

    // Print the parts of each process
    sleep(my_rank*2);
    printf("Process %d has the points\n",my_rank);
    for (int i = 0; i < local_n; i++)
        printf("Point %d -- (%.1lf,%.1lf,%.1lf)\n",local_points[i].id,local_points[i].x,local_points[i].y,local_points[i].z);

    // Free the defined structured type
    MPI_Type_free(&pointType);
    MPI_Finalize();

    return 0;
}