import random
import sys
import matplotlib.pyplot as plt

x_axis=[i*10 for i in range(11)]
plt.xticks(x_axis)
plt.axis([0,100,800,1600])
plt.xlabel("Local Search Iterations")
plt.ylabel("Overall Distance")


x_axis=[i*10 for i in range(1,11,1)]
y_axis= [1560,1460,1400,1300,1100,1100,1100,1100,1100,1100]
plt.plot(x_axis,y_axis,'r-o',label="2-opt without improvements",linewidth=2.0)
y_axis=[1470 ,1420,1350,1170,940,920,920,920,920,920]

plt.plot(x_axis,y_axis,'g-o',label="3-opt with improvements",linewidth=2.0)
lines=plt.plot([10,100],[900,900],"b-",linewidth=2.0,label="Optimal Answer")
plt.legend(loc=0,numpoints=1)
plt.show()
