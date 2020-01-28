How to use the OpenVino Classification Service

1. main.sh tdo

2. evaluate_detection.py 
    path_to_csv 
    label_in_path 
    correct_label 
    label_index_in_csv

In order to evaluate the person detection accuracy of the yoloV3 model from the utk face dataset, run
python3 evaluate_detection.py /path/to/service/results/yolo_results.csv 0 0 1


In order to evaluate the detection accuracy of the OpenVino facial analysis model from the utk face dataset, run
python3 evaluate_detection.py /path/to/service/results/face_detection_result.csv 0 1 1

In order to evaluate the gender detection accuracy type 
python3 evaluate_detection.py /path/to/service/results/face_detection_result.csv 1 1 6

todo classification description



3. evaluate_classification.py

