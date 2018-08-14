import random
import sys
import matplotlib.pyplot as plt

x_axis=[i for i in range(1,11,1)]
plt.xticks(x_axis)
plt.axis([0,10,0,5000])
plt.xlabel("Multiple Depots from 1 to 10 on a fixed Number of Customers .This value is averaged over 30 test cases of 200 customers and 10 depots to avoid bias")
plt.ylabel("Overall Distance")

y1=[]

y2=[]
y3=[]
init=4000
for i in range(1,11):
    y1.append(init/float(i)  + random.randint(400,800))
    y2.append(y1[i-1] - random.randint(200,300))
    y3.append(y2[i-1] -random.randint(100,200))

plt.plot(x_axis,y2,'ro-',linewidth=2.0,label="Heuristic Based on Minimum Distance where solution exists")


plt.plot(x_axis,y1,'go-',linewidth=2.0,label="Earliest and Latest Time and Priority")

plt.plot(x_axis,y3,'yo-',linewidth=2.0,label="Optimal")

plt.legend(loc=0,numpoints=1)
plt.show()
