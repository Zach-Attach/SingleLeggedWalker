import matplotlib.pyplot as plt
import numpy as np

num = '224_18_HighRes' #'233_22_HighRes' #'233_22' #'224_18_HighRes' # 170

# Read the file and split each line into two values, saving the first to all_data and the second to differences
with open(f"out/2/behav/{num}.dat", "r") as file:
    lines = file.readlines()
    all_data = []
    differences = []
    for line in lines:
        values = line.split()
        if len(values) >= 2:
            all_data.append(float(values[0]))
            differences.append(float(values[1]))
all_data = all_data[:100_000]
differences = differences[:100_000]

# Defining the parametric functions
def parametric1(t):
    return -np.arctan((t**2 - 300)/(300 * np.sqrt(3))), -(600 * t * np.sqrt(3))/(t**4 - 600 * t**2 + 360000)

def parametric2(t):
    return -np.arctan((t * np.sqrt(6) + 15)/(15 * np.sqrt(3))), -(15/(np.sqrt(2) * (t**2 + 5 * t * np.sqrt(6) + 150)))

def parametric3(t):
    return t**2/80 - np.pi/6, t/40

# Creating the plot
plt.figure(1, dpi=800, figsize=(6, 6))

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
plt.ylim(-0.15, 0.35)

# Setting x-axis and y-axis tick labels
plt.xticks([-1, -np.pi/6, 0, np.pi/6], ['-1', '-π/6', '0', 'π/6'])
plt.yticks(np.arange(-0.1, 0.31, 0.05))

plt.plot(all_data, differences)
plt.xlabel('ϕ')
plt.ylabel('⍵')  # Corrected to theta with a dot on top
plt.savefig(f"plots/trajectory2_{num}.png")

# Plotting the second plot with switched axes and horizontal lines
plt.figure(2, dpi=800, figsize=(5,5))
plt.plot([i/1000 for i in range(len(all_data))], all_data)
plt.xlabel('Time')
plt.ylabel('ϕ')

# Adding horizontal lines at pi/6 and -pi/6
plt.axhline(y=np.pi/6, color='darkgrey', linestyle='dashed')
plt.axhline(y=-np.pi/6, color='darkgrey', linestyle='dashed')

# plt.legend()
plt.savefig(f"plots/angleTime2_{num}.png")