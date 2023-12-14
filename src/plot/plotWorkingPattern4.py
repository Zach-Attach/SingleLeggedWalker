
# Import the needed libraries
import matplotlib.pyplot as plt
import numpy as np
from lib.couplingArray import couplingArray, good1N
from lib.color import map_RRRBBB, map_RRBB
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
    

def plotFull():

    # Create an ndarray to store the key and fitness value pairs
    result1 = np.empty(len(good1N))
    result2 = np.empty((len(good2N),len(good2N)))
    for i in range(len(good1N)):
        result1[i] = float(data[0][good1N[i]]['fitness'])

    newData = [[0 for j in range(243)] for i in range(243)]
    for i in range(243):
        for j in range(i+1):
            newData[i][j] = data[1][i][j]['fitness']
            newData[j][i] = data[1][i][j]['fitness']
    
    # Create a 27x27 list with the maximum "fitness" value of all coordinates that all match i%27, j%27
    max_fitness = []
    for i in range(27):
        row = []
        for j in range(27):
            max_val = max(newData[k][l] for k in range(i, 243, 27) for l in range(j, 243, 27))
            row.append(max_val)
        max_fitness.append(row)
    result2 = np.array(max_fitness)
    result2 = np.roll(result2, -9, axis=0)
    result2 = np.roll(result2, 18, axis=1)
    selection = [9,10,11,0,1,2,3,4,5,6,7,8,24,25,26]
    result2 = result2[np.ix_(selection,selection)]

    # Create the data to plot
    # data = np.random.random((243,243))

    # Create the coupling data
    couplings = couplingArray().transpose()

    grid = {"height_ratios" : [8,2]}

    # Create the figure and axes for the plot
    fig, ax = plt.subplots(2, 1, figsize=(10, 10),  gridspec_kw=grid, dpi=200)

    # Make the plot look nicer
    # plt.tight_layout(pad=0)
    plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1, wspace=0, hspace=0)

    newCouplings1 = np.array([couplings[i][0:3] for i in good1N])
    newCouplings2 = np.array([couplings[i][0:3] for i in good2N])
    newerCouplings = np.array([[0,1,2,
                                0,1,2,
                                0,1,2,
                                0,1,2,
                                0,1,2,
                                0,1,2,
                                0,1,2,
                                0,1,2,
                                0,1,2,
                                ], [
        -4,-4,-4,
        -3,-3,-3,
        -2,-2,-2,
        -1,-1,-1,
        0,0,0,
        1,1,1,
        2,2,2,
        3,3,3,
        4,4,4]])

    newSelection = [
        [1,1],[1,2],[2,2],
        [1,4],[1,5],[2,5],
        [4,4],[4,5],[5,5],
        [4,7],[4,8],[5,8],
        [7,7],[7,8],[8,8],
        [7,10],[7,11],[8,11],
        [10,10],[10,11],[11,11],
        [10,13],[10,14],[11,14],
        [13,13],[13,14],[14,14]
        ]

    result2 = [result2[i][j] for i,j in newSelection]

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
    mainAx.scatter(range(27), result1, s=1, c='black')
    ax[1].imshow(newerCouplings, cmap=map_RRRBBB(), aspect='auto')
    # Show all ticks and label them with the respective list entries
    ax[1].set_yticks(np.arange(2))
    ax[1].set_yticklabels(['ΣFtM', 'Σ(FM-BM)'])
    mainAx.margins(x=0)
    # Save the plot to a file
    plt.savefig('plots/scatter1New.png')
    plt.show()

plotFull()

