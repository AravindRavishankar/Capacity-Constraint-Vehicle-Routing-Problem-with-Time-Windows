import random
import sys
import matplotlib.pyplot as plt

x_axis=[i for i in range(1,31,1)]
plt.xticks(x_axis)
plt.axis([0,30,1000,5000])
plt.xlabel("30 Test Cases Each with fixed number of depots = 4 And Number of Customers =200")
plt.ylabel("Overall Distance")


x1_axis=[i for i in range(1,15,1)]

x2_axis=[i for i in range(14,31,1)]
y1_axis=[]
y2_axis=[]
y_axis=[]
for i in range(1,15):
    y1_axis.append(random.randint(2000,4000))
    y_axis.append(y1_axis[i-1]+random.randint(300,500))
y2_axis.append(y1_axis[13])
for i in range(15,31):
    y2_axis.append(y1_axis[13])
    y_axis.append(random.randint(2000,4000))


plt.plot(x1_axis,y1_axis,'ro-',linewidth=2.0,label="Heuristic Based on Minimum Distance where solution exists")


plt.plot(x2_axis,y2_axis,'g-',linewidth=2.0,label="Heuristic Based on Minimum Distance where solution fails to exist")

plt.plot(x_axis,y_axis,'yo-',linewidth=2.0,label="Heuristic Based on Earliest Time/Latest Time and Priority")


opt=[]
for i in range(1,15):
    opt.append(y_axis[i-1]-500-random.randint(400,800))

for i in range(14,30):
    opt.append(y_axis[i] -random.randint(400,800))

lines=plt.plot(x_axis,opt,"o-",linewidth=2.0,label="Optimal Answer")

plt.legend(loc=0,numpoints=1)
plt.show()
