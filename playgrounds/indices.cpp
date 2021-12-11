// toy program to figure out indicing of the matrices within each node

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
void initialize(int argc, char* argv[]) {

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
            // rows [0, ..., remainder-1] will have a size of edge_count_/node_count_ + 1
            // the remaining rows will have a size of edge_count_/node_count_
            lastLargeRow = remainder - 1; 
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

vector<int> receiveNumbers() {
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
    int rank, master_row, master_column, row, column;
    if (f.is_open()) {
        while (f >> line) {
            n1 = stoull(line);
            f >> line;
            n2 = stoull(line);
            master_row = 0;   // the row wrt the entire graph into which the element goes into 
            temp = n1;     // will contain the row of the subsquare that it goes to 
            for (auto& sz : sizes) {
                if (sz > temp) {
                    break;
                }
                temp -= sz;
                ++master_row;
            }
            row = temp;
            master_column = 0;   // the column wrt the entire graph into which the element goes into 
            temp = n2;
            for (auto& sz : sizes) {
                if (sz > temp) {
                    break;
                }
                temp -= sz;
                ++master_column;
            }
            column = temp;
            rank = (master_row + master_column) % node_count_;
            cout << "n1: " << n1 
                 << " n2: " << n2
                 << " goes to process #: " << rank
                 << " arr[" << master_row << ']'
                 <<  '[' << row << ']' 
                 << '[' << column << ']'
                 << " && " 
                 << " arr[" << master_column << ']'
                 << '[' << column << ']'
                 <<  '[' << row << ']' 
                 << '\n';
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
    // else {
    //     nums = receiveNumbers();
    // }
    cleanup();
}