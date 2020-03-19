import matplotlib.pyplot as plt
from matplotlib import patches
import numpy as np
import itertools
import argparse
import os

PPT = 8  # Pixels per tile


def sum_rgb(arr, i):
    return arr[i] * 256 * 256 + arr[i + 1] * 256 + arr[i + 2]


# For the matrix:
# 0: road
# 1: walls
# 2: slow
# 3: outter
# 4: jump
# rest: defined here as (color_for_plot, value_for_matrix) tuple
TEXTURES = {
    'common' : {
        # jump
        sum_rgb(np.array([248, 248, 144]), 0): ('m', 4),
        # jump
        sum_rgb(np.array([232, 232, 0]), 0): ('m', 4)
    },
    # MARIO CIRCUIT
    'mario_circuit' : {
        # dirt
        sum_rgb(np.array([160, 144, 96]), 0): ('g', 2),

    },
    # DONUT PLAINS
    'donut_plains' : {
    # grass 1
        sum_rgb(np.array([0, 152, 0]), 0): ('y', 2),
        # grass 2 (darker)
        sum_rgb(np.array([0, 80, 0]), 0): ('y', 2),
        # grass 3 (lighter)
        sum_rgb(np.array([0, 248, 0]), 0): ('y', 2),
        # water 1
        sum_rgb(np.array([40, 72, 152]), 0): ('b', 3)
    },
    # GHOST VALLEY
    'ghost_valley' : {
        # void
        sum_rgb(np.array([0, 0, 0]), 0): ('b', 3),
    },
    # BOWSER CASTLE
    'bowser_castle' : {
        # lava
        sum_rgb(np.array([176, 0, 0]), 0): ('b', 3),
        # lava (darker)
        sum_rgb(np.array([120, 0, 0]), 0): ('b', 3),
    },
    # RAINBOW ROAD
    'rainbow_road' : {
        # void
        sum_rgb(np.array([0, 0, 0]), 0): ('b', 3),
    }
}

parser = argparse.ArgumentParser()
parser.add_argument('-f', '--file', type=str,
                    help='Map PNG filename.',
                    required=True)
parser.add_argument('-w', '--walls', type=str,
                    help='Walls PNG filename.')
parser.add_argument('-t', '--type',
                    choices=['mario_circuit',
                             'donut_plains',
                             'ghost_valley',
                             'bowser_castle',
                             'rainbow_road'],
                    help='Type of the circuit.',
                    required=True)
args = parser.parse_args()

MATCHES = {}
MATCHES.update(TEXTURES['common'])
MATCHES.update(TEXTURES[args.type])

print('Leyendo {file} con paredes {walls}'.format(
    file=args.file, walls=args.walls))
img_circ = plt.imread(args.file)
[h, w] = img_circ.shape[0:2]
h //= PPT
w //= PPT

if args.walls is not None:
    img_walls = plt.imread(args.walls)
fig, ax = plt.subplots()
ax.imshow(img_circ * 0.7)

kart_map = np.zeros((h, w), dtype=np.uint8)
for (y, x) in itertools.product(range(h), range(w)):
    zone = img_circ[y*PPT:(y+1)*PPT, x*PPT:(x+1)*PPT, :]

    if args.walls is not None:
        for i in range(img_walls.shape[1] // PPT):
            wall = img_walls[:, i*PPT:(i+1)*PPT, :]
            for i in range(4):
                if np.sum(zone - wall) == 0:
                    # Create a Rectangle patch
                    rect = patches.Rectangle((x*PPT-0.5, y*PPT-0.5), PPT, PPT, color='r')
                    # Add the patch to the Axes
                    ax.add_patch(rect)
                    kart_map[y, x] = 1
                    break
                wall = np.rot90(wall)
            if kart_map[y, x] != 0:
                break
        if kart_map[y, x] != 0:
            continue

    zone = np.reshape(zone * 255.0, (PPT * PPT * 3)).astype(np.uint8)
    u, c = np.unique(np.array([sum_rgb(zone, i * 3)
                               for i in range(PPT * PPT)]), return_counts=True)
    modes = u[np.argwhere(c == np.amax(c))]
    for mode in modes:
        for k in MATCHES.keys():
            if mode - k == 0:
                color, mat_value = MATCHES[k]
                # Create a Rectangle patch
                rect = patches.Rectangle((x*PPT-0.5, y*PPT-0.5), PPT, PPT, color=color)
                # Add the patch to the Axes
                ax.add_patch(rect)
                kart_map[y, x] = mat_value
plt.show()

output = os.path.splitext(args.file)[0] + '.txt'
if os.path.exists(output):
    print('\"' + output + '\" already exists.')
    Ny = input('Do you want to replace it? [N/y]: ')
    if Ny != 'y' and Ny != 'Y':
        plt.close()
        exit(0)

with open(output, 'w+') as f:
    for y in range(h):
        for x in range(w):
            f.write(str(kart_map[y, x]))
        f.write('\n')
