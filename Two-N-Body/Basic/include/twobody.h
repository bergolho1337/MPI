#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <mpi.h>

// Macros
const int DIM = 3;
const int X = 0;
const int Y = 1;
const int Z = 2;

typedef double vect_t[DIM];

// Constants
const double G = 6.673e-11; // (m^3 / kg . s^2)


// Functions
void Build_Model (int argc, char *argv[]);
void Free_Model ();
void Init_MPI (int argc, char *argv[]);
void Get_Args ();
void Alloc_Memory ();
void Print_Error (const char msg[]);
void Read_Init_Cond ();
void Print_Particles ();
void Solve_Model ();
void Write_VTK (int step);



