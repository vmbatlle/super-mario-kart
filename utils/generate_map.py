import matplotlib.pyplot as plt
from matplotlib import patches
import numpy as np
import itertools
import argparse

PPT = 8 # Pixels per tile
WALL_COLOR = np.array([160, 144, 96]) / 255.0

parser = argparse.ArgumentParser()
parser.add_argument('-f', '--file', type=str,
                    help='Map PNG filename.')
parser.add_argument('-w', '--walls', type=str,
                    help='Walls PNG filename.')
args = parser.parse_args()

print('Leyendo {file} con paredes {walls}'.format(file=args.file, walls=args.walls))
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
            rect = patches.Rectangle((x*PPT, y*PPT), PPT, PPT, color='r')
            # Add the patch to the Axes
            ax.add_patch(rect)
            walls[y, x] = 1
    # if np.sum(np.median(zone, axis=(0, 1)) - WALL_COLOR) < 1e-6:
    #     # Create a Rectangle patch
    #     rect = patches.Rectangle((x*PPT, y*PPT), PPT, PPT, color='g')
    #     # Add the patch to the Axes
    #     ax.add_patch(rect)

with open('kart_map.txt', 'w+') as f:
    for y in range(h):
        for x in range(w):
            f.write(str(walls[y, x]))
        f.write('\n')

plt.show()


