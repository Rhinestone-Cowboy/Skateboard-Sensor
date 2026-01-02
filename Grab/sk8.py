import csv
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def print_all(csv_file_path):
    data_types = [
        "mag_x", "mag_y", "mag_z",
        "acc_x", "acc_y", "acc_z",
        "gyro_x", "gyro_y", "gyro_z"]

    for data in data_types:
        print_data(csv_file_path, data, True)

    plt.show()


def print_data(csv_file_path, data_name, cache_print=False):
    data = []
    time = []

    try:
        df = pd.read_csv(csv_file_path)
        data_num = df["data_num"].max()
        for i in range(1,int(data_num)+1):
            if data_name in df.columns:
                data = np.array(df.loc[df["data_num"] == float(i), data_name])
            else:
                print(f"Error: Column '{data_name}' not found.")
                return None
            time = np.array(df.loc[df["data_num"] == float(i), "time"])
            plt.figure()
            plt.plot(time, data)
            plt.xlabel("time(microseconds)")
            plt.ylabel(data_name)
            plt.title(f"data_num {i}")
    except FileNotFoundError:
        print(f"Error: File '{csv_file_path}' not found.")
        return None

    if not cache_print:
        plt.show()
    
    



def sk8_data_to_csv(data_file_path, csv_file_path):
    lines = []
    try:
        with open(data_file_path, 'r', encoding='utf-8') as file:
            for line in file:
                if "," in line:
                    lines.append(line.strip())
    except FileNotFoundError:
        print(f"Error: The file '{data_file_path}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

    datapoints = []
    for line in lines:
        str_line = line.split(",")
        int_line = [float(item) for item in str_line]
        datapoints.append(int_line)


    header = ["data_num","trick_type", "time", 
            "mag_x", "mag_y", "mag_z",
            "acc_x", "acc_y", "acc_z",
            "gyro_x", "gyro_y", "gyro_z"]


    with open(csv_file_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(header)
        writer.writerows(datapoints)








