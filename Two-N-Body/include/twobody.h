#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

// Macros
const int DIM = 3;
const int X = 0;
const int Y = 1;
const int Z = 2;

typedef double vect_t[DIM];

// Constants
const double G = 6.673e-11; // (m^3 / kg . s^2)

void buildModel (int argc, char *argv[]);
void Get_args (int argc, char *argv[]);
void allocMemory ();
//TwoBody* newTwoBody (int argc, char *argv[]);
//void solveModel (TwoBody *m);
//void Compute_force (Particle *p, vect_t *forces, int q, int n);
//void Compute_pos_vel (Particle *p, vect_t *forces, double dt, int q, int n);
//void readInput (TwoBody *m);
//void printParticle (const Particle *p, int N);
void printError (const char msg[]);
//void writeVTK (int step, Particle *p, int n);
void Usage (const char pName[]);