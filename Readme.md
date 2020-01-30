## Prerequisites
1. This system is designed for an Ubuntu 18.04 OS.
2. Make sure your local system meets the OpenVino system requirements.

## How to setup the OpenVino based Classification Service on your local machine 
(at Daniel: you can skip this section since you will use it only on the server)

  
1. Install OpenVino (v2019_R3.1 or newer) from https://docs.openvinotoolkit.org/latest/_docs_install_guides_installing_openvino_linux.html and all dependencies of Openvino
2. install all modules that are listed in the requirements.txt file of this repository if they are not yet installed
3. Clone this repository
4. Download and unzip (if neccessary) the required models from https://drive.google.com/open?id=1agkPt6rtCsaMt5iZd9A1D1ScG98BIksb . Paste the folder *models* inside the root of this repo.
5. Create a folder called `builds` inside the root with two subfolders called `classification` and `evaluation`
6. Now we need to build the C++ samples. Since all required software comes with the OpenVino installation, we should be ready to go.

    **Build the OpenVino face_detection:** 
    - call `source /opt/intel/openvino/bin/setupvars.sh` to initialize OpenVino
    - Navigate to `/path/to/service/builds/classification` and call `cmake /path/to/service/src/classification`
    - Now call `make face_detection`
    - under `/path/to/service/builds/classification/intel64/Release` you should find a file called `face_detection`. Copy this file to the folder `/path/to/service/source_pkg/`

    **Build the OpenVino benchmark_app:**
    - call `source /opt/intel/openvino/bin/setupvars.sh` to initialize OpenVino
    - Navigate to `/path/to/service/builds/evaluation` and call `cmake /opt/intel/openvino/inference_engine/samples`
    - Now call `make benchmark_app`
    - under `/path/to/service/builds/evaluation/intel64/Release` you should find a file called `benchmark_app`. Copy this file to the folder `/path/to/service/source_pkg/`

7. Check if setup was succesful by running `./main.sh /path/to/image.jpg CPU` . If it was executed without errors the setup was successful. 


**Troubleshoots**
 - reinitialize OpenVino using `source /opt/intel/openvino/bin/setupvars.sh`
 - if you get an error similar to `cant initialize NC device, does not appear after boot` try to unplug the Neural Compute Stick and plug it in again. This will solve the error 90% of the time.

## How to use the OpenVino based Classification Service
(at Daniel: now it gets interesting for you! ;) )

- the **classification service** can be called via the `main.sh` file in the root. The general syntax of this command is as follows: `main.sh arg1_input arg2_device`
    - `arg1_input`: can be a path pointing to a single .jpg or .mp4 file or a path pointing to a folder of .jpg images
    - `arg2_device`: specifies the target device on which you want to interfer the service. It can either be `CPU` or `MYRIAD`
    - all results are written in the results folder. There are two .csv files containing information about the detection results and two folder containing the annotated images/videos

- the evaluation based on **model performance** (FPS/latency) can be studied using the `benchmark_app` inside the `source_pkg`folder. The general syntax of this command is as follows: `benchmark_app -i input_path -m model_to_evaluate -d device`

    - `-i input_path`: can be a path to an image or video
    - `-m model_to_evaluate`: should be a path to the .xml file of a model (can be found in the model folder)
    - `-d device`: should be either `CPU` or `MYRIAD` depending on which platform you want to evaluate the performance

- the evaluation based on model accuracy can be studied using the `evaluation_detection.py` and the `evaluate_classification.py` script:

    - `evaluation_detection.py arg1_path_to_csv arg2_label_in_path arg3_correct_label arg4_label_index_in_csv`
    - `arg1_path_to_csv`: path to the csv file you want to analyze
    - `arg2_label_in_path`: either 0 or 1 interpreted as boolean value. Is the correct label encoded in the path filename?
    - `arg3_correct_label`: if the correct label is encoded in the filename, then this argument is the index of the correct label inside the filename. e.g the filename looks like jakob_23_german.jpg the index of my age (23) would be 1. If the correct label is not encoded in the filename then this argument specifies the value of the correct label.
    - `arg4_label_index_in_csv`: this argument specifies the index of the column in the csv file where the label of the detection result is written.
