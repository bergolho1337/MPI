import os 
import sys
import re
from matplotlib import pyplot
from pylab import genfromtxt

if (len(sys.argv) < 2 ):
    print("Usage:> python " + sys.argv[0] + " <serial_data_file>")
else:
    filename = sys.argv[1]
    mat = genfromtxt(filename)

    pyplot.grid()
    pyplot.title("Basic x Reduce")
    pyplot.xlabel("Number of particles")
    pyplot.ylabel("Time elapsed (s)")
    pyplot.plot(mat[:,0], mat[:,1], label="Basic",marker='o')
    pyplot.plot(mat[:,0], mat[:,2], label="Reduce",marker='o')
    pyplot.legend(loc=0)
    pyplot.savefig("result.pdf")
