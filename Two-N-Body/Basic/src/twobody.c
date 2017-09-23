#include "../include/twobody.h"
#include "../include/timer.h"

int my_rank, comm_sz;
MPI_Comm comm;
MPI_Datatype vect_mpi_t;

vect_t *vel = NULL;         /* Global velocity vector     */ 

int n;                      /* Total number of particles  */
int loc_n;                  /* Number of my particles     */
int n_steps;                /* Number of timesteps        */
int step;                   /* Current step               */
int loc_part;               /* Current local particle     */
int out_freq;               /* Frequency of output        */
double dt;                  /* Size of timestep           */
double t;                   /* Current Time               */
double* masses;             /* All the masses             */
vect_t* loc_pos;            /* Positions of my particles  */
vect_t* pos;                /* Positions of all particles */
vect_t* loc_vel;            /* Velocities of my particles */
vect_t* loc_forces;         /* Forces on my particles     */

void Build_Model (int argc, char *argv[])
{
    // Initialize MPI
    Init_MPI(argc,argv);

    // Process 0 reads and distributes the input arguments
    Get_Args();

    // Allocate memory
    Alloc_Memory();

    // Process 0 read initial condition
    Read_Init_Cond();

    // DEBUG
    //Print_Particles();

}

void Init_MPI (int argc, char *argv[])
{
    MPI_Init(&argc,&argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
}

void Get_Args ()
{
    if (my_rank == 0)
        if (!scanf("%lf %d %d %d",&dt,&n_steps,&out_freq,&n)) Print_Error("Reading input");

    MPI_Bcast(&dt,1,MPI_DOUBLE,0,comm);
    MPI_Bcast(&n_steps,1,MPI_INT,0,comm);
    MPI_Bcast(&out_freq,1,MPI_INT,0,comm);
    MPI_Bcast(&n,1,MPI_INT,0,comm);

    #ifdef DEBUG
    if (my_rank == 0) 
    {
      printf("n = %d\n", n);
      printf("n_steps = %d\n", n_steps);
      printf("delta_t = %e\n", dt);
      printf("output_freq = %d\n", out_freq);
   }
    #endif
}

void Alloc_Memory ()
{
    // 'n' should be divible by 'comm_sz'
    loc_n = n / comm_sz;

    // Allocate all the structures
    if (my_rank == 0) vel = (vect_t*)malloc(sizeof(vect_t)*n);
    masses = (double*)malloc(sizeof(double)*n);
    pos = (vect_t*)malloc(sizeof(vect_t)*n);
    loc_pos = pos + my_rank*loc_n;                      /* Store only the reference to my start particle */
    loc_vel = (vect_t*)malloc(sizeof(vect_t)*loc_n);
    loc_forces = (vect_t*)malloc(sizeof(vect_t)*loc_n);

    // Create a MPI type for a 3-d array of double
    MPI_Type_contiguous(DIM, MPI_DOUBLE, &vect_mpi_t);
    MPI_Type_commit(&vect_mpi_t);

}

void Read_Init_Cond ()
{
    if (my_rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (!scanf("%lf %lf %lf %lf %lf %lf %lf",&masses[i],&pos[i][X],&pos[i][Y],&pos[i][Z],\
                &vel[i][X],&vel[i][Y],&vel[i][Z])) Print_Error("Reading input");
        }
    }
    // Distribute the data to each process
    MPI_Bcast(masses,n,MPI_DOUBLE,0,comm);
    MPI_Bcast(pos,n,vect_mpi_t,0,comm);
    MPI_Scatter(vel,loc_n,vect_mpi_t,loc_vel,loc_n,vect_mpi_t,0,comm);
}

