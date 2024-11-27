import numpy as np
import matplotlib.pyplot as plt

def cumulative_rectangle(x, y):
    dx = x[1] - x[0]
    I_rect = np.zeros(len(y))
    I_rect[1:] = np.cumsum(y[:-1]) * dx
    return I_rect

def cumulative_trapezoid(x, y):
    dx = x[1] - x[0]
    I_trap = np.zeros(len(y))
    I_trap[1:] = np.cumsum((y[:-1] + y[1:]) / 2) * dx
    return I_trap

def cumulative_simpson(x, y):
    N = len(x)
    I_simp = np.zeros(N)
    if N < 3:
        return I_simp  # Not enough points for Simpson's rule
    dx = x[1] - x[0]
    for i in range(2, N, 2):
        I_simp[i] = I_simp[i-2] + (dx/3)*(y[i-2] + 4*y[i-1] + y[i])
        if i+1 < N:
            # For odd indices, approximate using trapezoidal rule
            I_simp[i+1] = I_simp[i] + (y[i] + y[i+1])/2 * dx
    # Handle last point if N is even
    if N % 2 == 0:
        I_simp[N-1] = I_simp[N-3] + (dx/3)*(y[N-3] + 4*y[N-2] + y[N-1])
    return I_simp

# Define the signal
N = 1000
x = np.linspace(0, 10, N)
dx = x[1] - x[0]
y = np.sin(x)

# Compute cumulative integrals using different methods
I_rect = cumulative_rectangle(x, y)
I_trap = cumulative_trapezoid(x, y)
I_simp = cumulative_simpson(x, y)

# Plot the original signal and the cumulative integrals
plt.figure(figsize=(10,6))
plt.plot(x, y, label='Original Signal')
plt.plot(x, I_rect, label='First Degree (Rectangle Rule)')
plt.plot(x, I_trap, label='Second Degree (Trapezoidal Rule)')
plt.plot(x, I_simp, label="Third Degree (Simpson's Rule)")
plt.legend()
plt.xlabel('x')
plt.ylabel('Value')
plt.title('Signal and Its Integral Approximations')
plt.grid(True)
plt.show()
