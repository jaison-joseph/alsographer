
with open("edgelist.txt", "w") as f:
    for i in range(10):
        for j in range(5):
            f.write(str(i) + " " + str(j) + '\n')

world_rank_ = 0
indices_to_connect = []  #will 
num_processe = 4

'''
def sendEdges():
    if world_rank == 0:
        with open("edgelist.txt") as f:
            i, j = f.readline.split(' ')
            rank, packet = getProcessIndices(i, j) 
            MPI_Send(
                packet,
                5,      # packet size 
                MPI_INT,
                rank,
                0,
                MPI_COMM_WORLD
            )
        # now that we have sent out all the packets, we need to signal the other processes to stop
        # if a worker process receives these special values, it will stop execution and return
        packet[0] = -1
        packet[1] = -1
        packet[2] = -1
        packet[3] = -1
        packet[4] = -1
        for i in range(1, num_processes):
            MPI_Send(
                packet,
                5,      # packet size 
                MPI_INT,
                i,
                0,
                MPI_COMM_WORLD
            )
    # if 'this' is a worker thread
    else:
        packet = [0, 0, 0, 0, 0]
        while(True):
            MPI_Recv(
                packet,
                5,
                MPI_INT,
                0, 
                0,
                MPI_COMM_WORLD,
                &status
            )
            # if the process received a terminate signal known through these special values, terminate
            if (packet[0] == packet[1] == packet[2] == packet[3] ==  -1):
                break
             #updating the graph
            arr[packet[0]][packet[2]][packet[3]] = 1
            arr[packet[1]][packet[3]][packet[2]] = 1
'''