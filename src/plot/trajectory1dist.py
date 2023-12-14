import matplotlib.pyplot as plt
import numpy as np

num = '170_LONG_X'

# Read the file and split each line into two values, saving the first to all_data and the second to differences
with open(f"out/1/behav/{num}.dat", "r") as file:
    lines = file.readlines()
    all_data = []
    differences = []
    for line in lines:
        value = line.strip()
        all_data.append(float(value))

all_data.pop(-1)
# Plotting the second plot with switched axes and horizontal lines
plt.figure(figsize=(5,2.5))
plt.plot([i/10 for i in range(len(all_data))], all_data)
plt.xlabel('Time')
plt.ylabel('Distance')

plt.savefig(f"plots/dist_{num}.png")
