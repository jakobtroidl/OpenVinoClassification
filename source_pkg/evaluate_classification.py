import csv
import sys

import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

# description when using from command line
# arg1: path_to_csv
# arg2: classification_index_filename
# arg3: classification_index_csv
# arg4: number_of_buckets
# arg5: max_classification_value_to_be_considered

# '/home/jakobtroidl/Desktop/face_detection_result.csv'
# path 0 7 10 100

if len(sys.argv) < 6:
  sys.exit("Number of arguments is lower than 5")

path = sys.argv[1]
class_idx_filename = int(sys.argv[2])
class_index_csv = int(sys.argv[3])
number_of_buckets = int(sys.argv[4])
max_classification_value = int(sys.argv[5])

objects = np.arange(0, number_of_buckets, 1)
y_pos = np.arange(len(objects))

buckets = np.zeros(number_of_buckets)
buckets_counter = np.ones(number_of_buckets)

with open(path) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')

    detected = 0  # number of detected frames of selected class
    line_count = 0  # number of lines processed
    diff = 0
    # iterate through all lines in .csv file
    for row in csv_reader:
        filename = row[0]
        gt = filename.split("_")[class_idx_filename]
        classification = row[class_index_csv]
        
        gt = gt.replace(" ", "")
        gt = gt.replace(";", "")
        classification = classification.replace(" ", "")
        classification = classification.replace(";", "")

        try:
            classification = float(classification)
            gt = float(gt)
        except ValueError:
            continue

        if gt < max_classification_value:
            idx = int(gt / (max_classification_value / number_of_buckets))
            buckets[idx] += abs(float(gt) - float(classification))
            buckets_counter[idx] += 1

            diff += abs(float(gt) - float(classification))
            line_count += 1

    buckets = buckets / buckets_counter
    mean_deviation = (diff / (line_count - 1))
    print("Average deviation: ", round(mean_deviation, 2))

    plt.bar(y_pos, buckets, align='center', alpha=0.5)
    plt.xticks(y_pos, objects)

    plt.xlabel("Buckets")
    plt.ylabel('Average Error')
    plt.title('Average deviation of classification per bucket')

    plt.show()
