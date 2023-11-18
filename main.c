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

    MPI_Barrier(MPI_COMM_WORLD);

    double start_comm_time = MPI_Wtime();

    MPI_Bcast(array, count, MPI_INTEGER, 0, MPI_COMM_WORLD);
	
    double end_comm_time = MPI_Wtime();

    const int wstart = (rank) * count / size;
    const int wend = (rank + 1) * count / size;

    MPI_Barrier(MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    
    for (int i = wstart; i < wend; i++)
    {
        if (array[i] > lmax) { lmax = array[i]; }
    }

    MPI_Reduce(&lmax, &max, 1, MPI_INTEGER, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (!rank) { 
        printf("Execution time: %f seconds\n", end_time - start_time);
        printf("Communication time: %f seconds\n", end_comm_time - start_comm_time);
    }

    free(array);

    MPI_Finalize();

    return 0;
}
