import sys
import matplotlib.pyplot as plt
import numpy as np

def readData (filename):
    with open(str(filename),'r') as data:
        x = []
        y = []
        for line in data:
            token = line.split()
            x.append(int(token[0]))
            y.append(int(token[1]))
        return x, y

def plotData (x,y):
    width = 1 / 1.5
    color = 'green'
    plt.bar(x,y,width,color=str(color),ec='black')
    plt.xlabel('Value')
    plt.ylabel('Frequency')
    plt.title('Histogram')
    plt.show()

if (len(sys.argv) < 2):
    print("Usage:> python %s <filename>" % sys.argv[0])
else:
    x,y = readData(sys.argv[1])
    plotData(x,y)