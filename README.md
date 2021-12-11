# grapher

- A program to create massive graphs stored as an Adjacency matrix.
- Designed for multi-core systems.
- Built using openMPI.
- to use openMPI on bigred3, run ```module load openmpi/intel/4.0.4``` followed by ```module save```
- use ```mpicc``` as the compiler for c programs
- use ```mpicxx``` as the compiler for c++ programs


## development roadmap
- how to store the graph (done)
- how to figure out indices of an element in the graph when distributed (done)
- sending information to initialize and fill up the parts of the graph in each node
- distributing the hotVector and the countVector
- updating the graph (add and delete a connection)


## extras (if time permits)
- move all this into a c++ class to make it more reusable
