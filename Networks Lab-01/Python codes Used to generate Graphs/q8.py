import matplotlib.pyplot as plt
from pylab import MaxNLocator


# x axis values
x = [1,5,8,14,17,23]
# corresponding y axis values
y = [29,29,28,30,29,29]

# plotting the points
plt.plot(x, y, color='green',linewidth = 3,
         marker='o', markerfacecolor='blue', markersize=8)
plt.xlim(0,24)
plt.ylim(26,31)
# setting x and y axis range
# naming the x axis
plt.xlabel('Time - Different Hours of a Day(24 Hour Format)',color="red")
# naming the y axis
plt.ylabel('Number of Hosts Online',color="red")
 
# giving a title to my graph
plt.title('Graph to find Hourly trends of Hosts Online',color="magenta")
 
# function to show the plot
plt.show()
