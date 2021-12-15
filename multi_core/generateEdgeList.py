
with open("edgelist.txt", "w") as f:
    for i in range(2000):
        for j in range(2000):
            f.write(str(i) + " " + str(j) + '\n')