with open("edgelist.txt", "w") as f:
    for i in range(10):
        for j in range(10):
            if i == j or i+j==9:
                continue
            f.write(str(i) + " " + str(j) + '\n')

