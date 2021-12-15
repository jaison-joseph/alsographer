
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
vector<bool> remainingNodes;        // basically our oneHotVector
vector<int> nodeCount;              // keeps a count of the number of ndoes connected to each edge?

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
        remainingNodes = vector<bool> (edge_count_, false);
        nodeCount = vector<int>(edge_count_, 0);
    }
}

// cleaning up MPI funciton call 
void cleanup() {
    MPI_Finalize();
}

// DEBUG
// display memory layout across nodes
void displayLayout() {
    if (world_rank_ != 0) {
        cout << "\n" <<world_rank_ << " : ";
        for (auto& i : arr) {
            cout << '[' << i.size() << "][" << i[0].size() << "], ";
        }
    }
}

// DEBUG 
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
    else {
        cout << "\n printing node count";
        for (auto& i : nodeCount) {
            cout << i << ", ";
        }
        cout << "\n printing node count";
        for (auto i : remainingNodes) {
            cout << (i ? "true" : "false") << ", ";
        }
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
    int sendPacket;
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
        // we send a 1 if we need to increment the count; 0 otherwise
        if (arr[packet[0]][packet[2]][packet[3]] == 0) {
            sendPacket = 1;
        }
        else {
            sendPacket = 0;
        }
        MPI_Send(
            &sendPacket,
            1, 
            MPI_INT,
            0,
            0,
            MPI_COMM_WORLD
        );
        arr[packet[0]][packet[2]][packet[3]] = 1;
        arr[packet[1]][packet[3]][packet[2]] = 1;
    }
}

// the function run by the workers to delete nodes; smart design first
void deleteNumbers() {
    //[master_row, master_column, row, column, width]
    int packet[5], index;
    int sendPacketSize = sizes[0];
    int* sendPacket = (int*)(malloc(sendPacketSize * sizeof(int)));      
    MPI_Status status;
    while(true) {
        MPI_Recv(
            packet,
            5,
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
        cout << "\n printing apcket: " << world_rank_ << ", ";
        for (int i = 0 ; i < 5 ; ++i) {
            cout << packet[i] << ", ";
        }
        index = 0;
        //[master_row, master_column, row, column, width]
        for (int j = 0 ; j < packet[4] ; ++j) {
            if (arr [packet[0]] [j] [packet[3]] == 1) {
                sendPacket[index] = j;
                index++;
            }
            cout << "\n deleting " 
             << " @ process #: " << world_rank_
             << " arr[" << packet[0] << ']'
             << '[' << j << ']'
             << '[' << packet[3] << ']' 
             << " && " 
             << " arr[" << packet[1] << ']'
             << '[' << packet[3] << ']'
             << '[' << j << ']';
            arr [packet[0]] [j] [packet[3]] = 0;
            arr [packet[1]] [packet[3]] [j] = 0;
        }
        if (index != sendPacketSize) {
            sendPacket[index] = -1;
        }
        MPI_Send(
            sendPacket,
            sendPacketSize, 
            MPI_INT,
            0,
            0,
            MPI_COMM_WORLD
        );
    }
    free(sendPacket);
}

// reads edgelist.txt and loads the numbers and sends to worker processes
void loadNumbers() {
    if (world_rank_ == 0) {
        ifstream f("edgelist.txt");
        string line;
        bigInt_c n1, n2, temp;
        int rank, master_row, master_column, row, column;
        int receivePacket;
        int result[5];
        MPI_Status status;
        if (!f.is_open()) {
            message = string("could not open file 'edgelist.txt'");
            returnValue = -1;
            return;
        }
        while (f >> line) {
            n1 = stoull(line);
            f >> line;
            n2 = stoull(line);

            // remainingNodes[n1] = true;
            // nodeCount[n1] += 1;
            // remainingNodes[n2] = true;
            // nodeCount[n2] += 1;
            
            // output format: [rank, master_row, master_column, row, column]
            getIndices(n1, n2, result);
            rank = result[0];
            master_row = result[1];
            master_column = result[2];
            row = result[3];
            column = result[4];
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
                result+1, // packet + 1 since packet[0] is the rank 
                4, 
                MPI_INT,
                rank+1,
                0,
                MPI_COMM_WORLD
            );
            MPI_Recv(
                &receivePacket,
                1,
                MPI_INT,
                rank+1, 
                0,
                MPI_COMM_WORLD,
                &status
            );
            if (receivePacket == 1) {
                nodeCount[n1] += 1;
                if (n1 != n2) {
                    nodeCount[n2] += 1;
                }
                remainingNodes[n1] = true;
                remainingNodes[n2] = true;
            }
            
        }
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
    else {
        receiveNumbers();
    }
}


void removeConnections(vector<bigInt_c>& oldConnections) {
    if (world_rank_ == 0) { 
        MPI_Status status;
        bigInt_c temp, index;
        int receivePacketSize = sizes[0];
        int* receivePacket = (int*)(malloc(receivePacketSize * sizeof(int)));
        int rank, start_row, start_column, row, column, end_row, end_column;
        // [rank, master_row, master_column, row, column, width]
        int packet[6]; 
        for (auto& n : oldConnections) {
            // if the node is already switched off; we dont need to do anything 
            if (remainingNodes[n] == false) {
                continue;
            }
            // [rank, master_row, master_column, row, column]
            index = 0;
            for (auto& sz: sizes) {
                getIndices(index, n, packet);
                packet[5] = sz;
                MPI_Send(
                    packet+1,
                    5, 
                    MPI_INT,
                    packet[0]+1,
                    0,
                    MPI_COMM_WORLD
                );
                MPI_Recv(
                    packet,
                    receivePacketSize,
                    MPI_INT,
                    packet[0]+1, 
                    0,
                    MPI_COMM_WORLD,
                    &status
                );
                for (int i = 0 ; i < sz ; ++i) {
                    if (packet[i] == -1) {
                        break;
                    }
                    // cout << "\n received request to decrement vertex: " << index + packet[i];
                    nodeCount[index + packet[i]] -= 1;
                    if (nodeCount[index + packet[i]] == 0) {
                        remainingNodes[index + packet[i]] = false;
                    }
                }
                index += sz;
            }
            nodeCount[n] = 0;
            remainingNodes[n] = false;
        }
        packet[0] = -1;
        packet[1] = -1;
        packet[2] = -1;
        packet[3] = -1;
        packet[4] = -1;
        for (int i = 1 ; i < node_count_ ; ++i) {
            //MPI_Send(it is done; wrap it up bois)
            MPI_Send(
                packet,
                5, 
                MPI_INT,
                i,
                0,
                MPI_COMM_WORLD
            );
        }
        free(receivePacket);
    }
    else {
        deleteNumbers();
    }
}


int main(int argc, char* argv[]) {
    
    // MPI and memory stuff 
    initialize(argc, argv);

    // connections to remove from the graph 
    vector<bigInt_c> oldConnections;

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

    // reading the numbers from file 
    loadNumbers();

    if (true) {
    // only process 0 gets the connections onto its vector 
        if (world_rank_ == 0) {
            // oldConnections.push_back(4);
            oldConnections.push_back(5);
            oldConnections.push_back(6);
            // oldConnections.push_back(7);
            // oldConnections.push_back(3);
            // oldConnections.push_back(8);
        }   

        // delete a connection 
        removeConnections(oldConnections);

        // have we done it correctly?
        MPI_Barrier(MPI_COMM_WORLD);
        cout << "\n after deleting the numbers \n";
        displayValues();
    }

    // mpi cleanup stuff 
    cleanup();
}