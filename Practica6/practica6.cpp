#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    MPI_Status estado;
    int num[1], rank, i;
    long long int fact_result;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Proceso número 0, encargado de recibir el número y mostrar el resultado
    if (rank == 0)
    {
        do
        {
            printf("Introduzca el numero: ");
            fflush(stdout);
            scanf_s("%d", num);

            // Envío asíncrono al proceso de rango 1
            MPI_Request request_enviar;
            MPI_Isend(num, 1, MPI_INT, 1, 10, MPI_COMM_WORLD, &request_enviar);

            // Inicio de la recepción asíncrona del resultado
            MPI_Request request_recibir;
            MPI_Irecv(&fact_result, 1, MPI_LONG_LONG_INT, 1, 20, MPI_COMM_WORLD, &request_recibir);

            // Espera asíncrona hasta que la recepción se complete
            MPI_Wait(&request_recibir, &estado);

            if (*num > 0)
            {
                printf("El resultado del factorial de %d es: %lld\n", *num, fact_result);
            }

        } while (*num > 0); // Hasta que el valor de num no sea 0 se repite el proceso
    }
    // Proceso número 1, encargado de recibir el número, realizar el factorial y devolvérselo al proceso 0
    else if (rank == 1)
    {
        do
        {
            MPI_Recv(num, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &estado);

            if (*num > 0)
            {
                long long int fact1 = 1;
                for (i = 1; i <= *num; i++)
                {
                    // Operación factorial
                    fact1 = fact1 * i;
                }

                // Envío asíncrono a proceso 0
                MPI_Request request_envio;
                MPI_Isend(&fact1, 1, MPI_LONG_LONG_INT, 0, 20, MPI_COMM_WORLD, &request_envio);

                // Espera asíncrona hasta que el envío se complete
                MPI_Wait(&request_envio, &estado);
            }

        } while (*num > 0); // Hasta que el valor de num no sea 0 se repite el proceso
    }

    MPI_Finalize();
    return 0;
}
