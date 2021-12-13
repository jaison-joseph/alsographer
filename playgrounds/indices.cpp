// toy program to figure out indicing of the matrices within each node

#include <mpi.h>
#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <string>

using namespace std;

typedef unsigned long long int bigInt_c;

// MPI stuff 
int node_count_, world_rank_, num_workers_;
bigInt_c edge_count_;

// graph stuff 
vector<bigInt_c> sizes;     // a vector that keeps track of the width of each row 
vector<vector<vector<int>>> arr;   // the vector that stores the parts of the graph
vector<int> remainingNodes;
vector<int> nodeCount;

// function call status 
int returnValue;
string message;

// the MPI methods that need to be called before calling any other MPI methods
void initialize(int argc, char* argv[]) {

    MPI_Init(NULL, NULL);

    // getting the # processes and 'this' processes' rank
    MPI_Comm_size(MPI_COMM_WORLD, &node_count_);  
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);

    // !!!!!!!!!!!!!
    // because process #0 is exclusively for managing other threads
    // node_count_ -= 1; 

    // if we dont have the correct number of arguments
    if (argc != 2) {
        returnValue = -1;
        message = string(" expected 1 argument: number of edges in the graph");
        return;
    }
    // if we only have 1 core to work with 
    if (node_count_ < 2) {
        returnValue = -1;
        message = string("this program requires at least 2 cores to run");
        return;
    }
    num_workers_ = node_count_ - 1;
    returnValue = 0;
    edge_count_ = stoull(argv[1]);
    
    //deciding the distribution of edges across nodes
    if (node_count_ > edge_count_) {
        sizes = vector<bigInt_c> (1, node_count_);
    }
    else {
        sizes = vector<bigInt_c> (num_workers_, edge_count_/num_workers_);
        bigInt_c remainder = edge_count_ % num_workers_;
        for (int i = 0 ; i < remainder ; ++i) {
            sizes[i] += 1;
        }
    }
    if (world_rank_ == 0) {
        cout << "\n sizes: ";
        for (auto& i : sizes) {
            cout << i << ", ";
        }
    }

    // since we need to ensure that the sizes array is filled up all right
    // MPI_Barrier(MPI_COMM_WORLD);
    // allocating space and initializing memory of the distributed graph
    if (world_rank_ != 0) {
        int i, j;
        i = 0;
        j = world_rank_ - 1;
        for (int k = 0 ; k < num_workers_ ; ++k) {
            vector<vector<int>> foo;
            for (int it = 0; it < sizes[i] ; ++it) {
                foo.push_back(vector<int> (sizes[j], 0));
            }
            arr.push_back(foo);
            ++i;
            --j;
            if (j < 0) {
                j = num_workers_ - 1;
            }
        }
    }
    else {
        // nodeCount = vector<int> (, )
    }
}

// cleaning up MPI funciton call 
void cleanup() {
    MPI_Finalize();
}

// display memory layout across nodes
void displayLayout() {
    if (world_rank_ != 0) {
        cout << "\n" <<world_rank_ << " : ";
        for (auto& i : arr) {
            cout << '[' << i.size() << "][" << i[0].size() << "], ";
        }
    }
}

// display values stored across nodes
void displayValues() {
    if (world_rank_ != 0) {
        int count = 0;
        for (auto& i : arr) {
            cout << "\n world rank#: " <<world_rank_ << " | subMatrix#: " << count << '\n';
            count++;
            for (auto& j : i) {
                for (auto& k : j) {
                    cout << k << ", ";
                }            
                cout << "\n";
            }
            cout << "\n **************** \n";
        }
        cout << "\n ------------------------- ";
        cout << "\n ------------------------- \n";
    }
}

// returns a tuple containing rank and all indices to store an element 
// input: the indices of the element in the main graph (i -> n1, j -> n2)
void getIndices(bigInt_c n1, bigInt_c n2, int packet[5]) {
    bool flag1, flag2;
    int rank, master_row, master_column, row, column;
    flag1 = flag2 = true;
    master_row = 0;   
    master_column = 0;   
    for (auto& sz : sizes) {
        if (flag1) {
            if (n1 < sz) {
                flag1 = false;
            }
            else {
                n1 -= sz;
                ++master_row;
            }
        }
        if (flag2) {
            if (n2 < sz) {
                flag2 = false;
            }
            else {
                n2 -= sz;
                ++master_column;
                continue;
            }
        }
        if ((flag1 || flag2) == false) {
            break;
        }
    }
    row = n1;
    column = n2;
    rank = (master_row + master_column) % num_workers_;
    packet[0] = rank;
    packet[1] = master_row;
    packet[2] = master_column;
    packet[3] = row;
    packet[4] = column;
}

void receiveNumbers() {
    int packet[4];      //[master_row, master_column, row, column]
    MPI_Status status;
    while(true) {
        MPI_Recv(
            packet,
            4,
            MPI_INT,
            0, 
            0,
            MPI_COMM_WORLD,
            &status
        );
        // if (packet[0] == packet[1] == packet[2] == packet[3] ==  -1) {
        if (packet[0] == -1) {
            break;
        }
        arr[packet[0]][packet[2]][packet[3]] = 1;
        arr[packet[1]][packet[3]][packet[2]] = 1;
    }
}

