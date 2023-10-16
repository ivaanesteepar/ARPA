#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inicialización de variables
    int rango, tamano, num;
    
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    if (tamano < 2) {
        printf("Se necesitan al menos 2 procesos para ejecutar este programa.\n");
        MPI_Finalize();
        return 1;
    }

    if (rango == 0) {
        // Proceso de rango 0
        scanf_s("%d", &num);
        MPI_Send(&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Proceso 0 envio el numero %d al proceso 1.\n", num);
    }
    else if (rango == 1) {
        // Proceso de rango 1
        MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Proceso 1 recibio el numero %d del proceso 0.\n", num);
    }

    MPI_Finalize();
    return 0;
}
