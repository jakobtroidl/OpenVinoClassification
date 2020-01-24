import csv
import sys

if len(sys.argv) < 4:
    sys.exit("Number of arguments is lower than 3")

path = sys.argv[1]
correct_label = int(sys.argv[2])
label_index = int(sys.argv[3])

with open(path) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')

    detected = 0  # number of detected frames of selected class
    line_count = 0  # number of lines processed
    last_filename = ""
    objects = []

    # iterate through all lines in .csv file
    for row in csv_reader:
        curr_label = row[label_index].replace(" ", "")
        objects.append(curr_label)

        # evaluate object list
        if row[0] != last_filename:
            if str(correct_label) in objects:
                detected += 1
            objects.clear()
            line_count += 1

        last_filename = row[0]

    accuracy = (detected / (line_count - 1)) * 100
    print("Accuracy: ", round(accuracy, 2), "%")
