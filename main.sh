
# setup variables
input=$1 
device=$2

# Setup OpenVino Environment
source /opt/intel/openvino/bin/setupvars.sh;

cd source_pkg/

yolo_model=../models/yolo_v3/frozen_darknet_yolov3_model.xml
yolo_labels=../models/yolo_v3/coco.names

face_model=../models/facedetection/FP16/face-detection-retail-0004.xml
age_gender_model=../models/gender_age/FP16/age-gender-recognition-retail-0013.xml

if [ "$device" = "CPU" ]; then
    echo "Its a CPU"
elif [ "$device" = "MYRIAD" ]; then
    echo "its a MYRIAD"
    face_model=../models/facedetection/FP32/face-detection-retail-0004.xml
    age_gender_model=../models/gender_age/FP32/age-gender-recognition-retail-0013.xml
elif [ "$device" = "GPU" ]; then
    echo "its a GPU"
    face_model=../models/facedetection/FP32/face-detection-retail-0004.xml
    age_gender_model=../models/gender_age/FP32/age-gender-recognition-retail-0013.xml
else
    echo "INVALID DEVICE"
fi

# Run Yoloobject detection on input data
python3 object_detection_yolov3.py -m $yolo_model -i $input -d $device --label $yolo_labels;

# Run openvino face detection / classification on input
./my_face_detection_demo -i $input -m $face_model -d $device -m_ag $age_gender_model -d_ag $device;