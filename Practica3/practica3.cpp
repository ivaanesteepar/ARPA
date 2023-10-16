#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    // Inicialización de variables
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 6; // Tamaño de los vectores
    int* x = NULL;
    int* y = NULL;

    if (rank == 0) {
        srand(time(NULL));
        x = (int*)malloc(n * sizeof(int));
        y = (int*)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            x[i] = rand() % 10; //valores de x
            y[i] = rand() % 10; //valores de y
        }
        printf("Vector x: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", x[i]);
        }
        printf("\n");

        printf("Vector y: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", y[i]);
        }
        printf("\n");
    }

    int local_x, local_y;
    MPI_Scatter(x, 1, MPI_INT, &local_x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, 1, MPI_INT, &local_y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_product = local_x * local_y;
    int global_product;

    MPI_Reduce(&local_product, &global_product, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Producto escalar: %d\n", global_product);
        free(x);
        free(y);
    }

    MPI_Finalize();
    return 0;
}
