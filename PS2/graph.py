#!/bin/python

#Plots the data from 22.211 Pset 2 MC simulations. 
#
#Are you happy now Travis?

import matplotlib.pyplot as plt
import numpy as np
import csv
import math

collide=[]
path=[]

with open('FluxByCollide.csv', 'r') as csvFile:
    spam=csv.reader(csvFile)
    next(spam)  #skip the header row
    for row in spam:
        buffer=[]
        for cell in row:
            buffer.append(float(cell.strip())) #converts string to number
        buffer[2]=math.sqrt(buffer[2]) #convert the variance to a std dev
        collide.append(buffer) #shove it on the stack


with open('FluxbyPath.csv','r') as csvFile:
    spam=csv.reader(csvFile)
    next(spam)

    for row in spam:
        buffer=[]

        for cell in row:
            buffer.append(float(cell.strip()))
        buffer[2]=math.sqrt(buffer[2])
        path.append(buffer)

path=np.asmatrix(path)
collide=np.asmatrix(collide)
print path
#plt.scatter(path[:,0],path[:,1],label='Path-length tally')
plt.errorbar(path[:,0],path[:,1],yerr=path[:,2],label='Path-length tally',fmt='o')
plt.errorbar(collide[:,0],collide[:,1],yerr=collide[:,2], label='Collison tally',fmt='o')
#plt.plot(path[:,0],path[:,1],label='Path-length tally')
#plt.plot(collide[:,0],collide[:,1],label='Collision tally')
plt.axvline(x=0)
plt.xlabel('x-component [cm]')
plt.ylabel('reaction rate per source neutron')
plt.legend()
plt.show()

#savefig('ReactionWithError.png')

