
with open("edgelist.txt", "w") as f:
    for i in range(10):
        for j in range(10):
            f.write(str(i) + " " + str(j) + '\n')