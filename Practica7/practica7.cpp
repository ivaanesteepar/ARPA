#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define N 5 //TAMAÑO DE LA MATRIZ

int main(int argc, char* argv[]) {
	int rank, tam, longitud, i, j;
	int tag = 1;
	int matriz[N][N];
	// FUNCIONES MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &tam);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	//MATRIZ SUPERIOR
	int vector_long_sup[N];
	MPI_Aint vector_despl_sup[N];
	MPI_Datatype vector_tipos[N];
	MPI_Datatype matriz_superior;
	int _long = N, _despl = 0;
	for (i = 0; i < N; i++) {
		vector_long_sup[i] = _long;
		vector_despl_sup[i] = _despl;
		_despl = _despl + (N + 1) * sizeof(int);
		_long = _long - 1;
		vector_tipos[i] = MPI_INT;
	}
	MPI_Type_struct(N, vector_long_sup, vector_despl_sup, vector_tipos, &matriz_superior);
	MPI_Type_commit(&matriz_superior);
	//MATRIZ INFERIOR
	int vector_long_inf[N];
	MPI_Aint vector_despl_inf[N];
	MPI_Datatype matriz_inferior;
	_despl = 0;
	for (i = 0; i < N; i++) {
		vector_despl_inf[i] = _despl;
		_despl = _despl + (N) * sizeof(int);
		vector_long_inf[i] = i + 1;
		vector_tipos[i] = MPI_INT;
	}
	MPI_Type_struct(N, vector_long_inf, vector_despl_inf, vector_tipos, &matriz_inferior);
	MPI_Type_commit(&matriz_inferior);
	//CREAMOS MATRIZ CON VALORES NULOS (0) PARA EL PROCESO 1 y 2
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			matriz[i][j] = 0;
		}
	}
	//PROCESO 0 GENERACIÓN DE MATRIZ CON NUMEROS ALEATORIOS USANDO rand()
	if (rank == 0) {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				matriz[i][j] = rand() % 10;
			}
		}
		//IMPRESIÓN POR PANTALLA DE MATRIZ INICIAL
		printf("\nMatriz proceso %d:\n", rank);
		fflush(stdout);
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				printf("%d ", matriz[i][j]);
			}
			printf("\n");
			fflush(stdout);
		}
		MPI_Send(&matriz, 1, matriz_superior, 1, tag, MPI_COMM_WORLD);
		MPI_Send(&matriz, 1, matriz_inferior, 2, tag, MPI_COMM_WORLD);
	}
	//PROCESO 1,MATRIZ TRIANGULAR SUPERIOR
	if (rank == 1) {
		printf("\nMatriz creada por proceso %d:\n", rank);
		fflush(stdout);
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				printf("%d ", matriz[i][j]);
			}
			printf("\n");
			fflush(stdout);
		}
		//RECIBIMOS MATRIZ DIAGONAL SUPERIOR DEL PROCESO 0
		MPI_Recv(&matriz, 1, matriz_superior, 0, tag, MPI_COMM_WORLD, &status);
		printf("\nMatriz recibida por proceso %d, hecha la diagonal superior:\n", rank);
		fflush(stdout);
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				printf("%d ", matriz[i][j]);
			}
			printf("\n");
			fflush(stdout);
		}
	}
	//PROCESO 2,MATRIZ TRIANGULAR INFERIOR
	if (rank == 2) {
		printf("\nMatriz creada por proceso %d:\n", rank);
		fflush(stdout);
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				printf("%d ", matriz[i][j]);
			}
			printf("\n");
			fflush(stdout);
		}
		//RECIBIMOS MATRIZ DIAGONAL INFERIOR DEL PROCESO 0
		MPI_Recv(&matriz, 1, matriz_inferior, 0, tag, MPI_COMM_WORLD, &status);
		printf("\nMatriz recibida por proceso %d, hecha la diagonal inferior:\n", rank);
		fflush(stdout);
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				printf("%d ", matriz[i][j]);
			}
			printf("\n");
			fflush(stdout);
		}
	}
	MPI_Finalize();
	return 0;
}
