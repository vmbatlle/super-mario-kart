import matplotlib.pyplot as plt
from matplotlib import patches
import numpy as np
import itertools
import argparse

PPT = 8  # Pixels per tile


def sum_rgb(arr, i):
    return arr[i] * 256 * 256 + arr[i + 1] * 256 + arr[i + 2]


# For the matrix:
# 0: road
# 1: walls
# rest: defined here as (color_for_plot, value_for_matrix) tuple
MATCHES = {
    # MARIO CIRCUIT
    # walls.png
    # dirt
    sum_rgb(np.array([160, 144, 96]), 0): ('g', 2),

    # DONUT PLAINS
    # walls.png
    # grass 1
    sum_rgb(np.array([0, 152, 0]), 0): ('y', 2),
    # grass 2 (darker)
    sum_rgb(np.array([0, 80, 0]), 0): ('y', 2),
    # grass 3 (lighter)
    sum_rgb(np.array([0, 248, 0]), 0): ('y', 2),
    # water 1
    sum_rgb(np.array([40, 72, 152]), 0): ('b', 3),

    # GHOST VALLEY
    # walls_ghost_valley.png
    # void
    sum_rgb(np.array([0, 0, 0]), 0): ('b', 3),

    # BOWSER CASTLE
    # walls 1
    sum_rgb(np.array([216, 200, 200]), 0): ('r', 1),
    # walls 2
    10522768: ('r', 1), # weird bug, hardcoded
    # walls 3
    11575456: ('r', 1), # weird bug, hardcoded
    # lava 1 (most common)
    sum_rgb(np.array([176, 0, 0]), 0): ('b', 3),
    # lava 2 (brighter)
    248: ('b', 3),  # weird bug, hardcoded
    # lava 3 (darker)
    sum_rgb(np.array([120, 0, 0]), 0): ('b', 3),

    # RAINBOW ROAD
    # void
    sum_rgb(np.array([0, 0, 0]), 0): ('b', 3),
}

parser = argparse.ArgumentParser()
parser.add_argument('-f', '--file', type=str,
                    help='Map PNG filename.')
parser.add_argument('-w', '--walls', type=str,
                    help='Walls PNG filename.')
args = parser.parse_args()

print('Leyendo {file} con paredes {walls}'.format(
    file=args.file, walls=args.walls))
img_circ = plt.imread(args.file)
[h, w] = img_circ.shape[0:2]
h //= PPT
w //= PPT

img_walls = plt.imread(args.walls)
fig, ax = plt.subplots()
ax.imshow(img_circ * 0.7)

walls = np.zeros((h, w), dtype=np.uint8)
for (y, x) in itertools.product(range(h), range(w)):
    zone = img_circ[y*PPT:(y+1)*PPT, x*PPT:(x+1)*PPT, :]
    for i in range(4):
        wall = img_walls[:, i*PPT:(i+1)*PPT, :]
        if np.sum(zone - wall) == 0:
            # Create a Rectangle patch
            rect = patches.Rectangle((x*PPT-0.5, y*PPT-0.5), PPT, PPT, color='r')
            # Add the patch to the Axes
            ax.add_patch(rect)
            walls[y, x] = 1
    zone = np.reshape(zone * 255.0, (PPT * PPT * 3)).astype(np.uint8)
    u, c = np.unique(np.array([sum_rgb(zone, i)
                               for i in range(PPT * PPT)]), return_counts=True)
    mode = u[np.argmax(c)]
    if y == 35 and x == 28:
        print(u[np.argmax(c)])
    for k in MATCHES.keys():
        if mode - k == 0:
            color, mat_value = MATCHES[k]
            # Create a Rectangle patch
            rect = patches.Rectangle((x*PPT-0.5, y*PPT-0.5), PPT, PPT, color=color)
            # Add the patch to the Axes
            ax.add_patch(rect)
            walls[y, x] = mat_value

with open('kart_map.txt', 'w+') as f:
    for y in range(h):
        for x in range(w):
            f.write(str(walls[y, x]))
        f.write('\n')

plt.show()
