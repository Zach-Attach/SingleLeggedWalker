
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

good2N = range(9,36)
# good2N = good2N[6:22]

def plotFull(dims):

    # Create an ndarray to store the key and fitness value pairs
    result1 = np.empty(len(good1N))
    result2 = np.empty((len(good2N),len(good2N)))
    for i in range(len(good1N)):
        result1[i] = float(data[0][good1N[i]]['fitness'])

    for i in range(len(good2N)):
        for j in range(len(good2N)):
            result2[i][j] = float(data[1][good2N[i]][good2N[j]]['fitness']) if i > j else float(data[1][good2N[j]][good2N[i]]['fitness'])

    # Create the data to plot
    # data = np.random.random((243,243))

    # Create the coupling data
    couplings = couplingArray().transpose()

    grid = {"height_ratios" : [8,2] ,"width_ratios" : [8,2]} if dims==2 else {"height_ratios" : [8,2]}

    # Create the figure and axes for the plot
    fig, ax = plt.subplots(2, dims, figsize=((5, 5) if dims ==2 else (5,2.5)),  gridspec_kw=grid)

    # Make the plot look nicer
    # plt.tight_layout(pad=0)
    plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1, wspace=0, hspace=0)

    newCouplings1 = np.array([couplings[i][0:3] for i in good1N])
    newCouplings2 = np.array([couplings[i][0:3] for i in good2N])

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

    if dims == 1:
        for a in ax:
            a.tick_params(
                axis='x',  # changes apply to the x-axis
                which='both',  # both major and minor ticks are affected
                bottom=False,  # ticks along the bottom edge are off
                top=False,  # ticks along the top edge are off
                labelbottom=False)  # labels along the bottom edge are off
        mainAx = ax[0]
        # Plot the data
        mainAx.scatter(range(len(good1N)), result1, s=1, c='black')
        ax[1].imshow(newCouplings1.transpose(), cmap=map_RB(), aspect='auto')
        # Show all ticks and label them with the respective list entries
        ax[1].set_yticks(np.arange(5))
        ax[1].set_yticklabels(['$E_{ft}$', '$E_{f}$', '$E_{b}$', '$S_{ft}$', '$S_{a}$'], fontsize=8)
        mainAx.margins(x=0)
        # Save the plot to a file
        plt.savefig('plots/scatter1WorkingPattern.png')
        plt.show()
    else:
        noYTicks(ax[0][1])
        noXTicks(ax[0][0])
        noYTicks(ax[0][0])
        noXTicks(ax[1][0])

        mainAx = ax[0][0]

        # Plot the data
        mainAx.imshow(result2, aspect='auto')
        ax[0][1].imshow(newCouplings2, cmap=map_RB(), aspect='auto')
        ax[1][0].imshow(newCouplings2.transpose(), cmap=map_RB(), aspect='auto')
        # Show all ticks and label them with the respective list entries
        ax[0][1].set_xticks(np.arange(3))
        ax[0][1].set_xticklabels(['$E_{ft}$', '$E_{f}$', '$E_{b}$'], fontsize=8)
        # Show all ticks and label them with the respective list entries
        ax[1][0].set_yticks(np.arange(3))
        ax[1][0].set_yticklabels(['$E_{ft}$', '$E_{f}$', '$E_{b}$'], fontsize=8)
        ax[1, 1].set_visible(False)

        mainAx.margins(x=0)
        # Save the plot to a file
        plt.savefig('plots/scatter2WorkingPattern.png')
        plt.show()

nd = 2

plotFull(nd)

