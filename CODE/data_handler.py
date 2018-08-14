import sys
import os
import random

out = open('IN.txt', 'w')
optout = open('OPT.txt', 'w')
depots = []
customer = []
opt_ans = 0

dir_path = os.path.dirname(os.path.realpath(__file__))
for filename in os.listdir(dir_path):
    if filename.endswith(".opt"):
        f = open(filename, 'r')
        for line in f.readlines():
            if "cost" in  line: opt_ans += int(line.split()[1].strip())


for filename in os.listdir(dir_path):
    if filename.endswith(".vrp"): 
        f = open(filename, 'r')
        x, y, c = random.randint(-100,101), random.randint(-100, 101), random.randint(1000, 100001)
        depots.append([x, y, c])
        start = 0
        cust_dict = {}
        for line in f.readlines():
        	if 'NODE_COORD' in line: start = 1
        	elif 'DEMAND' in line: start = 2
        	elif 'DEPOT' in line: start = 3
        	elif start == 1: 
        		cust_dict[line.split()[0]] = [int(line.split()[1]) + x, int(line.split()[2]) + y]
        	elif start == 2:
        		cust_dict[line.split()[0]].append(int(line.split()[1]))
        for elem in cust_dict:
        	customer.append(cust_dict[elem])

out.write(str(len(depots)) + '\n')
for depot in depots:
	line = str(depot[0]) + ' ' + str(depot[1]) + ' ' + str(depot[2]) + '\n'
	out.write(line)

out.write(str(len(customer)) + '\n')
for cust in customer:
	e = random.randint(0, 500)
	l = random.randint(e + 100, 2000)
	p = random.randint(1, 11)
	line = str(cust[0]) + ' ' + str(cust[1]) + ' ' + str(e) + ' ' + str(l) + ' ' + str(cust[2]) + ' ' + str(p) + '\n'
	out.write(line)
out.write('5000\n')
optout.write(str(opt_ans) + '\n')






