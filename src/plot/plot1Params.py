
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


def plotFull():

        # Create an ndarray to store the key and fitness value pairs
    result1 = np.empty(len(good1N))

    for i in range(len(good1N)):
        result1[i] = float(data[0][good1N[i]]['fitness'])

    # Create the coupling data
    couplings = couplingArray().transpose()

    grid = {"height_ratios" : [8,2]}

    # Create the figure and axes for the plot
    fig, ax = plt.subplots(2, 1, figsize=(10, 10),  gridspec_kw=grid)

    # Make the plot look nicer
    # plt.tight_layout(pad=0)
    plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1, wspace=0, hspace=0)

    newCouplings1 = np.array([couplings[i] for i in good1N])

    grid = {"height_ratios" : [2,2]}

    # Create the figure and axes for the plot
    fig, ax = plt.subplots(2, 1, figsize=(10, 5),  gridspec_kw=grid, dpi=200)

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
    
    attributes = ['tc', 'bias', 'weight']

    # Creating an array for the plot
    plot_array = np.zeros((len(attributes), len(good1N)))

    # Populate the array
    for j, attr in enumerate(attributes):
        for i, g in enumerate(good1N):
            plot_array[j, i] = data[0][g][attr]

    # Plotting
    cax = ax[0].imshow(plot_array, aspect='auto')
        
    for a in ax:
        a.tick_params(
            axis='x',  # changes apply to the x-axis
            which='both',  # both major and minor ticks are affected
            bottom=False,  # ticks along the bottom edge are off
            top=False,  # ticks along the top edge are off
            labelbottom=False)  # labels along the bottom edge are off
    mainAx = ax[0]
    # Plot the data
    ax[0].set_yticks(np.arange(3))
    ax[0].set_yticklabels(['tau', 'phi', 'w'])

    ax[1].imshow(newCouplings1.transpose(), cmap=map_RB(), aspect='auto')
    # Show all ticks and label them with the respective list entries
    ax[1].set_yticks(np.arange(5))
    ax[1].set_yticklabels(['FtM', 'FM', 'BM', 'FtS', 'AS'])
    mainAx.margins(x=0)
    # Save the plot to a file
    plt.savefig('plots/scatter1WorkingPattern.png')
    plt.show()

    noXTicks(ax[0])
    noYTicks(ax[0])
    noXTicks(ax[1])

    mainAx = ax[0]
    # Plot the data
    # Attributes
###

    ax[1].imshow(newCouplings1, cmap=map_RB(), aspect='auto')

    # Adding a colorbar
    plt.colorbar(cax, ax=ax[0])

    ax[0].set_ylabel('Attribute')
    ax[0].set_title('Attribute Values Across Keys')

    mainAx.margins(x=0)
    # Save the plot to a file
    plt.savefig('plots/scatter2.png')
    plt.show()


plotFull()