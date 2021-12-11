#include <mpi.h>
#include <stdio.h>

int main() {
    MPI_Init(NULL, NULL);
    int world_size, world_rank;

    // getting the # processes and 'this' processes' rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("\n hello world from process #%d out of %d processes", world_rank, world_size);

    MPI_Finalize();
}