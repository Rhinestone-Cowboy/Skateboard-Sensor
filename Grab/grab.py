import csv


file_path = 'texttest.txt'
lines = []
try:
    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            if "," in line:
                lines.append(line.strip())
except FileNotFoundError:
    print(f"Error: The file '{file_path}' was not found.")
except Exception as e:
    print(f"An error occurred: {e}")

datapoints = []
for line in lines:
    datapoints.append(line.split(","))

print(datapoints)

header = ["data_num","trick_type", "time", 
          "mag_x", "mag_y", "mag_z",
          "acc_x", "acc_y", "acc_z",
          "gyro_x", "gyro_y", "gyro_z"]


with open('data.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(header)
    writer.writerows(datapoints)








