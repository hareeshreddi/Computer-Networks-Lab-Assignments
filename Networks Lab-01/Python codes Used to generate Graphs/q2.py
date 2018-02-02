import matplotlib.pyplot as plt
from pylab import MaxNLocator


# x axis values
x = [64,128,256,512,1024,2048]
# corresponding y axis values
y = [251.407,251.556,252.396,254.957,257.258,262.441]

# plotting the points
plt.plot(x, y, color='green',linewidth = 3,
         marker='o', markerfacecolor='blue', markersize=6)
#plt.xlim(0,24)
#plt.ylim(26,31)
# setting x and y axis range
# naming the x axis
plt.xlabel('Packet Size in Bytes',color="red")
# naming the y axis
plt.ylabel('Average RTT in milliseconds',color="red")
 
# giving a title to my graph
plt.title('Graph of Average RTT VS Packet Size for the host cricbuzz.com',color="magenta")
 
# function to show the plot
plt.show()
