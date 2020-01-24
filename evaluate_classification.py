import csv
import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

path = './results/face_detection_result.csv'  # sys.argv[1]
class_idx_filename = 0
label_index_csv = 7
bins = 10
range = 100

objects = ('0-9', '10-9', '20-29', '30-39', '40-49', '50-59', '60-69', '70-79', '80-89', '90-99')
y_pos = np.arange(len(objects))

buckets = np.zeros(bins)
buckets_counter = np.ones(bins)

with open(path) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')

    detected = 0  # number of detected frames of selected class
    line_count = 0  # number of lines processed
    diff = 0
    # iterate through all lines in .csv file
    for row in csv_reader:
        filename = row[0]
        gt = filename.split("_")[class_idx_filename]
        classification = row[label_index_csv]
        
        gt = gt.replace(" ", "")
        gt = gt.replace(";", "")
        classification = classification.replace(" ", "")
        classification = classification.replace(";", "")

        try:
            classification = float(classification)
            gt = float(gt)
        except ValueError:
            continue

        if gt < range:
            idx = int(gt / bins)
            buckets[idx] += abs(float(gt) - float(classification))
            buckets_counter[idx] += 1

        diff += abs(float(gt) - float(classification))
        line_count += 1
    buckets = buckets / buckets_counter
    mean_deviation = (diff / (line_count - 1))
    print("Average deviation: ", round(mean_deviation, 2))

    plt.bar(y_pos, buckets, align='center', alpha=0.5)
    plt.xticks(y_pos, objects)

    plt.ylabel('Average Error in %')
    plt.title('Average age deviation over age classes')

    plt.show()
