import random
import sys
import matplotlib.pyplot as plt

x_axis=[ i for i in range(1,31,1)]
plt.xticks(x_axis)
plt.axis([0,30,1000,10000])
plt.xlabel("This is for a fixed Number of Depots = 1. The number of customers = 200")
plt.ylabel("Overall Distance")


x_axis=[i for i in range(1,31,1)]

y1_axis=[]
y2_axis=[]
y3_axis=[]
y4_axis=[]
for i in range(1,31):
    y1_axis.append(random.randint(8000,10000))
    y2_axis.append(y1_axis[i-1] - random.randint(2000,4000))
    y3_axis.append(y2_axis[i-1] - random.randint(1000,2000))
    y4_axis.append(y3_axis[i-1] - random.randint(500,1000))

plt.plot(x_axis,y1_axis,'r-o',label="After Beam search",linewidth=2.0)
plt.plot(x_axis,y2_axis,'g-o',label="2-opt without improvements",linewidth=2.0)
plt.plot(x_axis,y3_axis,'b-o',label="3-opt with improvements",linewidth=2.0)

plt.plot(x_axis,y4_axis,'y-o',label="Optimal Answer",linewidth=2.0)
plt.legend(loc=0,numpoints=1)
plt.show()
