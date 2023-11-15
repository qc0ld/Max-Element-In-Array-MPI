#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
	int ret  = -1;
	int size = -1;
	int rank = -1;

	const int count = 100000000;
	const int random_seed = 920215;

	int* array = NULL; 
	int lmax = -1;
	int  max = -1; 

	ret = MPI_Init(&argc, &argv);
	if (!rank) { printf("MPI Init returned (%d);\n", ret); }

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	array = malloc(count * sizeof(int));

	if (!rank) {
		srand(random_seed);
		for (int i = 0; i < count; i++) { array[i] = rand(); }
	}

	MPI_Bcast(array, count, MPI_INTEGER, 0, MPI_COMM_WORLD);

	const int wstart = (rank) * count / size;
	const int wend = (rank + 1) * count / size;

	double start_time = MPI_Wtime();
	
	for (int i = wstart; i < wend;i++)
	{
		if (array[i] > lmax) { lmax = array[i]; }
	}

	double end_time = MPI_Wtime();

	MPI_Reduce(&lmax, &max, 1, MPI_INTEGER, MPI_MAX, 0, MPI_COMM_WORLD);

	ret = MPI_Finalize();

	if (!rank) { 
		printf("Threads: %d, Execution time: %f seconds\n", size, end_time - start_time);
		printf("Max element: %d\n", max);
	}

	free(array);

	return(0);
}