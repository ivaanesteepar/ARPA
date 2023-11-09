#include <mpi.h>
#include <stdio.h>
#define NUM 6

int main(int argc, char** argv) {

	//ARRAYS DE TIPO ENTERO PARA ESCRIBIR Y LEER LOS DATOS
	int lista[NUM];
	int leido[NUM];
	int rank;
	MPI_File file;

	// CHAR * CREADOS PARA USO POSTERIOR EN LAS FUNCIONES DE MPI
	char native[7] = "native";
	char text[5] = "text";

	// RELLENA LA LISTA CON LOS RANGOS
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	for (int i = 0; i < NUM; i++) {
		lista[i] = rank;
	};

	//PROCESO DE ESCRITURA
	MPI_File_open(MPI_COMM_WORLD, text, MPI_MODE_CREATE | MPI_MODE_RDWR,
		MPI_INFO_NULL, &file);
	MPI_File_set_view(file, NUM * sizeof(int) * rank, MPI_INT, MPI_INT, native,
		MPI_INFO_NULL);
	MPI_File_write_at(file, NUM * sizeof(int) * rank, lista, NUM, MPI_INT,
		MPI_STATUS_IGNORE);

	MPI_File_close(&file);

	//PROCESO DE LECTURA
	MPI_File_open(MPI_COMM_WORLD, text, MPI_MODE_CREATE | MPI_MODE_RDWR,
		MPI_INFO_NULL, &file);
	MPI_File_set_view(file, NUM * sizeof(int) * rank, MPI_INT, MPI_INT, native,
		MPI_INFO_NULL);
	MPI_File_read_at(file, NUM * sizeof(int) * rank, &leido, NUM, MPI_INT,
		MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//IMPRESIÓN POR PANTALLA DE LOS DATOS LEIDOS POR EL PROCESO
	printf("Proceso número %d ha leido los siguientes datos:\n", rank);
	fflush(stdout);
	for (int i = 0; i < NUM; i++) {
		printf("%d ", leido[i]); fflush(stdout);
	};
	
	printf("\n"); fflush(stdout);
	MPI_Finalize();
	return 0;
}
