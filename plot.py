import matplotlib.pyplot as plt

with open('plot.txt','r') as f:
    array = []
    for line in f: # read rest of lines
        array.append([int(x) for x in line.split()])

x = []
s = []
r = []
i = []

for j in array:
	x.append(j[0])
	s.append(j[1])
	i.append(j[2])
	r.append(j[3])
plt.plot(x,i, label = "Infected", color = 'red')
plt.plot(x,r,label = "Recovered", color = 'green')
#plt.plot(x,s,label = "Susceptible", color = 'blue')
plt.title("Daily results")
plt.legend()
plt.show()
  