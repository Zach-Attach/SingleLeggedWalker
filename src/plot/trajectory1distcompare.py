import matplotlib.pyplot as plt
import numpy as np

num = '35_X'
num2 = '116_X'

# Read the file and split each line into two values, saving the first to all_data and the second to differences
with open(f"out/1/behav/{num}.dat", "r") as file:
    lines = file.readlines()
    all_data = []
    differences = []
    for line in lines:
        value = line.strip()
        all_data.append(float(value))

with open(f"out/1/behav/{num2}.dat", "r") as file:
    lines = file.readlines()
    all_data2 = []
    differences2 = []
    for line in lines:
        value = line.strip()
        all_data2.append(float(value))

# all_data.pop(-1)
# Plotting the second plot with switched axes and horizontal lines
fig = plt.figure(figsize=(5, 2.5))
plt.plot([i/10 for i in range(len(all_data[:1000]))], all_data[:1000], color='red', linestyle='dotted', label="$w_{a} < 0$")
plt.plot([i/10 for i in range(len(all_data2[:1000]))], all_data2[:1000], color='limegreen', label="$w_{a} = 0$")
plt.xlabel('Time', fontsize=16)
plt.ylabel('Distance', fontsize=16)
plt.legend()
fig.subplots_adjust(left=0.15, right=0.95, top=0.95, bottom=0.2, wspace=0, hspace=0.05)
plt.savefig(f"plots/dist_compare.png")
