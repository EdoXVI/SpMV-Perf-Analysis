import os

csr = "perf stat -o csr.txt -e LLC-load-misses,LLC-store-misses ./bin/spmv "
coo = "perf stat -o coo.txt -e LLC-load-misses,LLC-store-misses ./bin/spmv "

nnz = list()

perfCSR = list()
perfCOO = list()

opCSR = list()
opCOO = list()



#number of iteration of the algorithm
num_iter = 1000

#open file with all dataset names
path = "data/names.txt"
f = open(path, "r")

#actual script
while(True):
    #read name of dataset
    dataset = f.readline().strip()
    if(len(dataset) == 0):
        break

    #############################################
    #-----     CSR

    #build flags and launch the shell command
    flag = "-s " + dataset  + " -i " + str(num_iter) + " -a 1"
    stream = os.popen(csr + dataset + flag)
    outputLines = stream.readlines()

    #parse the output from stdin
    line = outputLines[0].strip()
    values = line.split()
    nnz.append(int(values[0]))
    perfCSR.append(float(values[1]))

    #parse the output from perf (I don't know why I can't parse it directly from python, so I had to save them in a file and then parse it from there)
    f1 = open('csr.txt', "r")
    lines = f1.readlines()
    op = (int(lines[5].strip().split()[0].replace('.', '')))*64/num_iter
    opCSR.append(nnz[-1]/op)
    print(nnz[-1]/op)

    #############################################
    #-----     COO

    #build flags and launch the shell command
    flag = "-s " + dataset  + " -i " + str(num_iter) + " -a 2"
    stream = os.popen(coo + dataset + flag)
    outputLines = stream.readlines()

    #parse the output from stdin
    line = outputLines[0].strip()
    values = line.split()
    perfCOO.append(float(values[1]))

    #parse the output from perf (I don't know why I can't parse it directly from python, so I had to save them in a file and then parse it from there)
    f1 = open('coo.txt', "r")
    lines = f1.readlines()
    op = (int(lines[5].strip().split()[0].replace('.', '')))*64/num_iter
    opCOO.append(nnz[-1]/op)
    print(nnz[-1]/op)




