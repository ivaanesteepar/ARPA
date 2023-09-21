#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4

int main(int argc, char** argv) {
    int rank, size, i, j;
    int A[N][N], B[N][N], C[N][N];
    int local_A[N * N / 4], local_B[N * N / 4], local_C[N * N / 4];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != N) {
        printf("El número de procesos debe ser igual a N (el tamaño de las matrices).\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // Generar las matrices A y B en el proceso 0
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                A[i][j] = rand() % 10; // Rellenar con valores aleatorios
                B[i][j] = rand() % 10;
            }
        }
        printf("\nMatriz A:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("\nMatriz B:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }
   

    // Distribuir las matrices A y B entre los procesos
    MPI_Scatter(A, N * N / N, MPI_INT, local_A, N * N / N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, N * N / N, MPI_INT, local_B, N * N / N, MPI_INT, 0, MPI_COMM_WORLD);

    // Realizar la suma local de las matrices
    for (i = 0; i < N * N / N; i++) {
        local_C[i] = local_A[i] + local_B[i];
    }


    // Recopilar los resultados parciales en el proceso 0
    MPI_Gather(local_C, N * N / N, MPI_INT, C, N * N / N, MPI_INT, 0, MPI_COMM_WORLD);


    if (rank == 0) {
        // Mostrar la matriz C (suma de A y B)
        printf("\nMatriz C (suma de A y B):\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