- the evaluation based on classification like the estimation of the age, can be studied using the `evaluate_classification.py` script
    - `evaluate_classification.py arg1_path_to_csv arg2_classification_index_filename arg3_classification_index_csv arg4_ number_of_buckets arg5_max_classification_value_to_be_considered`
    - `arg1_path_to_csv`: path to the csv you want to analyze
    - `arg2_classification_index_filename`: the index of the ground truth in the filename
    - `arg3_classification_index_csv`: index of the classification in the csv file 
    - `arg4_ number_of_buckets`: number of buckets in which you want to subdivide your classification space, e.g age estimation can be subdivided into certain age classes like 0-9 up to 90-99 (in this case this parameter would be 10)
    - `arg5_max_classification_value_to_be_considered`: maximal classification value that shall be considered e.g the max age that should be considered is 100
    - a corresponding plot is safed in the same directory

## !! Specific information for Daniel !!
I already did all the setup on the server. All you need to do is to execute some commands and have a look at the results.

- In order to use my framework, login to the server and navigate to `/home/jtroidl/OpenVINO_ClassificationService/`. Please ask Theodor to get access to my home directory. 
- I already created a `data/` folder with some sample data including a sample images, videos and the UTK face dataset, which I used for evaluation
- to test the **main functionality** using the CPU call

`./main.sh /home/jtroidl/OpenVINO_ClassificationService/data/boy.jpg CPU`
- to test the main functionality using the Neural Compute Stick call

`./main.sh /home/jtroidl/OpenVINO_ClassificationService/data/boy.jpg MYRIAD`

- if you want to test my framework on a video call

`./main.sh /home/jtroidl/OpenVINO_ClassificationService/data/videos/pedestrians_0.mp4 CPU`

- the all results (.csv files and annotated images) are now written to the folder `results`

- to **evaluate performance numbers** of a model on the CPU follow these steps:
    - navigate to the `source_pkg` folder 
    - initialize OpenVino `source /opt/intel/openvino/bin/setupvars.sh`
    - call `./benchmark_app -i /home/jtroidl/OpenVINO_ClassificationService/data/boy.jpg -m /home/jtroidl/OpenVINO_ClassificationService/models/facedetection/FP16/face-detection-retail-0004.xml -d CPU`
    - if you want to evaluate the performance numbers on the NCS2 replace CPU by MYRIAD

- if you wish to **evaluate the accuracy of the yoloV3** person detection, follow these steps
    - navigate to the folder `source_pkg`
    - execute `python3 evaluate_detection.py /home/jtroidl/OpenVINO_ClassificationService/results/utkpart2_yolo_results.csv 0 0 1`
- if you wish to **evaluate the accuracy of the OpenVino** facial analysis model, follow these steps
    - navigate to the folder `source_pkg`
    - execute `python3 evaluate_detection.py /home/jtroidl/OpenVINO_ClassificationService/results/utkpart2_face_detection_result.csv 0 1 1`
- if you wish to **evaluate the accuracy of the gender estimation** of the OpenVino facial analysis model, follow these steps
    - navigate to the folder `source_pkg`
    - execute `python3 evaluate_detection.py /home/jtroidl/OpenVINO_ClassificationService/results/utkpart2_face_detection_result.csv 1 1 6`

- if you wish to **evaluate the average age deviation** of the OpenVino facial analysis model, follow these steps:
    - navigate to the folder `source_pkg`
    - `python3 evaluate_classification.py /home/jtroidl/OpenVINO_ClassificationService/results/utkpart2_face_detection_result.csv 0 7 10 100` 
    - the corresponding plot is safed in the same directory


## General Information about the framework 

- videos in the results folder are safed uncompressed as .avi files. I recommend opening them using the VLC media player and exporting them in a compressed data format.

 - the source code of all python related scripts can be found in the `source_pkg` folder. The source of the face_dection binary in the same folder can be found under `/src/classification/face_detection`

- information about the structure of resulting .csv files can be found in the first line of the respective csv file

- all evaluation related sources have been completley developed by me, all other sources are based on the samples provided by OpenVino, but they are strongly customized for our needs by me

- Please note that the `benchmark_app` was not developed by me. It is just a sample that comes with OpenVino.

