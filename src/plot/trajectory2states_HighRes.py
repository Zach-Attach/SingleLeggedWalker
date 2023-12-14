import matplotlib.pyplot as plt
import numpy as np

num = '224_18_HighRes_States' # 170

# Read the file and split each line into two values, saving the first to all_data and the second to differences
with open(f"out/2/behav/{num}.dat", "r") as file:
    lines = file.readlines()
    n1 = []
    n2 = []
    for line in lines:
        values = line.split()
        if len(values) >= 2:
            n1.append(float(values[0]))
            n2.append(float(values[1]))

n1 = n1[:60_000]
n2 = n2[:60_000]

# Plotting the second plot with switched axes and horizontal lines
fig, [ax1, ax2, ax3, ax4] = plt.subplots(4, 1, figsize=(6,4), dpi=800)
# plt.figure(1, dpi=800, figsize=(5,5))
ax4.plot([i/1000 for i in range(len(n1))], n1, color='blue')
ax4.plot([i/1000 for i in range(len(n2))], n2, color='red')

ax4.set_xlabel('Time')
ax4.set_ylabel('Neuron State', fontsize=8)  # Set the font size to 8

num = '224_18_HighRes_Output' # 170

# Read the file and split each line into two values, saving the first to all_data and the second to differences
with open(f"out/2/behav/{num}.dat", "r") as file:
    lines = file.readlines()
    n1 = []
    n2 = []
    for line in lines:
        values = line.split()
        if len(values) >= 2:
            n1.append(float(values[0]))
            n2.append(float(values[1]))

n1 = n1[:60_000]
n2 = n2[:60_000]

# Plotting the second plot with switched axes and horizontal lines
# plt.figure(1, dpi=800, figsize=(5,5))
ax3.plot([i/1000 for i in range(len(n1))], n1, color='blue')
ax3.plot([i/1000 for i in range(len(n2))], n2, color='red')

ax3.set_ylabel('Neuron Output', fontsize=8)  # Set the font size to 8

num = '224_18_HighRes' # 170

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

# Plotting the second plot with switched axes and horizontal lines

ax2.plot([i/1000 for i in range(len(all_data[:60_000]))], all_data[:60_000])

ax2.set_ylabel('Leg Angle', fontsize=8)  # Set the font size to 8

# Adding horizontal lines at pi/6 and -pi/6
ax2.axhline(y=np.pi/6, color='darkgrey', linestyle='dotted', label='Forward Angle Limit')
ax2.axhline(y=-np.pi/6, color='darkgrey', linestyle='dotted', label='Backward Angle Limit')

num = '224_18_HighRes_Foot' # 170

# Read the file and split each line into two values, saving the first to all_data and the second to differences
with open(f"out/2/behav/{num}.dat", "r") as file:
    lines = file.readlines()
    all_data = []
    for line in lines:
        value = line.strip()
        all_data.append(float(value))

# Plotting the second plot with switched axes and horizontal lines

ax1.plot([i/1000 for i in range(len(all_data[:60_000]))], all_data[:60_000])

ax1.set_ylabel('Foot State', fontsize=8)  # Set the font size to 8

fig.subplots_adjust(left=0.15, right=0.95, top=0.95, bottom=0.1, wspace=0, hspace=0.05)
ax1.set_xticks([])
ax2.set_xticks([])
ax3.set_xticks([])
# ax4.set_xticks([i for i in range(len(n1[::1000]))])

fig.align_ylabels()


# plt.legend()
plt.savefig(f"plots/angleTime2_{num}.png")
