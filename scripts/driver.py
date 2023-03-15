#!/usr/bin/env python3

import subprocess
import numpy as np
import matplotlib.pyplot as plt

runs = 5000


def data_processing(data_set, n):
    list = []
    for i in range(100):
        data_set[0][i].sort()
        check = round(runs*0.03)
        for j in range(1, check):
            data_set[0][i][j] = 0
            data_set[0][i][n - j] = 0
        list.append(sum(data_set[0][i]) / len(data_set[0][i]-check))
    return list

if __name__ == "__main__":
    Ys = []
    comp_porc = subprocess.run('cd ..', shell = True)
    for i in range(runs):
        comp_porc = subprocess.run('sudo taskset 0x1 ./client', shell = True)
    output = np.loadtxt('data.txt', dtype = 'float').T
    Ys.append(output)
    # print(Ys[0][3])
    Y = data_processing(Ys, runs)
    X = list(range(1, 101))

    fig, ax = plt.subplots(1, 1, sharey = True)
    ax.set_title('Fibonacci Performance - Fast Doubling Method', fontsize = 16)
    ax.set_xlabel(r'$n_{th}$ fibonacci', fontsize = 16)
    ax.set_ylabel('time (ns)', fontsize = 16)
    ax.plot(X, Y, marker = '+', markersize = 7, label = 'kernel')
    ax.legend(loc = 'upper left')
    plt.savefig('plot/plot.png') 
    plt.show()
    comp_porc = subprocess.run('rm -rf data.txt', shell = True)