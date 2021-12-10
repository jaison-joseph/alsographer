#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
// #include "x86intrin.h"

void printMatrix(int* c, const int size) {
   printf("\n ------------------- \n");
    for (int i = 0 ; i < size ; i++) {
        printf("[ ");
        for (int j = 0 ; j < size ; j++) {
            printf("%d, ", c[i*size+j]);
        }
        printf(" ]\n");    
    } 
}

void printArray(int* c, const int size) {
    printf("\n[ ");
    for (int j = 0 ; j < size ; ++j) {
        printf("%d, ", c[j]);
    }
    printf(" ]\n");    
}

int readFile(const int n_nodes, int* adjacencyMatrix, int* hotVector, int* counts) {
    FILE* f = fopen("edgelist.txt", "r");
    if (f == 0) {
        return -1;
    }
    int n1, n2, num = 0;
    char ch;
    while ((ch = getc(f)) != EOF) {
        // space check 
        if (ch == ' ') {
            n1 = num;
            num = 0;
        }
        // line feed check
        else if (ch == 10) {
            n2 = num;
            printf("\n %d %d", n1, n2);
            adjacencyMatrix[n1*n_nodes + n2] = 1;
            adjacencyMatrix[n2*n_nodes + n1] = 1;
            hotVector[n1] = 1;
            hotVector[n2] = 1;
            counts[n1] += 1;
            counts[n2] += 1;
            num = 0;
        }
        else {
            num = num*10 + (ch-'0');
        }   
    }
    n2 = num;
    printf("\n %d %d", n1, n2);
    adjacencyMatrix[n1*n_nodes + n2] = 1;
    adjacencyMatrix[n2*n_nodes + n1] = 1;
    hotVector[n1] = 1;
    hotVector[n2] = 1;
    counts[n1] += 1;
    counts[n2] += 1;
    fclose(f);
    return 1;
}

/*
updates the 2D matrix adjacencyMatrix [n_nodes * n_nodes] and the hotVector[n_nodes] to
reflect the changes brought about by deleting the node at index node_num
*/
void deleteNode(int *adjacencyMatrix, int *hotVector, int *counts, int n_nodes, int node_num) {
    // if there are no nodes to delete since there are no connected nodes, we can return 
    if (counts[node_num] == 0) {
        return;
    }
    for (int i = 0 ; i < n_nodes ; ++i) {
        if (adjacencyMatrix[node_num*n_nodes + i] == 1) {
            adjacencyMatrix[node_num*n_nodes + i] = 0;
            adjacencyMatrix[i*n_nodes + node_num] = 0;
            --counts[i];
            --counts[node_num];
            if (counts[i] == 0) {
                hotVector[i] = 0;
            }
            // if there are no remaining connected nodes 
            if (counts[node_num] == 0) {
                hotVector[node_num] = 0;
                return;
            }
        }
    }
    // this check can be done at the end 
    if (counts[node_num] == 0) {
        hotVector[node_num] = 0;
    }
}

// creates a connection between the nodes node_num_1 and node_num_2
int addNode(int *adjacencyMatrix, int *hotVector, int *counts, int n_nodes, int node_num_1, int node_num_2) {
    adjacencyMatrix[node_num_1*n_nodes + node_num_2] = 1;
    adjacencyMatrix[node_num_2*n_nodes + node_num_1] = 1;
    counts[node_num_1] += 1;
    counts[node_num_2] += 1;
    hotVector[node_num_1] = 1;
    hotVector[node_num_2] = 1;
}

//prints the adjacency matrix, the hotVector and the counts vector 
void printAll(int *adjacencyMatrix, int *hotVector, int *counts, int n_nodes) {
    // printing the adjacency matrix 
    printf("\n printing the adjacency matrix");
    printMatrix(adjacencyMatrix, n_nodes);

    // printing the hot vector 
    printf("\n printing the hot vector");
    printArray(hotVector, n_nodes);

    // printing the count vector 
    printf("\n printing the count vector");
    printArray(counts, n_nodes);
}

int main() {
    // the number of nodes in this 
    const int n_nodes = 4;
    
    // to store return value of functions
    int returnVal;
    
    // declaring and initializing the adjacency matrix and the hot vector 
    int *adjacencyMatrix = (int*) malloc(n_nodes * n_nodes * sizeof(int));
    int *hotVector = (int*) malloc(n_nodes * sizeof(int));
    int *counts = (int*) malloc(n_nodes * sizeof(int));

    // setting to 0
    memset(adjacencyMatrix, 0, n_nodes*n_nodes*sizeof(int));
    memset(hotVector, 0, n_nodes*sizeof(int));
    memset(counts, 0, n_nodes*sizeof(int));

    // reading the edgelist file 
    readFile(n_nodes, adjacencyMatrix, hotVector, counts);

    // print the 3 data structures 
    printAll(adjacencyMatrix, hotVector, counts, n_nodes);

    deleteNode(adjacencyMatrix, hotVector, counts, n_nodes, 0);
    deleteNode(adjacencyMatrix, hotVector, counts, n_nodes, 0);

    printAll(adjacencyMatrix, hotVector, counts, n_nodes);

    // free-ing the adjacency matrix and the hotVector
    free(adjacencyMatrix);
    free(hotVector);
    free(counts);
}