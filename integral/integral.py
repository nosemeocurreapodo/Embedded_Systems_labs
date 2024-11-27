import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

# Define the original signal
N = 10  # Number of original data points
x_original = np.linspace(0, 10, N)
y_original = np.sin(x_original)

# Points where we want to evaluate the interpolated functions
x_interp = np.linspace(0, 10, 1000)

# Zeroth-degree (nearest-neighbor) interpolation
nearest_interp = interp1d(x_original, y_original, kind='nearest')
y_nearest = nearest_interp(x_interp)

# First-degree (linear) interpolation
linear_interp = interp1d(x_original, y_original, kind='linear')
y_linear = linear_interp(x_interp)

# Second-degree (quadratic) interpolation
quadratic_interp = interp1d(x_original, y_original, kind='quadratic')
y_quadratic = quadratic_interp(x_interp)

# Third-degree (cubic) interpolation
cubic_interp = interp1d(x_original, y_original, kind='cubic')
y_cubic = cubic_interp(x_interp)

# Plot the original signal and the interpolations
plt.figure(figsize=(12, 8))
plt.plot(x_interp, np.sin(x_interp), 'k--', label='True Signal')
plt.plot(x_original, y_original, 'ko', label='Original Data Points')
plt.plot(x_interp, y_nearest, label='Zeroth Degree (Nearest-Neighbor Interpolation)')
plt.plot(x_interp, y_linear, label='First Degree (Linear Interpolation)')
plt.plot(x_interp, y_quadratic, label='Second Degree (Quadratic Interpolation)')
plt.plot(x_interp, y_cubic, label='Third Degree (Cubic Interpolation)')
plt.legend()
plt.xlabel('x')
plt.ylabel('Value')
plt.title('Signal and Its Interpolations')
plt.grid(True)
plt.show()
