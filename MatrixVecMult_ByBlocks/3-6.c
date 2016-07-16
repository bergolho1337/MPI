#ifdef __cplusplus
   extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

typedef struct {
   int n;
   int* data;
} SquareMatx;

static SquareMatx* Allocate(int n);
static SquareMatx* Generate(int n, int range);
static int* GenerateVector(int n, int range);
static void Desallocate(SquareMatx* matx);
static int* Partition(SquareMatx* matx, const int size);
static int* PartitionVector(int* vec, int n, const int size);

static void Printm(SquareMatx* matx);
static void Printv(int* vec, int n);

int main(int argc, char* argv[])
{
   const char debug = (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'd');
	int my_rank, comm_sz, divisions;
	double start, finish, local_elapsed, elapsed;

	MPI_Init (&argc, &argv);
   MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &comm_sz);

   divisions = sqrt(comm_sz);

   if(my_rank == 0) {
      int order;
      printf("Digite a ordem (potencia de 2):\t> ");
      scanf("%d", &order);
      // compartilha a orderm com os demais processos
      MPI_Bcast(&order, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // Gera uma matriz randomica
      srand(time(0));
      SquareMatx* matx = Generate(order, 100);
      if(!matx) { printf("Process 0 Error: not alloc matrix\n"); exit(EXIT_FAILURE); }
      // Gera um vetor randomico
      int* vector = GenerateVector(order, 100);
      if(!vector) { printf("Process 0 Error: not alloc vector\n"); exit(EXIT_FAILURE); }

      /// Entrada
      if(debug) {
         printf("M:\n"); Printm(matx);
         printf("V = "); Printv(vector, order);
      }

      // Calcula a ordem de cada submatriz
      int size = order / sqrt(comm_sz);
      // Particiona a matriz e envia cada parte a um processo
      int i, j;

      start = MPI_Wtime();

      int* myBuffer = Partition(matx, size);
      if(!myBuffer) { MPI_Finalize(); exit(EXIT_FAILURE); }

      for(i = 1; i < comm_sz; i++) {
         int* buffer = Partition(matx, size);
         MPI_Send(buffer, size * size, MPI_INT, i, i, MPI_COMM_WORLD);
      }

      int* myVector = PartitionVector(vector, order, size);
      if(!myVector) { MPI_Finalize(); exit(EXIT_FAILURE); }

      for(i = 1; i < comm_sz; i++) {
         int* buffer = PartitionVector(vector, order, size);
         MPI_Send(buffer, size, MPI_INT, i, i+comm_sz, MPI_COMM_WORLD);
      }

      for(i = 0; i < order; i++)
         vector[i] = 0;
      for(i = 0; i < size; i++)
         for(j = 0; j < size; j++)
            vector[i] += myBuffer[j + i * size] * myVector[j];

      int msgNumber = (comm_sz - 1) * size;
      MPI_Status* status = (MPI_Status*) malloc(sizeof(MPI_Status));
      for(i = 0; i < msgNumber; i++){
         int partial;
         MPI_Recv(&partial, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
         // Tag é igual ao id da linha multiplicada
         vector[status->MPI_TAG] += partial;
      }

      /// Saida
      if(debug){
	
         printf("M x V = "); Printv(vector, order);
      }		

      free(myBuffer);
      free(vector);
      Desallocate(matx);
   } else {
      int order, size;
      MPI_Bcast(&order, 1, MPI_INT, 0, MPI_COMM_WORLD);

      size = order / divisions;

      // Aloca os buffer
      int* myBuffer = (int*) malloc(size * size * sizeof(int));
      int* myVector = (int*) malloc(size * sizeof(int));
      if(!myBuffer || !myVector) {
         printf("Process %d Error: not alloc myBuffer or myVector\n", my_rank);
         MPI_Finalize();
         exit(EXIT_FAILURE);
      }
      // Recebe a submatriz e o subvetor
      MPI_Recv(myBuffer, size * size, MPI_INT, 0, my_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(myVector, size, MPI_INT, 0, my_rank + comm_sz, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      int i, j;
      for(i = 0; i < size; i++){
         int partial = 0;
         for(j = 0; j < size; j++) {
            partial += myBuffer[j + i * size] * myVector[j];
         }
         int idLinha = (my_rank/divisions) * size + i;
         MPI_Send(&partial, 1, MPI_INT, 0, idLinha, MPI_COMM_WORLD);
      }

      free(myBuffer);
      free(myVector);
   }
	finish = MPI_Wtime();
	local_elapsed = finish-start;
	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);      
	if (my_rank == 0)      	
		printf("\nTotal elapsed: %e (s)\n",elapsed);
	
	MPI_Finalize();

	return 0;
}

/// Matrix
SquareMatx* Allocate(int n) {
   SquareMatx* ret = (SquareMatx*) malloc(sizeof(SquareMatx));
   if(!ret) return NULL;
   ret->data = (int*) malloc(n * n * sizeof(int));
   if(!ret->data) return NULL;
   ret->n = n;
   return ret;
}

SquareMatx* Generate(int n, int range) {
   SquareMatx* ret = Allocate(n);
   if(!ret) return NULL;

   int i, n2 = n * n;
   for(i = 0; i < n2; i++){
      int signal = pow(-1, rand() % 2);
      ret->data[i] = signal * rand() % range;
   }
   return ret;
}

int* Partition(SquareMatx* matx, const int size) {
   // parâmetros iniciais, controla o partionamento
   static int start_row = 0;
   static int start_col = 0;
   // Casos de erro
   if(!matx || !matx->data) {
      printf("Partition Error: matx == NULL || matx->data == NULL\n");
      return NULL;
   }
   if(start_row + size > matx->n || start_col + size > matx->n) {
      printf("Partition Error: start + size > size(matx)\n");
      return NULL;
   }
   // Aloca o buffer e o preenche
   int* buffer = (int*) malloc(size * size * sizeof(int));
   if(!buffer) {
      printf("Partition Error: buffer was not allocated\n");
      return NULL;
   }

   int row, col, i = 0;
   for(row = start_row; row < (start_row + size); row++){
      for(col = start_col; col < (start_col + size); col++, i++){
         buffer[i] = matx->data[col + row * matx->n];
      }
   }
   // Atualiza as posições iniciais
   start_col += size;
   if(start_col >= matx->n) {
      start_col = 0;
      start_row += size;
   }
   if(start_row >= matx->n)
      start_row = start_col = 0;

   return buffer;
}

void Desallocate(SquareMatx* matx) {
   if(!matx) return;
   free(matx->data);
   free(matx);
}

/// Vector
int* GenerateVector(int n, int range) {
   int* vec = (int*) malloc(n * sizeof(int));
   if(!vec) return NULL;

   int i;
   for(i = 0; i < n; i++){
      int signal = pow(-1, rand() % 2);
      vec[i] = signal * rand() % range;
   }
   return vec;
}

int* PartitionVector(int* vec, int n, const int size) {
   static int start = 0;
   // Casos de erro
   if(!vec) {
      printf("PartitionVec Error: vec == NULL\n");
      return NULL;
   }
   if(start + size > n) {
      printf("PartitionVec Error: start + size >= size(vec)\n");
      return NULL;
   }
   // Aloca o buffer e o preenche
   int* buffer = (int*) malloc(size * sizeof(int));
   if(!buffer) {
      printf("PartitionVec Error: buffer was not allocated\n");
      return NULL;
   }
   int pos, i = 0;
   for(pos = start; pos < start + size; pos++, i++){
      buffer[i] = vec[pos];
   }
   // Atualiza posição inicial
   start += size;
   if(start >= n) start = 0;

   return buffer;
}

/// Print's
void Printm(SquareMatx* matx) {
   if(!matx || !matx->data) return;

   int i, n2 = matx->n * matx->n;
   for(i = 0; i < n2; i++){
      printf("| %02d ", matx->data[i]);
      if((i + 1) % matx->n == 0) printf("|\n");
   }
}
void Printv(int* vec, int n) {
   if(!vec) return;

   int i;
   printf("[ %02d", vec[0]);
   for(i = 1; i < n; i++){
      printf(", %02d", vec[i]);
   }
   printf(" ]\n");
}

#ifdef __cplusplus
   }
#endif // __cplusplus
