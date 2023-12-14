
# Import the needed libraries
import matplotlib.pyplot as plt
import numpy as np
from lib.couplingArray import couplingArray, good1N
from lib.color import map_RB
import pickle
import numpy as np

# Create the data to plot
# data = np.random.random((243,))

# Load the data object from the pickle file
with open('out/output.pickle', 'rb') as file:
    data = pickle.load(file)

keys = range(243)

# Create an ndarray to store the key and fitness value pairs
result1 = np.empty(243)
result2 = np.empty((243,243))
for i in keys:
    result1[i] = float(data[0][i]['fitness'])
    for j in keys:
        result2[i][j] = float(data[1][i][j]['fitness']) if i > j else float(data[1][j][i]['fitness'])

# Create the data to plot
# data = np.random.random((243,243))

# Create the coupling data
couplings = couplingArray()

grid = {"height_ratios" : [8,2]}

# Create the figure and axes for the plot
fig, ax = plt.subplots(2, 1, figsize=(10, 10),  gridspec_kw=grid, dpi=200)

# Make the plot look nicer
# plt.tight_layout(pad=0)
plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1, wspace=0, hspace=0)

def noXTicks(axis):
    axis.tick_params(
        axis='x',  # changes apply to the x-axis
        which='both',  # both major and minor ticks are affected
        bottom=False,  # ticks along the bottom edge are off
        top=False,  # ticks along the top edge are off
        labelbottom=False)  # labels along the bottom edge are off

def noYTicks(axis):
    axis.tick_params(
        axis='y',  # changes apply to the x-axis
        which='both',  # both major and minor ticks are affected
        left=False,  # ticks along the bottom edge are off
        right=False,  # ticks along the top edge are off
        labelleft=False)  # labels along the bottom edge are off

for a in ax:
    a.tick_params(
        axis='x',  # changes apply to the x-axis
        which='both',  # both major and minor ticks are affected
        bottom=False,  # ticks along the bottom edge are off
        top=False,  # ticks along the top edge are off
        labelbottom=False)  # labels along the bottom edge are off
mainAx = ax[0]
# Plot the data
mainAx.scatter(range(243), result1, s=1, c='black')
ax[1].imshow(couplings, cmap=map_RB(), aspect='auto')
# Show all ticks and label them with the respective list entries
ax[1].set_yticks(np.arange(5))
ax[1].set_yticklabels(['FtM', 'FM', 'BM', 'FtS', 'AS'])
mainAx.margins(x=0)
# Save the plot to a file
plt.savefig('plots/scatter1.png')
plt.show()

