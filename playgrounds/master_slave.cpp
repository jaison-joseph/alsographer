#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

typedef unsigned long long int bigInt_c;
#define bigInt_MPI MPI_UNSIGNED_LONG_LONG

// MPI stuff 
int node_count_, world_rank_;
bigInt_c edge_count_;

// graph stuff 
vector<bigInt_c> sizes;     // a vector that keeps track of the width of each row 
bigInt_c lastLargeRow;

// function call status 
int returnValue;
string message;

// the MPI methods that need to be called before calling any other MPI methods
int initialize(int argc, char* argv[]) {

    MPI_Init(NULL, NULL);

    // getting the # processes and 'this' processes' rank
    MPI_Comm_size(MPI_COMM_WORLD, &node_count_);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);


    // setting up graph stuff 
    if (world_rank_ == 0) {
        if (argc != 2) {
            returnValue = -1;
            message = string(" expected 1 argument: number of edges in the graph");
            return;
        }
        edge_count_ = stoull(argv[1]);
        
        //deciding the distribution of edges across nodes
        if (node_count_ > edge_count_) {
            sizes = vector<bigInt_c> (1, node_count_);
        }
        else {
            sizes = vector<bigInt_c> (node_count_, edge_count_/node_count_);
            bigInt_c remainder = edge_count_ % node_count_;
            for (int i = 0 ; i < remainder ; ++i) {
                sizes[i] += 1;
            }
            lastLargeRow = remainder - 1; // rows [0, ..., remainder-1] will have a size of edge_count_/node_count_
        }
        cout << "\n sizes: ";
        for (auto& i : sizes) {
            cout << i << ", ";
        }

        returnValue = 0;
    }
}

// cleaning up MPI funciton call 
void cleanup() {
    MPI_Finalize();
}

// can multiple files read at the same time?
void readAFile() {
    ifstream f("input.txt");
    string line;
    if (f.is_open()) {
        while (f >> line) {
            cout << "\n" << world_rank_ << " : " << line << "\n";
        }
    }
}

vector<int>& receiveNumbers() {
    vector<int> numbers;
    while(true) {
        
    }
    return numbers;
}

// p0 sends numbers to other processors 
void sendNumbers() {
    ifstream f("edgelist.txt");
    string line;
    bigInt_c n1, n2, temp;
    int rank;
    // MPI_UNSIGNED_LONG_LONG == unsigned long long int 
    if (f.is_open()) {
        while (f >> line) {
            n1 = stoull(line);
            f >> line;
            n2 = stoull(line);
            if (n1 + n2 > (lastLargeRow+1) * sizes[0]) {
                
            }
        }
    }
    cout << "\n\n";
}

int main(int argc, char* argv[]) {
    initialize(argc, argv);
    // if the above function had a hiccup
    if (returnValue < 0) {
        // one process need only display the error message 
        if (world_rank_ == 0) {
            cout << message;
        }
        // every process needs to clean up
        cleanup();
        exit(0);
    }
    vector<int> nums;
    if (world_rank_ == 0) {
        sendNumbers();
    }
    else {
        nums = receiveNumbers();
    }
    cleanup();
}