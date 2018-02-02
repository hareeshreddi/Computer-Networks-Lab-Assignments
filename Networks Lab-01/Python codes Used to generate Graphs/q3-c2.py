import sys
import os

import matplotlib.pyplot as plt
from numpy import median


ages=[]
f=open("p2.txt","r")
for x in range(0,1003,1):
	y=f.readline()
	if(x>=3):
		ages.append(float(y[-9:-4]))

print(median(ages))
# setting the ranges and no. of intervals
range = (min(ages), max(ages))
bins = int((max(ages)-min(ages))*35)
 
# plotting a histogram
plt.hist(ages, bins, range, color = 'red',
        histtype = 'bar', rwidth = 0.6)
 
# x-axis label
plt.xlabel('Ping Latency in milli seconds',color='blue')
# frequency label
plt.ylabel('Frequency',color='blue')
# plot title
plt.title('Distribution of Ping Latencies',color='magenta')

print(min(ages))
print(max(ages))
# function to show the plot
plt.show()

