
# setup variables
input=$1 
device=$2

yolo_model=./models/yolo_v3/frozen_darknet_yolov3_model.xml
yolo_labels=./models/yolo_v3/coco.names

face_model=./models/facedetection/FP16/face-detection-retail-0004.xml
age_gender_model=./models/gender_age/FP16/age-gender-recognition-retail-0013.xml
# Setup OpenVino Environment
source /opt/intel/openvino/bin/setupvars.sh;

# Run Yoloobject detection on input data
python3 object_detection_demo_yolov3_async.py -m $yolo_model -i $input -d $device --label $yolo_labels;

# Run openvino face detection / classification on input
./my_face_detection_demo -i $input -m $face_model -d $device -m_ag $age_gender_model -d_ag $device;