void Compute_Force (int loc_part)
{
    vect_t f_part;
    double mg;
    double len, len_3, fact;

    // Global index of the particle
    int part = loc_part + my_rank*loc_n;
    // Compute the forces related to this particle
    loc_forces[loc_part][X] = 0; loc_forces[loc_part][Y] = 0; loc_forces[loc_part][Z] = 0;
    
    for (int k = 0; k < n; k++)
    {
        if (k != part)
        {
            f_part[X] = pos[part][X] - pos[k][X];
            f_part[Y] = pos[part][Y] - pos[k][Y];
            f_part[Z] = pos[part][Z] - pos[k][Z];
            len = sqrt(f_part[X]*f_part[X] + f_part[Y]*f_part[Y] + f_part[Z]*f_part[Z]);
            len_3 = len*len*len;
            mg = -G*masses[part]*masses[k];
            fact = mg / len_3;
            f_part[X] *= fact;
            f_part[Y] *= fact;
            f_part[Z] *= fact;

            // Add force on local_force
            loc_forces[loc_part][X] += f_part[X];
            loc_forces[loc_part][Y] += f_part[Y];
            loc_forces[loc_part][Z] += f_part[Z];
        }
    }

}

void Compute_Pos_Vel (int loc_part)
{
    int part = loc_part + my_rank*loc_n;
    double fact = dt / masses[part];

    // Solve the linear system of ODEs
    loc_pos[loc_part][X] += dt * loc_vel[loc_part][X];
    loc_pos[loc_part][Y] += dt * loc_vel[loc_part][Y];
    loc_pos[loc_part][Z] += dt * loc_vel[loc_part][Z];

    loc_vel[loc_part][X] += fact * loc_forces[loc_part][X];
    loc_vel[loc_part][Y] += fact * loc_forces[loc_part][Y];
    loc_vel[loc_part][Z] += fact * loc_forces[loc_part][Z];

}

void Solve_Model ()
{
    double start, end, elapsed, total_elapsed;
    MPI_Barrier(comm);
    GET_TIME(start);
    // For each timestep
    for (int step = 0; step < n_steps; step++)
    {
        // Compute forces
        for (int loc_part = 0; loc_part < loc_n; loc_part++)
            Compute_Force(loc_part);
        
        // Update positions and velocities
        for (int loc_part = 0; loc_part < loc_n; loc_part++)
            Compute_Pos_Vel(loc_part);
        
        // Gather all parts of the position array. The MPI_IN_PLACE makes --> source = destination
        MPI_Allgather(MPI_IN_PLACE,loc_n,vect_mpi_t,pos,loc_n,vect_mpi_t,comm);
        
        // Write the output
        //if (step % out_freq == 0 && my_rank == 0)
        //    Write_VTK(step);
    }
    GET_TIME(end);
    elapsed = end - start;
    MPI_Reduce(&elapsed,&total_elapsed,1,MPI_DOUBLE,MPI_MAX,0,comm);
    
    if (my_rank == 0)
        printf("Time elapsed = %.10lf s\n",total_elapsed);
}

void Free_Model ()
{
    // Free memory
    MPI_Type_free(&vect_mpi_t);
    free(masses);
    free(pos);
    free(loc_vel);
    free(loc_forces);
    
    MPI_Finalize();
}

void Print_Error (const char msg[])
{
    fprintf(stderr,"[-] ERROR ! %s\n",msg);
}

void Print_Particles ()
{
    sleep(my_rank*2);
    for (int i = 0; i < loc_n; i++)
        printf("Proc %d: Particle %d -- Position (%lf,%lf,%lf) -- Velocity (%lf,%lf,%lf)\n",my_rank,i,loc_pos[i][X],loc_pos[i][Y],loc_pos[i][Z],\
                loc_vel[i][X],loc_vel[i][Y],loc_vel[i][Z]);
}

void Write_VTK (int step)
{
    char filename[500];
    sprintf(filename,"VTK/step%d.vtk",step);
    FILE *vtk = fopen(filename,"w+");
    fprintf(vtk,"# vtk DataFile Version 3.0\n");
    fprintf(vtk,"Vtk output\n");
    fprintf(vtk,"ASCII\n");
    fprintf(vtk,"DATASET POLYDATA\n");
    fprintf(vtk,"POINTS %d float\n",n);
    for (int i = 0; i < n; i++)
        fprintf(vtk,"%e %e %e\n",pos[i][X],pos[i][Y],pos[i][Z]);
    fprintf(vtk,"VERTICES %d %d\n",n,n*2);
    for (int i = 0; i < n; i++)
        fprintf(vtk,"1 %d\n",i);
    fclose(vtk);
}