#include "../include/twobody.h"

int n;                      /* Total number of particles  */
int loc_n;                  /* Number of my particles     */
int n_steps;                /* Number of timesteps        */
int step;                   /* Current step               */
int loc_part;               /* Current local particle     */
int output_freq;            /* Frequency of output        */
double dt;                  /* Size of timestep           */
double t;                   /* Current Time               */
double* masses;             /* All the masses             */
vect_t* loc_pos;            /* Positions of my particles  */
vect_t* pos;                /* Positions of all particles */
vect_t* loc_vel;            /* Velocities of my particles */
vect_t* loc_forces;         /* Forces on my particles     */

vect_t* vel = NULL;         /* I/O velocity vector        */ 

MPI_Comm comm;              /* Communication MPI          */
MPI_Datatype mpi_vect_t;    /* MPI Datatype for vect_t    */
int my_rank, comm_sz;       /* Processes ids and size     */

void buildModel (int argc, char *argv[])
{
    // Initialize MPI and get number of processes
    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    Get_args(argc,argv);
    //allocMemory();
    //readInput();

    
    // Process 0 store the whole velocity vector for output purpose
    /*
    if (my_rank == 0)
    {
        vel = (vect_t*)malloc(sizeof(vect_t)*n);
        for (int i = 0; i < n; i++)
        {
            if (!scanf("%lf %lf %lf %lf %lf %lf %lf",&masses[i],&pos[i][X],&pos[i][Y],&pos[i][Z],\
            &vel[i][X],&vel[i][Y],&vel[i][Z])) printError("Reading input");
        }
    } 
    */   
}

void Get_args (int argc, char *argv[])
{
    if (my_rank == 0)
        if (!scanf("%lf %d %d %d",&dt,&n_steps,&output_freq,&n)) printError("Reading input");
    // Distribute the input along the processes
    MPI_Bcast(&n, 1, MPI_INT, 0, comm);
    MPI_Bcast(&n_steps, 1, MPI_INT, 0, comm);
    MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, comm);
    MPI_Bcast(&output_freq, 1, MPI_INT, 0, comm);

    // Check for errors on input
    if (n <= 0 || n_steps < 0 || dt <= 0)
    {
        if (my_rank == 0)
            Usage();
        MPI_Finalize();
        exit 1;
    }

    // DEBUG
    if (my_rank == 0)
    {
        printf("Number of particles = %d\n",n);
        printf("Number of steps = %d\n",n_steps);
        printf("dt = %lf\n",dt);
        printf("Output Frequency = %d\n",output_freq);
    }
}

void allocMemory ()
{
    // n should be divible by comm_sz
    loc_n = n / comm_sz;
    
    // Every process have these variables
    masses = (double*)malloc(sizeof(double)*n);
    pos = (vect_t*)malloc(sizeof(vect_t)*n);
    loc_forces = (vect_t*)malloc(sizeof(vect_t)*loc_n);
    loc_vel = (vect_t*)malloc(sizeof(vect_t)*loc_n);
    // Reference to my first particle
    loc_pos = pos + my_rank*loc_n;
}

void printError (const char msg[])
{
    fprintf(stderr,"[-] ERROR ! %s\n",msg);
    exit(EXIT_FAILURE);
}