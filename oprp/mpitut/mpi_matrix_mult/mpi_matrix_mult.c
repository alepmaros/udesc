/*
 *  Exemplo de programa para multiplicacao de matrizes em paralelo.
 *  Para fins de simplificacao, as matrizes devem ser quadradas.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <mpi.h>
#include "matrix.h"

#define MASTER 0        // id do mestre
#define MSG_TAG 0       // tag das mensagens

int n;

// Envia matriz 'm' para processo 'dest'.
void mpi_send_matrix(matrix_t *m, int dest)
{
   int position = 0;
   size_t tamanhoBuffer = (sizeof(double) * m->rows * m->cols) + (sizeof(int) * 2);
   void *output = (void*) malloc(tamanhoBuffer);

   MPI_Pack(&m->rows, 1, MPI_INT, output, tamanhoBuffer, &position, MPI_COMM_WORLD);
   MPI_Pack(&m->cols, 1, MPI_INT, output, tamanhoBuffer, &position,  MPI_COMM_WORLD);
   MPI_Pack(&m->data[0][0],  m->rows * m->cols, MPI_DOUBLE, output, tamanhoBuffer, &position, MPI_COMM_WORLD);
   MPI_Send(output, position, MPI_PACKED, dest, MSG_TAG, MPI_COMM_WORLD);
   
}

// Envia um bloco da matriz 'm' para processo 'dest'.
// Atencao: so funciona para blocos contendo linhas completas.
void mpi_send_submatrix(matrix_t *m, block_t part, int dest)
{
   int rows = part.row_end - part.row_start;
   int cols = part.col_end - part.col_start;
   assert(cols == m->cols);

   int position = 0;
   size_t tamanhoBuffer = (sizeof(double) * rows * cols) + (sizeof(int) * 2);
   void *output = (void*) malloc(tamanhoBuffer);

   MPI_Pack(&rows, 1, MPI_INT, output, tamanhoBuffer, &position, MPI_COMM_WORLD);
   MPI_Pack(&cols, 1, MPI_INT, output, tamanhoBuffer, &position,  MPI_COMM_WORLD);
   MPI_Pack(&m->data[part.row_start][0], rows * cols, MPI_DOUBLE, output, tamanhoBuffer, &position, MPI_COMM_WORLD);
   MPI_Send(output, position, MPI_PACKED, dest, MSG_TAG, MPI_COMM_WORLD);
 
}

// Recebe matriz do processo 'source'.
matrix_t *mpi_recv_matrix(int source)
{
   int position = 0;
   size_t tamanhoBuffer = (sizeof(double) * n * n) + (sizeof(int) * 2);
   void *input = (void*) malloc(tamanhoBuffer);

   MPI_Status status;
   matrix_t *m;
   int rows, cols;
   
   MPI_Recv(input, tamanhoBuffer, MPI_PACKED, source, MSG_TAG, MPI_COMM_WORLD, &status);
   MPI_Unpack(input, tamanhoBuffer, &position, &rows, 1, MPI_INT, MPI_COMM_WORLD);
   MPI_Unpack(input, tamanhoBuffer, &position, &cols, 1, MPI_INT, MPI_COMM_WORLD);
   m = matrix_create(rows, cols);
   MPI_Unpack(input, tamanhoBuffer, &position, &m->data[0][0], rows*cols, MPI_DOUBLE, MPI_COMM_WORLD);
   return m;
}


// Recebe um bloco da matriz 'm' calculado pelo processo 'source'.
void mpi_recv_submatrix(matrix_t *m, block_t part, int source)
{
   MPI_Status status;
   int rows, cols;
   
   int position = 0;
   size_t tamanhoBuffer = (sizeof(double) * n * n) + (sizeof(int) * 2);
   void *input = (void*) malloc(tamanhoBuffer);

   MPI_Recv(input, tamanhoBuffer, MPI_PACKED, source, MSG_TAG, MPI_COMM_WORLD, &status);
   MPI_Unpack(input, tamanhoBuffer, &position, &rows, 1, MPI_INT, MPI_COMM_WORLD);
   MPI_Unpack(input, tamanhoBuffer, &position, &cols, 1, MPI_INT, MPI_COMM_WORLD);
   MPI_Unpack(input, tamanhoBuffer, &position, &m->data[part.row_start][0], rows*cols, MPI_DOUBLE, MPI_COMM_WORLD);
   
   assert(cols == m->cols);
}





// Funcao executada pelos processos trabalhadores
void matrix_mult_slave()
{
   matrix_t *A = mpi_recv_matrix(MASTER);
   matrix_t *B = mpi_recv_matrix(MASTER);
   matrix_t *C = matrix_multiply(A, B);
   mpi_send_matrix(C, MASTER);
   matrix_destroy(A);
   matrix_destroy(B);
   matrix_destroy(C);
}

// Funcao de multiplicacao executada pelo processo mestre
matrix_t *matrix_mult_master(matrix_t *A, matrix_t *B, int ntasks)
{

   int i;
   block_t *part;
   matrix_t *C;
   
   assert(A->cols == B->rows);

   C = matrix_create(A->rows, B->cols);

   // Particiona matriz em 'ntasks' partes
   part = matrix_row_partition(A, ntasks);

   // Envia dados para trabalhadores
   for (i = 0; i < ntasks-1; i++) {
      mpi_send_submatrix(A, part[i], i + 1);
      mpi_send_matrix(B, i + 1);
   }
   
   // Mestre calcula uma parte do produto
   matrix_block_multiply(A, B, C, &part[ntasks-1]);

   // Recebe dados dos trabalhadores
   for (i = 0; i < ntasks-1; i++) {
      mpi_recv_submatrix(C, part[i], i + 1);
   }
   partition_destroy(part);
   return C;
}





int main(int argc, char **argv)
{
   //int n;
   int rc;
   int taskid, ntasks;
   double start_time, end_time;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
   MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

   if ((argc != 2)) {
      printf("Uso: mpirun -np <nprocs> %s <ordem da matriz quadrada>\n", 
             argv[0]);
      MPI_Abort(MPI_COMM_WORLD, rc);
      exit(1);
   }

   n = atoi(argv[1]);

   if (taskid == MASTER) {  // Codigo executado pelo processo mestre

      

      // Cria matrizes
      matrix_t *A = matrix_create(n, n);
      matrix_randfill(A);
      matrix_t *B = matrix_create(n, n);
      matrix_fill(B, 1.);
      matrix_t *C;

      // Calcula C = A * B, medindo o tempo
      start_time = MPI_Wtime();
      C = matrix_mult_master(A, B, ntasks);
      end_time = MPI_Wtime();

      // Mostra estatisticas da execucao
      printf("%d %f\n", ntasks, end_time - start_time);
      fflush(stdout);

      matrix_destroy(A);
      matrix_destroy(B);
      matrix_destroy(C);

   } else { // Codigo executado pelos processos trabalhadores

      matrix_mult_slave();
   }

   MPI_Finalize();
   return 0;
}
