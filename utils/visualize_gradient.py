import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm

# matrix = np.loadtxt('gradient.txt', dtype=np.int32)
matrix = np.loadtxt('assets/circuit/donut_plains_1/gradient.txt', dtype=np.int32)
vis_type = '2d' # 2d or else 3d

if vis_type == '2d':

    matrix = np.power(matrix.flatten(), 0.3)
    matrix[matrix < 0] = max(matrix) # adjust to max value
    matrix = matrix.reshape((128, 128))

    plt.imshow(matrix, cmap='hot', interpolation='nearest')

else:

    # NOTA: para ver position hace falta editar ligeramente el fichero (ultima linea)

    matrix[matrix < 0] = 0
    matrix[matrix < 1] = 1

    x_data, y_data = np.meshgrid( np.arange(matrix.shape[1]),
                                np.arange(matrix.shape[0]) )

    x_data = x_data.flatten()
    y_data = y_data.flatten()
    cmap = cm.get_cmap(name='hot', lut=451)
    matrix = matrix.flatten()
    colors = np.array([cmap(matrix[i]) for i in range(len(matrix))])
    z_data = matrix

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.bar3d(x_data, y_data, np.zeros(len(z_data)), 1, 1, z_data, shade=True, color=colors)

plt.show()