# grapher

- A program to create massive graphs stored as an Adjacency matrix.
- Designed for multi-core systems.
- Built using openMPI.
- to use openMPI on bigred3, run ```module load openmpi/intel/4.0.4``` followed by ```module save```
- use ```mpicc``` as the compiler for c programs
- use ```mpicxx``` as the compiler for c++ programs

## to run the application
- cd into the ```multi_core``` directory and run ```mpicxx -O3 main.cpp -o b```
- create a file called ```edgelist.txt``` that contains the connections to be made between the nodes
- run using ```srun -p debug -n <number of cores for the application> b <number of vertices in the graph>```
- for a demo run, run the ```generateEdgeList.py``` file followed by ```mpicxx -O3 main.cpp -o b``` and finally 
```srun -p debug -n 3 --time=00:12:00 b 10```
- modify the nodes that are removed by heading over to the main function in the main.cpp and append the indices of 
the vertices to be removed into the ```oldConnections``` vector before ```removeConnections()``` is called. 


## development roadmap
- how to store the graph (done)
- how to figure out indices of an element in the graph when distributed (done)
- sending information to initialize and fill up the parts of the graph in each node (done)
- distributing the hotVector and the countVector (done; there's a catch)
- updating the graph (add and delete a connection) (done)


## extras (if time permits)
- move all this into a c++ class to make it more reusable (coming soon)
