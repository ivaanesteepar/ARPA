#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
	bool flag;
	int rank, tam, comm;
	MPI_Status estado;
	srand(time(NULL));
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm);
	if (rank == 0) {
		printf("Introduzca el tamaño de la matriz: ");
		fflush(stdout);
		scanf_s("%d", &tam);
	}

	//ENVIO DE TAMAÑO
	MPI_Bcast(&tam, 1, MPI_INT, 0, MPI_COMM_WORLD); //SE ENVIA EL TAMAÑO DE LA MATRIZ DESDE EL PROCESO 0 A TODOS LOS DEMAS PROCESOS
	//RESERVAS DE MEMORIA:
	int** MatrixA = NULL, ** MatrixB = NULL; // SE ASIGNA MEMORIA PARA LA MATRIZ A Y B EN TODOS LOS PROCESOS
	if (rank == 0) {
		MatrixA = (int**)malloc(tam * sizeof(int*));
		for (int i = 0; i < tam; i++) {
			MatrixA[i] = (int*)malloc(tam * sizeof(int));
		}
	}
	MatrixB = (int**)malloc(tam * sizeof(int*));
	for (int i = 0; i < tam; i++) {
		MatrixB[i] = (int*)malloc(tam * sizeof(int));
	}
	if (rank == 0) {
		//MatrixA
		for (int i = 0; i < tam; i++) {
			for (int j = 0; j < tam; j++) {
				MatrixA[i][j] = rand() % 10;
			}
		}
		//MatrixB
		for (int i = 0; i < tam; i++) {
			for (int j = 0; j < tam; j++) {
				MatrixB[i][j] = rand() % 10;
			}
		}
		//IMPRESIÓN DE MATRICES
		printf("Matrix A: \n");
		fflush(stdout);
		for (int i = 0; i < tam; i++) {
			for (int j = 0; j < tam; j++) {
				printf("%d ", MatrixA[i][j]);
				fflush(stdout);
			}
			printf("\n");
			fflush(stdout);
		}
		printf("\nMatrix B: \n");
		fflush(stdout);
		for (int i = 0; i < tam; i++) {
			for (int j = 0; j < tam; j++) {
				printf("%d ", MatrixB[i][j]);
				fflush(stdout);
			}
			printf("\n");
			fflush(stdout);
		}
	}
	for (int i = 0; i < tam; i++) {
		MPI_Bcast(MatrixB[i], tam, MPI_INT, 0, MPI_COMM_WORLD); //SE TRANSMITE CADA FILA DE LA MATRIZ B DESDE EL PROCESO 0 A TODOS LOS DEMAS PROCESOS
	}
	double time1 = MPI_Wtime();
	//RESERVA DE ESPACIO
	int* espacio;
	espacio = (int*)malloc(tam * sizeof(int));
	if (rank == 0) {
		//RESERVA DE ESPACIO para enviadas
		int* espacioA = (int*)malloc(comm * sizeof(int));
		int cont = 0;
		for (; cont < comm - 1; cont++) {
			if (cont < tam) {
				MPI_Send(MatrixA[cont], tam, MPI_INT, cont + 1, 0, MPI_COMM_WORLD); //EL PROCESO 0 ENVÍA LAS FILAS DE LA MATRIZ A A TODOS LOS PROCESOS
				espacioA[cont + 1] = cont;
			}
			else {
				MPI_Send(MatrixA[0], tam, MPI_INT, cont + 1, 1, MPI_COMM_WORLD);
			}
		} for (int i = 0; i < tam; i++) {
			MPI_Recv(espacio, tam, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &estado); //LOS DISTINTOS PROCESOS RECIBEN LAS FILAS DE LA MATRIZ A
			for (int j = 0; j < tam; j++) {
				MatrixA[espacioA[estado.MPI_SOURCE]][j] = espacio[j];
			} if (cont < tam) {
				MPI_Send(MatrixA[cont], tam, MPI_INT, estado.MPI_SOURCE, 0, MPI_COMM_WORLD);
				espacioA[estado.MPI_SOURCE] = cont;
				cont++;
			}
		} for (int i = 1; i < comm; i++) {
			//KILL DE DATOS
			MPI_Send(MatrixA[0], tam, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	}
	if (rank != 0) { 
		//RESERVA DE DATOS
		int* espacioB;
		espacioB = (int*)malloc(tam * sizeof(int));
		flag = true;
		while (flag) {
			MPI_Recv(espacio, tam, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &estado); //2º EL PROCESO 0 RECIBE LOS RESULTADOS PARCIALES
			if (estado.MPI_TAG == MPI_TAG_UB) {
				flag = false;
			}
			for (int i = 0; i < tam; i++) {
				int sum = 0;
				for (int j = 0; j < tam; j++) {
					sum = sum + MatrixB[j][i] * //1º CADA PROCESO DIFERENTE DE 0 REALIZA LA MULTIPLICACION DE LAS MATRICES PARA LAS FILAS RECIBIDAS
						espacio[j];
				}
				espacioB[i] = sum;
			}
			MPI_Send(espacioB, tam, MPI_INT, 0, estado.MPI_TAG, MPI_COMM_WORLD); //ENVIA EL RESULTADO AL PROCESO 0
		}
	}
	if (rank == 0) {
		//CALCULO DE TIEMPO
		double time2 = MPI_Wtime();
		double time3 = time2 - time1;
		//Imprimimos los resultados
		printf("\nTime: %g\n", time3);
		printf("\nResult: \n");
		for (int i = 0; i < tam; i++) {
			for (int j = 0; j < tam; j++) {
				printf("%d ", MatrixA[i][j]);
				fflush(stdout);
			}
			printf("\n");
			fflush(stdout);
		}
	}
	MPI_Finalize();
	return 0;
}
