import pickle

def couplingArray():
  # Data generation
  couplings = []
  for i in range(243):
      c = []
      for _ in range(5):
          c.append(int(i % 3) - 1)
          i = int(i / 3)
      couplings.append(c)
  return couplings

# Load the data object from the pickle file
with open('out/output.pickle', 'rb') as file:
    data = pickle.load(file)

keys = range(243)
couplings = couplingArray()
# print('FtM\tFM\tBM\tFtS\tAS')
for i in keys:
    if data[0][i]['fitness'] != 0:
        print(i)
        # print('\t'.join([str(j) for j in couplings[i]]))

# Observation: only successful runs have foot motor and either one of the following:
# +1 forward and/or -1 backward motor
arrI = []
arrJ = []
for i in keys:
    for j in keys:
      if j < len(data[1][i]):
        if data[1][i][j]['fitness'] != 0:
          arrI += [i]
          arrJ += [j]
      else:
        if data[1][j][i]['fitness'] != 0:
          arrI += [i]
          arrJ += [j]

print(set(arrI))
# print(set(arrJ))
print(len(set(arrI)))
print(len(set(arrJ)))