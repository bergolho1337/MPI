import os
import sys

if (len(sys.argv) < 2):
    print("=========================================================")
    print("Usage:> python makeInput.py <number_particles> <type>")
    print("<type> = 1 (Default), 2 (Solar System)")
    print("=========================================================")
else:
    dt = 0.001
    num_steps = 100000
    output_freq = 100
    num_part = int(sys.argv[1])
    typ = int(sys.argv[2])

    inFile = open("input","w")
    inFile.write("%lf %d %d %d\n" % (dt, num_steps, output_freq, num_part))
    
    if (typ == 1):
        m0 = 5.0e+24
        gap = 1.0e+05
        v0 = 3.0e+04
        for i in range(num_part):
            x0 = gap*i
            inFile.write("%e %e 0 0 0 %e 0\n" % (m0,x0,v0))
            v0 = -v0
    elif (typ == 2):
        inFile.write("5.000000e+24 0 0 0 0 0 0\n")         # Sun
        
        m0 = 1.0e+22 
        x0 = 1.0e+05 
        v0 = 1.0e+04
        for i in range(num_part-1):
            inFile.write("%e %e 0 0 0 %e 0\n" % (m0,x0,v0))
            m0 = m0 + 1.0e+20
            x0 = x0 + 1.0e+05
    
    inFile.close()