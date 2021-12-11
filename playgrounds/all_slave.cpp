// cant compile cpp :()

#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector> 

using namespace std;

int main(int argc, char* argv []) {
    MPI_Init(NULL, NULL);
    int world_size, world_rank;

    // getting the # processes and 'this' processes' rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("\n hello world from process #%d out of %d processes \n", world_rank, world_size);

    if (world_rank == 0) {
        printf("\n argc: %d", argc);
        for (int i = 0 ; i < argc ; ++i) {
            printf("\n argv[%d]: %s", i, argv[i]);
        }
        cout << "\ncan we compile c++?";

        vector<int> foo (5 ,10);
        for (auto& i : foo) {
            cout << "\n" << i << "\n";
        }
    }

    MPI_Finalize();
}