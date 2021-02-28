import os

#open all dataset names
path = "data/names.txt"
f = open(path, "r")

#actual script
while(True):
    perf = "perf stat -e LLC-load-misses,LLC-store-misses "
    cmd = "./bin/spmv "
    dataset = f.readline()

    flag = "-s " + dataset  + " -i 10000 -a 1"
    if(len(dataset) == 0):
        break
    #stream = os.popen(perf + cmd + dataset + flag)
    #outputLines = stream.readlines()
    process = subprocess.Popen(
        ['perf', 'stat', '-e LLC-load-misses,LLC-store-misses', './bin/spmv','-s '+dataset, '-i 1000', '-a 1'], 
        stdout=subprocess.PIPE,
        universal_newlines=True)
    #print(outputLines[0])

    flag = "-s " + dataset  + " -i 1 -a 2"
    stream = os.popen(perf + cmd + dataset + flag)
    outputLines = stream.readlines()
    #print(outputLines[0])

# flag = "-s " + "nome Dataset" + "-i 1000 -a "



