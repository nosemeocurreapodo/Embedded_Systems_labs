import numpy as np
import cv2
from scipy.ndimage import convolve

# Load the image in grayscale
image = cv2.imread('render.png', cv2.IMREAD_GRAYSCALE)

# Define a 3x3 kernel (e.g., a simple averaging kernel for blurring)
kernel = np.array([[-1, 0, 1],
                   [-2, 0, 2],
                   [-1, 0, 1]], dtype=np.float32)  # Normalized kernel

# Get the dimensions of the image
height, width = image.shape

# Prepare an empty output image
output = np.zeros_like(image)
output = output.astype(np.uint8)

# Pad the image to handle the borders
padded_image = np.pad(image, pad_width=1, mode='constant', constant_values=0)

# Perform the convolution using nested loops
for i in range(height):
    for j in range(width):
        # Extract the region of interest
        roi = padded_image[i:i+3, j:j+3].astype(np.float32)/255.0
        # Apply the kernel to the region
        output[i, j] = (np.sum(roi * kernel) + 0.5)*127.0

# Save or display the output image
cv2.imwrite('filtered_image.png', output)
#cv2.imshow('Blurred Image', output)
#cv2.waitKey(0)
#cv2.destroyAllWindows()
