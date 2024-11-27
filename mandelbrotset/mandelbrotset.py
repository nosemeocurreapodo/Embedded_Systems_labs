import numpy as np
import matplotlib.pyplot as plt

# Define the resolution of the image
width, height = 800, 600

# Define the real and imaginary range of the plot
re_start, re_end = -2.0, 1.0
im_start, im_end = -1.0, 1.0

# Create a meshgrid of complex numbers (c = x + yi)
real = np.linspace(re_start, re_end, width)
imag = np.linspace(im_start, im_end, height)
real, imag = np.meshgrid(real, imag)
c = real + 1j * imag

# Initialize z to zero
z = np.zeros_like(c)

# Set the maximum number of iterations
max_iter = 100

# Initialize an array to hold the iteration counts
mandelbrot_set = np.zeros(c.shape, dtype=int)

# Perform the iteration
for i in range(max_iter):
    # Apply the Mandelbrot iteration: z = z^2 + c
    z = z**2 + c
    # Identify points where the magnitude of z is still less than 2
    mask = (np.abs(z) <= 2)
    # Update the iteration counts for points that haven't escaped
    mandelbrot_set += mask

# Transpose the set for correct orientation
mandelbrot_set = mandelbrot_set.T

# Plot the Mandelbrot set
plt.figure(figsize=(10, 8))
plt.imshow(mandelbrot_set, extent=[re_start, re_end, im_start, im_end], cmap='hot')
plt.xlabel('Real Axis')
plt.ylabel('Imaginary Axis')
plt.title('Mandelbrot Set')
plt.show()
