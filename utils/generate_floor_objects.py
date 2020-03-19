import matplotlib.pyplot as plt
from matplotlib import patches
import numpy as np
import itertools
import argparse
import os

PPT = 8  # Pixels per tile
PPO = 16  # Pixels per object

parser = argparse.ArgumentParser()
parser.add_argument('-f', '--file', type=str,
                    help='Map PNG filename.',
                    required=True)
parser.add_argument('-o', '--objects', type=str,
                    help='Objects PNG filename.',
                    required=True)
args = parser.parse_args()

print('Leyendo {file} con objetos {objects}'.format(
    file=args.file, objects=args.objects))
img_circ = plt.imread(args.file)
[h, w] = img_circ.shape[0:2]
h //= PPT
w //= PPT


img_objects = plt.imread(args.objects)
fig, ax = plt.subplots()
ax.imshow(img_circ * 0.7)

hit = np.zeros((h, w), dtype=np.uint8)
floor_objects = []
plot_colors = ['red', 'darkorange', 'yellow', 'lime', 'cyan', 'blue', 'magenta']
for (y, x) in itertools.product(range(h), range(w)):
    if hit[y, x] == 1:
        continue

    zone = img_circ[y*PPT:(y+1)*PPT, x*PPT:(x+1)*PPT, 0:3]

    for i in range(img_objects.shape[1] // PPO):
        obj = np.copy(img_objects[:, i*PPO:(i+1)*PPO, :])
        for r in range(obj.shape[0] // PPT):
            for c in range(obj.shape[1] // PPT):
                tile = obj[r*PPT:(r+1)*PPT, c*PPT:(c+1)*PPT, :]
                # Adjust rotation to the agreement of the last meeting
                # 0 --> up = top
                # 1 --> 90º clockwise
                # 2 --> 180º clockwise
                # 3 --> 270º clockwise
                rotation = [[0, 3], [1, 2]]
                # np.rot90 uses counter-clockwise, so it must be converted
                tile = np.rot90(tile, (4 - rotation[r][c]) % 4)
                colors = tile[:, :, 0:3]
                mask = np.reshape(np.repeat(tile[:, :, 3], 3, axis=1), (8,8,3))
                # if there is at least 1px alpha = 1
                # and if zone is not background
                # and if zones match
                if np.sum(mask) != 0 and \
                   np.mean(zone * mask) != np.mean(colors * (1 - mask)) and \
                   np.sum((zone - colors) * mask) == 0:

                    obj = np.rot90(obj, (4 - rotation[r][c]) % 4)

                    obj_w = 1
                    obj_h = 1

                    tile_top_right = obj[0:PPT, -PPT:, :]
                    tile_bottom_left = obj[-PPT:, 0:PPT, :]
                    # tile_bottom_right = obj[-PPT:, -PPT:, :]

                    # Check top right limit
                    if np.sum(tile_top_right[:, :, 3]) != 0:
                        colors = tile_top_right[:, :, 0:3]
                        mask = np.reshape(np.repeat(tile_top_right[:, :, 3], 3, axis=1), (8,8,3))
                        while x < w - 1:
                            x += 1
                            obj_w += 1
                            zone = img_circ[y*PPT:(y+1)*PPT, x*PPT:(x+1)*PPT, 0:3]
                            if np.sum((zone - colors) * mask) == 0:
                                break
                        else:
                            # handle unexpected errors
                            obj_w = 1
                        # restore original 'x'
                        x = x - obj_w + 1

                    # Check bottom left
                    if np.sum(tile_bottom_left[:, :, 3]) != 0:
                        colors = tile_bottom_left[:, :, 0:3]
                        mask = np.reshape(np.repeat(tile_bottom_left[:, :, 3], 3, axis=1), (8,8,3))
                        while y < h - 1:
                            y += 1
                            obj_h += 1
                            zone = img_circ[y*PPT:(y+1)*PPT, x*PPT:(x+1)*PPT, 0:3]
                            if np.sum((zone - colors) * mask) == 0:
                                break
                        else:
                            # handle unexpected errors
                            obj_h = 1
                        # restore original 'y'
                        y = y - obj_h + 1
                    
                    floor_objects.append({
                        'type' : i,
                        'loc' : (x, y),
                        'rot' : rotation[r][c],
                        'size' : (obj_w, obj_h)
                    })
                    hit[y:(y+obj_h), x:(x+obj_w)] = 1
                    # Create a Rectangle patch
                    rect = patches.Rectangle((x*PPT-0.5, y*PPT-0.5), 
                        obj_w * PPT, obj_h * PPT, color=plot_colors[i])
                    # Add the patch to the Axes
                    ax.add_patch(rect)
                if hit[y, x] != 0:
                    break
            if hit[y, x] != 0:
                break
        if hit[y, x] != 0:
                break
plt.show()

output = os.path.splitext(args.file)[0] + '.txt'
if os.path.exists(output):
    print('\"' + output + '\" already exists.')
    Ny = input('Do you want to replace it? [N/y]: ')
    if Ny != 'y' and Ny != 'Y':
        plt.close()
        exit(0)

with open(output, 'w+') as f:
    f.write(str(len(floor_objects)) + '\n')
    for obj in floor_objects:
        t = obj['type']
        (x, y) = obj['loc']
        r = obj['rot']
        (w, h) = obj['size']
        f.write('%d %d %d %d %d %d' % (t, x * PPT, y * PPT, r, w * PPT, h * PPT))
        f.write('\n')
