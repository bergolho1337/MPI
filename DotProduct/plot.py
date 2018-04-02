import os
import sys
import re
import numpy as np
from matplotlib import pyplot
from pylab import genfromtxt

mat = genfromtxt("times.dat")
n = ["1048576","2097152","4194304","8388608","16777216","33554432"]

for i in range(6):
    pyplot.plot(mat[:,0], mat[:,i+1], label = n[i], marker='o')

pyplot.grid()
pyplot.title("Tempos de execucao")
pyplot.xlabel("Processos")
pyplot.ylabel("Tempo (s)")  
pyplot.legend()
pyplot.show()

pyplot.clf()
speedup = np.zeros((2,6))
for i in range(6):
    serial = mat[0,i+1]
    for j in range(2):
        parallel = mat[j+1,i+1]
        speedup[j,i] = serial / parallel
print speedup
    