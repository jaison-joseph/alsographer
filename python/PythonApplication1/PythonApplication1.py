
"""
How to use this application
python PythonApplication1.py <name of file with points> <name of file with edgeList>

"""

import re, sys
from itertools import repeat
from operator import add

def readInput(points, adjacencyMatrix, numberOfEdges, pointsFileName, edgesFileName):
    with open(pointsFileName) as f:
        for l in f:
            points.append(
                list(
                    map(
                        int,
                        (l.split(','))
                    )
                )
            )

    adjacencyMatrix += [[0 for i in range(len(points))] for i in range(len(points))]
    numberOfEdges += [0 for i in range(len(points))]
    with open(edgesFileName) as f:
        for l in f:
            indices = list(
                map(
                    int, l.split(' ')
                )
            )
            adjacencyMatrix[indices[0]][indices[1]] = 1
            adjacencyMatrix[indices[1]][indices[0]] = 1
            numberOfEdges[indices[0]] += 1
            numberOfEdges[indices[1]] += 1

def generateHotVector(points, adjacencyMatrix, hotVector):
    if len(hotVector) == 0:
        hotVector += [0 for i in range(len(points))]
    for i, row in enumerate(adjacencyMatrix):
        if 1 in row:
            hotVector[i] = 1
    # print(hotVector)

def deleteNode(index, points, adjacencyMatrix, hotVector, numberOfEdges):
    if index < 0 or index >= len(points):
        raise OutOBoundsException("provided index was out of bounds")
    # index of all the nodes connected to node at index index
    ones = [i for i,j in enumerate(adjacencyMatrix[index]) if j == 1]
    # updating numberOfEdges
    for x in ones:
        numberOfEdges[x]-=1
     # because we are deleting the node at index index
    numberOfEdges[index] = 0
    # index of nodes with 0 edges; used to update hotVector
    zeroes = [i for i, j in enumerate(numberOfEdges) if j == 0]
    # updating hotVector
    for x in zeroes:
        hotVector[x] = 0
    # updating adjacencyMatrix
    for x in adjacencyMatrix:
        x[index] = 0
    adjacencyMatrix[index] = [0 for i in range(len(points))]

if __name__ == "__main__":
    points = []
    adjacencyMatrix = []
    hotVector = []    #1 if atleast 1 endge, 0 if not, len = len(points)
    numberOfEdges = []
    readInput(points, adjacencyMatrix, numberOfEdges, sys.argv[1], sys.argv[2])
    generateHotVector(points, adjacencyMatrix, hotVector)
    print("points:", points)
    print("hotVector:", hotVector)
    print("numberOfEdges:", numberOfEdges)
    print("adjacencyMatrix:", adjacencyMatrix)
    deleteNode(0, points, adjacencyMatrix, hotVector, numberOfEdges)
    print("points:", points)
    print("hotVector:", hotVector)
    print("numberOfEdges:", numberOfEdges)
    print("adjacencyMatrix:", adjacencyMatrix)
