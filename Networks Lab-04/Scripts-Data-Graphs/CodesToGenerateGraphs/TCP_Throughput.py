import matplotlib.pyplot as plt
from pylab import MaxNLocator
# x axis values
x = [0,256,512,1000]
# corresponding y axis values
y = [51.88,52.318,52.216,52.2208]

# plotting the points
plt.plot(x, y, color='green',linewidth = 3,
         marker='o', markerfacecolor='blue', markersize=6)
#plt.xlim(0,24)
#plt.ylim(26,31)
# setting x and y axis range
# naming the x axis
plt.xlabel('RTS Threshold in bytes',color="red")
# naming the y axis
plt.ylabel('TCP Throughput in Mbps',color="red")
# giving a title to my graph
plt.title('Graph of TCP Throughput vs RTS Threshold',color="magenta")
# function to show the plot
plt.show()
