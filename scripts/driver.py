#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import numpy as np
import matplotlib.pyplot as plt
import sys

runs = 1000

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

    argv = [sys.argv[0]]
    argv.extend(sys.argv[1].split("+"))
    argc = len(argv)

    for n in range(1, argc):       
        Ys = []
        comp_porc = subprocess.run('cd ..', shell = True)

        for i in range(runs):
            # comp_porc = subprocess.run('sudo taskset 0x1 ./plotsrc', shell = True)
            exec(f"comp_porc = subprocess.run('sudo taskset 0x1 ./plotsrc {argv[n]}', shell = True)")
        
        exec(f"comp_porc = subprocess.run('mv -f {argv[n]} record/{argv[n]}', shell = True)")
        exec(f"output = np.loadtxt('record/{argv[n]}', dtype = 'float').T")
        Ys.append(output)
        # print(Ys[0][3])
        Y = data_processing(Ys, runs)
        X = list(range(1, 501))
        plt.ion()
        fig, ax = plt.subplots(1, 1, sharey = True)
        ax.set_title('Fibonacci Performance', fontsize = 16)
        ax.set_xlabel(r'$n_{th}$ fibonacci', fontsize = 16)
        ax.set_ylabel('time (ns)', fontsize = 16)
        algorithm_id = int(argv[n])
        exec(f"ax.plot(X, Y, marker = '+', markersize = 7, label = algorithm[algorithm_id])")
        ax.legend(loc = 'upper left')
        algorithm_name = algorithm[algorithm_id]
        exec(f"plt.savefig('plot/{algorithm_name}.png') ")
        plt.savefig('plot/.png')
        # plt.savefig('plot/plot.png') 
        # plt.show()
        plt.pause(0.1)
    plt.ioff()