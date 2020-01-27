import csv
import sys

# Version 1: correct label globally valid
# arg1:path_to_csv arg2:label_in_path(false) arg3:correct_label arg4:label_index

# Version 2: correct label encoded in filename
# arg1:path_to_csv arg2:label_in_path(true) arg3:label_index_filepath arg4:label_index

if len(sys.argv) < 5:
  sys.exit("Number of arguments is lower than 4")

path = sys.argv[1]
label_in_path = int(sys.argv[2])
correct_label = int(sys.argv[3])
label_index = int(sys.argv[4])

with open(path) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')

    detected = 0  # number of detected frames of selected class
    line_count = 0  # number of lines processed
    last_filename = ""
    objects = []

    # iterate through all lines in .csv file
    iterations = 0
    for row in csv_reader:
        curr_label = row[label_index].replace(" ", "")
        objects.append(curr_label)

        if iterations > 0:
            if label_in_path:
                filename = row[0]
                correct_value = filename.split("_")[correct_label]
            else:
                correct_value = correct_label

            # evaluate object list
            if row[0] != last_filename:
                if str(correct_value) in objects:
                    detected += 1

                line_count += 1
                objects.clear()

        last_filename = row[0]
        iterations += 1

    accuracy = (detected / (line_count)) * 100
    print("Accuracy: ", round(accuracy, 2), "%")
