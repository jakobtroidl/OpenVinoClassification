# Install script for directory: /opt/intel/openvino/inference_engine/demos

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/thirdparty/gflags/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/common/format_reader/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/crossroad_camera_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/gaze_estimation_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/human_pose_estimation_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/interactive_face_detection_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/mask_rcnn_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/multichannel_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/my_face_detection_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/object_detection_demo_faster_rcnn/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/object_detection_demo_ssd_async/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/object_detection_demo_yolov3_async/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/pedestrian_tracker_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/security_barrier_camera_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/segmentation_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/smart_classroom_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/super_resolution_demo/cmake_install.cmake")
  include("/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/text_detection_demo/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/jakobtroidl/Desktop/OpenVINO_ClassificationService/face_testBuild/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
