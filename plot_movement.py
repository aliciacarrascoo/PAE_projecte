import numpy as np
from matplotlib import pylab as plt

data = np.genfromtxt("movement.log", comments="#", delimiter=',')
x = data[:, 0]
y = data[:, 1]
w = data[:, 2]
vl = data[:, 3]
vr = data[:, 4]

fig, ax = plt.subplots()
q = ax.quiver(x, y, np.cos(w), np.sin(w), units='xy', scale=1, color='red')
plt.plot(x, y, 'o--')
plt.show()
