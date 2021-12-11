#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

// MPI stuff 
int world_size_, world_rank_;

void initialize() {
    MPI_Init(NULL, NULL);

    // getting the # processes and 'this' processes' rank
    MPI_Comm_size(MPI_COMM_WORLD, &world_size_);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);
}

void cleanup() {
    MPI_Finalize();
}

void readAFile() {
    ifstream f("input.txt");
    string line;
    if (f.is_open()) {
        while (f >> line) {
            cout << "\n" << world_rank_ << " : " << line << "\n";
        }
    }
}

int main() {
    initialize();
    readAFile();
    cleanup();
}