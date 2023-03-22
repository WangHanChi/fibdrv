#!/usr/bin/env python3

import subprocess
import numpy as np
import matplotlib.pyplot as plt
import sys

runs = 10

temp_str = []
argv = [sys.argv[0]]
argv.extend(sys.argv[1].split("+"))
cmp_num = len(argv)
algorithm_name = ""
print(sys.argv)
print(len(sys.argv))
print(argv)
print(cmp_num)
print("------------")
algorithm = ['original_iter', 'fdoubling_rec', 'fdoubling_iter', 
             'bn_fib', 'bn_fib_fdoubling', 'bn_fib_fdoubling_v1']

def data_processing(data_set, n):
    list = []
    for i in range(500):
        data_set[0][i].sort()
        check = round(runs*0.03)
        for j in range(1, check):
            data_set[0][i][j] = 0
            data_set[0][i][n - j] = 0
        list.append(sum(data_set[0][i]) / len(data_set[0][i]-check))
    return list

if __name__ == "__main__":
    comp_porc = subprocess.run('cd ..', shell = True)
    # for i in range(runs):
    #     comp_porc = subprocess.run('sudo taskset 0x1 ./plotsrc', shell = True)

    # data_name = sys.argv[1]
    # print(data_name)

    for i in range(1, cmp_num):
        var_name = 'output' + str(i)
        print(var_name)
        data_name = argv[i]
        print(data_name)
        exec(f"{var_name} = np.loadtxt('record/{data_name}', dtype='float').T")
    
    for i in range(1, cmp_num):
        var_name = 'output' + str(i)
        source_name = 'Ys' + str(i)
        exec(f"{source_name} = []")
        exec(f"{source_name}.append({var_name})")
    
    for i in range(1, cmp_num):
        result_name = 'X' + str(i)
        list_name = 'Y' + str(i)
        source_name = 'Ys' + str(i)
        exec(f"{result_name} = []")
        exec(f"{list_name} = []")
        exec(f"{list_name} = data_processing({source_name}, runs)")
        exec(f"{result_name} = list(range(1, runs+1))")

    # Y = data_processing(Ys, runs)
    # X = list(range(1, 501))


    fig, ax = plt.subplots(1, 1, sharey = True)

        

    ax.set_title('Fibonacci Performance Comparision', fontsize = 16)
    ax.set_xlabel(r'$n_{th}$ fibonacci', fontsize = 16)
    ax.set_ylabel('time (ns)', fontsize = 16)
    # print("PASS!")
    x = np.linspace(0, 499, 500) 

    for i in range(1, cmp_num):
        list_name = 'Y' + str(i)
        label_name = algorithm[int(argv[i])]
        exec(f"ax.plot(x, {list_name}, marker = '+', markersize = 7, label = '{label_name}')")
        # ax.plot(X, Y, marker = '+', markersize = 7, label = 'kernel')
        algorithm_id = int(argv[i])
        algorithm_name += algorithm[algorithm_id]



    ax.legend(loc = 'upper left')

    file_name = ''
    for i in range(1, cmp_num):
        # exec(f"comp_porc = subprocess.run('rm -rf {argv[i]}.txt', shell = True)")
        if i == cmp_num:
            break
        file_name += (argv[i] + "_")

    exec(f"plt.savefig('plot/{algorithm_name}.png')")
    # plt.savefig('plot/plot.png') 
    plt.show()
    # comp_porc = subprocess.run('rm -rf data.txt', shell = True)
