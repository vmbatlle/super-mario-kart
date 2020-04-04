from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

img = Image.open('assets/drivers/yoshi.png').convert('RGBA')
img_array = np.array(img)
# green = np.array([68, 255, 68, 255])
green = np.array([51, 255, 51, 255])
print(green)
print(img_array[0, 0, :])

green_mask = np.all(img_array == green, axis=-1)
img_array[green_mask, 3] = 0

green = np.array([32, 246, 32, 255])
green_mask = np.all(img_array == green, axis=-1)
img_array[green_mask, 3] = 0

# plt.imshow(green_mask)
plt.imshow(img_array / 255)
plt.show()
Image.fromarray(img_array, 'RGBA').save('assets/drivers/yoshi.png')