import numpy as np
from scipy.interpolate import interp1d
from scipy.integrate import quad
import matplotlib.pyplot as plt

# Define the Mexican hat function
def mexican_hat(t):
    return 100.0*(1 - t**2) * np.exp(-t**2 / 2)

# Define the range and number of sample points
t_min = -5
t_max = 5
num_samples = 20
t_samples = np.linspace(t_min, t_max, num_samples)
y_samples = mexican_hat(t_samples)

# Define a fine grid for plotting and integration
t_fine = np.linspace(t_min, t_max, 1000)
y_original = mexican_hat(t_fine)

# Integrate the original function
integral_original, _ = quad(mexican_hat, t_min, t_max)
print(f"Integral of original function: {integral_original:.6f}")

# Perform zeroth-degree (nearest neighbor) interpolation
interp0 = interp1d(t_samples, y_samples, kind='nearest', fill_value="extrapolate")

# Perform first-degree (linear) interpolation
interp1 = interp1d(t_samples, y_samples, kind='linear', fill_value="extrapolate")

# Perform second-degree (quadratic) interpolation
interp2 = interp1d(t_samples, y_samples, kind='quadratic', fill_value="extrapolate")

# Perform third-degree (cubic) interpolation
interp3 = interp1d(t_samples, y_samples, kind='cubic', fill_value="extrapolate")

# Define functions for interpolation to integrate
def interp0_func(t):
    return interp0(t)

def interp1_func(t):
    return interp1(t)

def interp2_func(t):
    return interp2(t)

def interp3_func(t):
    return interp3(t)

# Integrate each interpolated function
integral_interp0, _ = quad(interp0_func, t_min, t_max)
integral_interp1, _ = quad(interp1_func, t_min, t_max)
integral_interp2, _ = quad(interp2_func, t_min, t_max)
integral_interp3, _ = quad(interp3_func, t_min, t_max)

# Calculate errors
error0 = abs(integral_original - integral_interp0)
error1 = abs(integral_original - integral_interp1)
error2 = abs(integral_original - integral_interp2)
error3 = abs(integral_original - integral_interp3)

print(f"Integral of zeroth-degree interpolation: {integral_interp0:.6f}, Error: {error0:.6f}")
print(f"Integral of first-degree interpolation: {integral_interp1:.6f}, Error: {error1:.6f}")
print(f"Integral of second-degree interpolation: {integral_interp2:.6f}, Error: {error2:.6f}")
print(f"Integral of third-degree interpolation: {integral_interp3:.6f}, Error: {error3:.6f}")

# Plot the original function and the interpolations
plt.figure(figsize=(10, 6))
plt.plot(t_fine, y_original, label='Original function')
plt.plot(t_fine, interp0(t_fine), label='Zeroth-degree interpolation')
plt.plot(t_fine, interp1(t_fine), label='First-degree interpolation')
plt.plot(t_fine, interp2(t_fine), label='Second-degree interpolation')
plt.plot(t_fine, interp3(t_fine), label='Third-degree interpolation')
plt.scatter(t_samples, y_samples, color='black', label='Sample points')
plt.legend()
plt.title('Mexican Hat Function and Its Interpolations')
plt.xlabel('t')
plt.ylabel('ψ(t)')
plt.grid(True)
plt.show()
