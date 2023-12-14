import matplotlib.pyplot as plt
import numpy as np

def read_data(num):
    file_path = f"out/1/behav/{num}.dat"
    with open(file_path, "r") as file:
        lines = file.readlines()
        all_data = []
        differences = []
        for line in lines:
            values = line.split()
            if len(values) >= 2:
                all_data.append(float(values[0]))
                differences.append(float(values[1]))
    return all_data, differences

num1 = 116  # Original value
num2 = 35   # New value for the second file
num3 = 197

all_data1, differences1 = read_data(num1)
all_data2, differences2 = read_data(num2)
all_data3, differences3 = read_data(num3)

# Plotting both sets of data on the same plot with different colors
# plt.figure(figsize=(10, 10))


# Defining the parametric functions
def parametric1(t):
    return -np.arctan((t**2 - 300)/(300 * np.sqrt(3))), -(600 * t * np.sqrt(3))/(t**4 - 600 * t**2 + 360000)

def parametric2(t):
    return -np.arctan((t * np.sqrt(6) + 15)/(15 * np.sqrt(3))), -(15/(np.sqrt(2) * (t**2 + 5 * t * np.sqrt(6) + 150)))

def parametric3(t):
    return t**2/80 - np.pi/6, t/40

# Creating the plot
fig = plt.figure(1, dpi=800, figsize=(5, 2.5))

# Plotting the parametric plots
t1 = np.linspace(0, 10 * np.sqrt(6), 1000)
plt.plot(*parametric1(t1), color='grey', linestyle='dotted')

t2 = np.linspace(0, 25/np.sqrt(6), 1000)
plt.plot(*parametric2(t2), color='grey', linestyle='dotted')

t3 = np.linspace(0, 4 * np.sqrt(5 * np.pi/3), 1000)
plt.plot(*parametric3(t3), color='grey', linestyle='dotted')

# Plotting additional lines
plt.plot([-np.arctan(8/(3 * np.sqrt(3))), -np.pi/6], [-(9 * np.sqrt(2))/455, 0], color='grey', linestyle='dotted')
plt.plot([np.pi/6, np.pi/6], [np.sqrt(np.pi/15)/2, 0], color='grey', linestyle='dotted', label="Optimal")

# Setting the specified y-axis range
plt.ylim(-0.12, 0.02)

# Setting x-axis and y-axis tick labels
plt.xticks([-1, -np.pi/6, 0, np.pi/6], ['-1', '-π/6', '0', 'π/6'])
plt.yticks(np.arange(-0.12, 0.02, 0.02),['-0.12', '-0.1','-0.08', '-0.06', '-0.04', '-0.02', '0'])

plt.plot(all_data1, differences1, color='limegreen', label="$w_{a} = 0$", linewidth=2)
plt.plot(all_data2, differences2, color='red', linestyle='dotted',label="$w_{a} < 0$", linewidth=2)
plt.plot(all_data3, differences3, color='blue', linestyle='dotted',label="$w_{a} > 0$", linewidth=2)

plt.xlabel('ϕ', fontsize=16)
plt.ylabel('⍵', fontsize=16)  # Corrected to theta with a dot on top
plt.legend()
fig.subplots_adjust(left=0.15, right=0.95, top=0.95, bottom=0.2, wspace=0, hspace=0.05)
plt.savefig(f"plots/trajectory1comparison.png", dpi=800)
# plt.show()

# Plotting the second plot with switched axes and horizontal lines
fig = plt.figure(2, dpi=800, figsize=(5,2.5))

# Adding horizontal lines at pi/6 and -pi/6
# plt.axhline(y=np.pi/6, color='grey', linestyle='dotted')
# plt.axhline(y=-np.pi/6, color='grey',linestyle='dotted')

plt.plot([i/10 for i in range(len(all_data1[:1000]))], all_data1[:1000], color='limegreen', label="$w_{a} = 0$")
plt.plot([i/10 for i in range(len(all_data1[:1000]))], all_data2[:1000], color='red', linestyle='dotted',label="$w_{a} < 0$")
plt.plot([i/10 for i in range(len(all_data1[:1000]))], all_data3[:1000], color='blue', linestyle='dotted', label="$w_{a} > 0$")

plt.xlabel('Time', fontsize=16)
plt.ylabel('ϕ', fontsize=16)
plt.yticks([-1, -np.pi/6, 0, np.pi/6], ['-1', '-π/6', '0', 'π/6'])
fig.subplots_adjust(left=0.15, right=0.95, top=0.95, bottom=0.2, wspace=0, hspace=0.05)

plt.legend()
plt.savefig(f"plots/angleTime1comparison.png", dpi=800)
