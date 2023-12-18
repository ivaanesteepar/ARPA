#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int rank, comm, tam;
    float* MatrixA, * MatrixB, * MatrixRes;
    double time1, time2, time3;
    srand(time(NULL));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm);

    if (rank == 0) {
        char* param = argv[1];
        tam = atoi(param);
    }
    //el proceso 0 envia el tamaño de la matriz al resto de procesos
    MPI_Bcast(&tam, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //se asigna memoria para las matrices
    MatrixA = (float*)malloc(sizeof(float) * tam * tam);
    MatrixB = (float*)malloc(sizeof(float) * tam * tam);
    MatrixRes = (float*)malloc(sizeof(float) * tam * tam);

    if (rank == 0) {

        // llenamos con números aleatorios las matrices A y B
        for (int i = 0; i < tam * tam; ++i) {
            MatrixA[i] = (float)rand() / (float)(RAND_MAX);
        }

        for (int i = 0; i < tam * tam; ++i) {
            MatrixB[i] = (float)rand() / (float)(RAND_MAX);
        }
    }
    // el proceso 0 envia las matrices A y B
    MPI_Bcast(MatrixA, tam * tam, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(MatrixB, tam * tam, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // calculamos el tiempo antes de la multiplicación  
    time1 = MPI_Wtime();
    if (rank == 0) { // si el rango es 0
        for (int fila = 0; fila < tam; fila += comm - 1) {
            for (int fila_proceso = fila, num_proceso = 1; fila_proceso < tam && num_proceso < comm; ++fila_proceso, ++num_proceso) {
                MPI_Send(&fila_proceso, 1, MPI_INT, num_proceso, 0, MPI_COMM_WORLD);
            }
            // el proceso 0 recibe los resultados de los otros procesos
            for (int fila_proceso = fila, num_proceso = 1; fila_proceso < tam && num_proceso < comm; ++fila_proceso, ++num_proceso) {
                MPI_Recv(MatrixRes + fila_proceso * tam, tam, MPI_FLOAT, num_proceso, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        //se envía el flag de terminación al resto de procesos
        for (int num_proceso = 1; num_proceso < comm; ++num_proceso) {
            int flag_fin = -1;
            MPI_Send(&flag_fin, 1, MPI_INT, num_proceso, 0, MPI_COMM_WORLD);
        }
    }
    else { // si el rango es distinto de 0
        int flag2;
        float* filaResultado = (float*)malloc(sizeof(float) * tam);

        MPI_Recv(&flag2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        while (flag2 != -1) { // mientras no reciba el flag de terminación (-1), realiza la multiplicación
            // se multiplican las filas 
            for (int celda = 0; celda < tam; celda++) {
                float suma = 0;

                for (int i = 0; i < tam; ++i) {
                    suma += MatrixA[flag2 * tam + i] * MatrixB[i * tam + celda];
                }

                filaResultado[celda] = suma;
            }

            // se envía el resultado al proceso 0
            MPI_Send(filaResultado, tam, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

            // se recibe una nueva operación
            MPI_Recv(&flag2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        free(filaResultado);
    }

    //terminamos el tiempo después de la multiplicación
    time2 = MPI_Wtime();
    time3 = time2 - time1;

    if (rank == 0) {
        printf("\nTime: %f\n", time3); // tiempo en segundos
        fflush(stdout);
    }

    // liberamos la memoria de las matrices
    free(MatrixA);
    free(MatrixB);
    free(MatrixRes);

    MPI_Finalize();

    return 0;
}
