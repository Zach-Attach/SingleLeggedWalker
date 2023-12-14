import matplotlib.pyplot as plt
import numpy as np

# Defining the parametric functions
def parametric1(t):
    return -np.arctan((t**2 - 300)/(300 * np.sqrt(3))), -(600 * t * np.sqrt(3))/(t**4 - 600 * t**2 + 360000)

def parametric2(t):
    return -np.arctan((t * np.sqrt(6) + 15)/(15 * np.sqrt(3))), -(15/(np.sqrt(2) * (t**2 + 5 * t * np.sqrt(6) + 150)))

def parametric3(t):
    return t**2/80 - np.pi/6, t/40

# Creating the plot
plt.figure(figsize=(6, 6))

# Plotting the parametric plots
t1 = np.linspace(0, 10 * np.sqrt(6), 1000)
plt.plot(*parametric1(t1), color='gray', linestyle='dashed', linewidth=1)

t2 = np.linspace(0, 25/np.sqrt(6), 1000)
plt.plot(*parametric2(t2), color='gray', linestyle='dashed', linewidth=1)

t3 = np.linspace(0, 4 * np.sqrt(5 * np.pi/3), 1000)
plt.plot(*parametric3(t3), color='gray', linestyle='dashed', linewidth=1)

# Plotting additional lines
plt.plot([-np.arctan(8/(3 * np.sqrt(3))), -np.pi/6], [-(9 * np.sqrt(2))/455, 0], color='gray', linestyle='dashed', linewidth=1)
plt.plot([np.pi/6, np.pi/6], [np.sqrt(np.pi/15)/2, 0], color='gray', linestyle='dashed', linewidth=1)

# Setting the specified y-axis range
plt.ylim(-0.1, 0.3)

# Setting x-axis and y-axis tick labels
plt.xticks([-1, -np.pi/6, 0, np.pi/6], ['-1', '-π/6', '0', 'π/6'])
plt.yticks(np.arange(-0.1, 0.31, 0.05))

# Displaying the plot
plt.show()

