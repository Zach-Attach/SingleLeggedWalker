import pickle
import glob

input_files = ["out/1/best.txt", "out/2/best.txt"]
output_file = "out/output.pickle"

data = [{},{}]

with open(input_files[0], 'r') as file:
    for line in file:
        key, fitness = line.strip().split(' ')
        data[0][int(key)] = {
            "fitness": float(fitness)
        }

data[0][0]['tc'] = 0
data[0][0]['bias'] = 0
data[0][0]['weight'] = 0
# Loop over numbers 1 through 242
for i in range(1, 243):
    # Use glob to find the file matching the pattern
    file_pattern = f"out/1/best/ns/*_{i}.dat"
    files = glob.glob(file_pattern)

    if not files:
        print(f"No files found for pattern: {file_pattern}")
        continue

    for file_path in files:
        try:
            # Open the file
            with open(file_path, 'r') as file:
                lines = file.readlines()

                # Assign the values to the dictionary
                data[0][i]['tc'] = float(lines[2].strip())     # 3rd line
                data[0][i]['bias'] = float(lines[4].strip())   # 5th line
                data[0][i]['weight'] = float(lines[8].strip()) # 9th line

        except FileNotFoundError:
            print(f"File not found: {file_path}")
        except IndexError:
            print(f"File format error in: {file_path}")

# The data dictionary is now populated with the required data.

with open(input_files[1], 'r') as file:
    for line in file:
        key1, key2, fitness = line.strip().split(' ')
        if int(key1) not in data[1]:
            data[1][int(key1)] = {}
        data[1][int(key1)][int(key2)] = {
            "fitness": float(fitness)
        }


print(data[0])
with open(output_file, 'wb') as file:
    pickle.dump(data, file)