/*
int MPI_Recv(
    void *buf, 
    int count, 
    MPI_Datatype datatype,
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status
);
*/

// p0 sends numbers to other processors 
// this function must only be run by the master thread; or just a single thread 
void sendNumbers() {
    ifstream f("edgelist.txt");
    string line;
    bigInt_c n1, n2, temp;
    int rank, master_row, master_column, row, column;
    int result[5];
    if (!f.is_open()) {
        message = string("could not open file 'edgelist.txt'");
        returnValue = -1;
        return;
    }
    while (f >> line) {
        n1 = stoull(line);
        f >> line;
        n2 = stoull(line);
        
        // output format: [rank, master_row, master_column, row, column]
        getIndices(n1, n2, result);
        rank = result[0];
        master_row = result[1];
        master_column = result[2];
        row = result[3];
        column = result[4];
        cout << "n1: " << n1 
             << " n2: " << n2
             << " goes to process #: " << rank
             << " arr[" << master_row << ']'
             << '[' << row << ']' 
             << '[' << column << ']'
             << " && " 
             << " arr[" << master_column << ']'
             << '[' << column << ']'
             << '[' << row << ']' 
             << '\n';
        MPI_Send(
            result+1, // packet + 1 since packet[0] is the rank 
            4, 
            MPI_INT,
            rank+1,
            0,
            MPI_COMM_WORLD
        );
    }
    cout << "\n finished \"sending the packets\" \n";
    // sending out the packets that signal the receiving processes to stop 
    result[0] = -1;
    result[1] = -1;
    result[2] = -1;
    result[3] = -1;
    for (int i = 1 ; i < node_count_ ; ++i) {
        //MPI_Send(it is done; wrap it up bois)
        MPI_Send(
            result,
            4, 
            MPI_INT,
            i,
            0,
            MPI_COMM_WORLD
        );
    }
}

/**
MPI_Send(
    void* buf,                 // data 
    int count,                  // size of the buffer (# of elements, not bytes)
    MPI_Datatype datatype,      // type of data in void* data
    int dest,                   // destination
    int tag,                    // idk man; just make sure the value is the same on send and receive end 
    MPI_Comm communicator);     // MPI_COMM_WORLD
 */

void addConnections(vector<pair<bigInt_c, bigInt_c>> newConnections) {
    if (world_rank_ == 0) { 
        bigInt_c n1, n2, temp;
        int rank, master_row, master_column, row, column;
        int packet[5];
        for (auto& ij : newConnections) {
            n1 = get<0>(ij);
            n2 = get<1>(ij);
            // [rank, master_row, master_column, row, column]
            getIndices(n1, n2, packet);
            // rank = packet[0];
            // master_row = packet[1];
            // master_column = packet[2];
            // row = packet[3];
            // column = packet[4];
            // cout << "n1: " << n1 
            //     << " n2: " << n2
            //     << " goes to process #: " << rank
            //     << " arr[" << master_row << ']'
            //     << '[' << row << ']' 
            //     << '[' << column << ']'
            //     << " && " 
            //     << " arr[" << master_column << ']'
            //     << '[' << column << ']'
            //     << '[' << row << ']' 
            //     << '\n';
            MPI_Send(
                packet+1,
                4, 
                MPI_INT,
                packet[0]+1,
                0,
                MPI_COMM_WORLD
            );
        }
        packet[0] = -1;
        packet[1] = -1;
        packet[2] = -1;
        packet[3] = -1;
        for (int i = 1 ; i < node_count_ ; ++i) {
            //MPI_Send(it is done; wrap it up bois)
            MPI_Send(
                packet,
                4, 
                MPI_INT,
                i,
                0,
                MPI_COMM_WORLD
            );
        }
    }
    else {
        receiveNumbers();
    }
}

int main(int argc, char* argv[]) {
    initialize(argc, argv);

    // connections to add to the graph 
    vector<pair<bigInt_c, bigInt_c>> newConnections;

    // connections to add to the graph 
    vector<pair<bigInt_c, bigInt_c>> oldConnections;

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

    // see the allocations 
    // displayLayout();

    // sending 'em out 
    // if (world_rank_ == 0) {
    //     sendNumbers();
    // }
    // else {
    //     receiveNumbers();
    // }

    // all the processes need to receive the numbers from p0
    // so, we place a barrier taht wait for all processes 
    MPI_Barrier(MPI_COMM_WORLD);

    // only process 0 gets the connections onto its vector 
    if (world_rank_ == 0) {
        newConnections.push_back(pair<bigInt_c, bigInt_c>(0, 1));
        newConnections.push_back(pair<bigInt_c, bigInt_c>(2, 3));
        newConnections.push_back(pair<bigInt_c, bigInt_c>(6, 8));
    }    

    addConnections(newConnections);

    // have we done it correctly?
    displayValues();

    // delete a connection 

    // mpi cleanup stuff 
    cleanup();
}