#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 4 // Número de filas
#define N 2 // Número de columnas

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numProcesos = M * N;

    if (size != numProcesos) {
        printf("El numero de procesos debe de ser igual a MxN (%d x %d)\n", M, N);
        MPI_Finalize();
        return 1;
    }

    // Crear una topología cartesiana 2D (por ejemplo, 2x2)
    int dims[2] = { 4, 2 }; // 4 filas y 2 columnas
    int periods[2] = { 0, 0 }; // No se aplican condiciones de períodos
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

    // Obtener coordenadas del proceso actual en la topología cartesiana
    int coords[2];
    MPI_Cart_coords(cart_comm, rank, 2, coords);
    int row = coords[0]; // Fila actual
    int col = coords[1]; // Columna actual

    // Definir las matrices A y B
    int local_A[M][N];
    int local_B[M][N];
    int local_sum[M][N];


    // Inicializar las matrices A y B con valores aleatorios
    srand(time(NULL) + rank);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            local_A[i][j] = rand() % 50; // Números aleatorios entre 0 y 49
            local_B[i][j] = rand() % 50; // Números aleatorios entre 0 y 49
        }
    }

    // Realizar la suma de las matrices locales de cada proceso
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            local_sum[i][j] = local_A[i][j] + local_B[i][j];
        }
    }

    // Mostrar las matrices locales de cada proceso
    printf("\nProceso %d - Matriz A:\n", rank);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", local_A[i][j]);
        }
        printf("\n");
    }

    printf("\nProceso %d - Matriz B:\n", rank);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", local_B[i][j]);
        }
        printf("\n");
    }

    // Mostrar la matriz local sumada en cada proceso
    printf("\nProceso %d - Matriz Suma Local:\n", rank);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", local_sum[i][j]);
        }
        printf("\n");
    }

    // Sumar todas las matrices local_sum usando MPI_Reduce
    int global_sum[M][N];
    MPI_Reduce(local_sum, global_sum, M * N, MPI_INT, MPI_SUM, 0, cart_comm);

    // Mostrar la matriz suma global desde el proceso 0
    if (rank == 0) {
        printf("\nMatriz Suma Global:\n");
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", global_sum[i][j]);
            }
            printf("\n");
        }
    }

    // Liberar la topología cartesiana y finalizar MPI
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}
