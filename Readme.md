## Prerequisites
1. This system is designed for an Ubuntu 18.04 OS.
2. Make sure your local system meets the OpenVino system requirements.

## How to setup the OpenVino based Classification Service on your local machine 
(at Daniel: you can skip this section since you will use it only on the server)

  
2. Install OpenVino (v2019_R3.1 or newer) from https://docs.openvinotoolkit.org/latest/_docs_install_guides_installing_openvino_linux.html and all dependencies of Openvino
3. install all modules that are listed in the requirements.txt file of this repository if they are not yet installed
4. Clone this repository
5. Download and unzip (if neccessary) the required models from https://drive.google.com/open?id=1agkPt6rtCsaMt5iZd9A1D1ScG98BIksb . Paste the folder *models* inside the root of this repo.
6. Create a folder called *builds* inside the root of this repo
7. Now we need to build the c++ samples. Since all required software comes with the OpenVino installation, we should be ready to go.
    1. **Build the OpenVino face_detection:** 
    - Navigate to the build folder and call `cmake /path/to/service/src/classification`
    - Now call `make face_detection`

## How to use the OpenVino based Classification Service


## General Information about the framework 


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

