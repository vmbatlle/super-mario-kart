import numpy as np
import matplotlib.pyplot as plt

# matrix = np.loadtxt('gradient.txt', dtype=np.int32)
matrix = np.loadtxt('wallpenalty.txt', dtype=np.int32)

plt.imshow(matrix, cmap='hot', interpolation='nearest')
plt.show()