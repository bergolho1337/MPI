MPICC = mpicc
MPICC_FLAGS = -g -Wall

mpi_mat_vec: main.c mpi_mat_vec.c
	$(MPICC) $(MPICC_FLAGS) -o mpi_mat_vec $^

clear:
	rm -r mpi_mat_vec
