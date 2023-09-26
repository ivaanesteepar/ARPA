#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4

int main(int argc, char** argv) {
    int rank, size, i, j;
    int A[N][N], B[N][N], C[N][N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != N) {
        printf("El número de procesos debe ser igual a N (el tamaño de las matrices).\n");
        MPI_Finalize();
        return 1;
    }

    double startTime, endTime;

    if (rank == 0) {
        // Generar las matrices A y B en el proceso 0
        srand(time(NULL));
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

    // Iniciar el tiempo justo antes de la difusión (broadcast)
    startTime = MPI_Wtime();

    // Distribuir las matrices A y B entre los procesos
    for (i = 0; i < N; i++) {
        MPI_Bcast(A[i], N, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(B[i], N, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Calcular la suma de las matrices
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }

    // Terminar el tiempo justo después de la operación
    endTime = MPI_Wtime();

    if (rank == 0) {
        // Mostrar la matriz C (suma de A y B)
        printf("\nMatriz C (suma de A y B):\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }

        // Mostrar el tiempo de ejecución
        printf("\nTiempo de ejecución: %f segundos\n", endTime - startTime);
    }

    MPI_Finalize();
    return 0;
}
