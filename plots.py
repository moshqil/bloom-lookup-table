# to do a plot when the results of the experiment will be ready

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns


a, b, c, d = [], [], [], []

for i in range(10):
    k, n, denom, percent = map(float, input().split())
    a.append(k)
    b.append(n)
    c.append(denom)
    d.append(percent)
a = np.array(a)
b = np.array(b)
d = np.array(d)
c = np.array(c)


fig = plt.figure(figsize=(80, 80))
ax = fig.add_subplot(111, projection='3d')
ax.set_xlim(-10, 10)
"""
x = np.random.standard_normal(100)
y = np.random.standard_normal(100)
z = np.random.standard_normal(100)
c = np.random.standard_normal(100)
"""
img = ax.scatter(a, b, d, c=c, cmap=plt.hot())
fig.colorbar(img)
plt.show()
