// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

/**
* \brief The entry point for the Inference Engine interactive_face_detection demo application
* \file interactive_face_detection_demo/main.cpp
* \example interactive_face_detection_demo/main.cpp
*/
#include <gflags/gflags.h>
#include <functional>
#include <iostream>
#include <random>
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <iterator>
#include <map>
#include <list>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <glob.h>
#include <dirent.h>

#include <inference_engine.hpp>

#include <samples/ocv_common.hpp>
#include <samples/slog.hpp>

#include "interactive_face_detection.hpp"
#include "detectors.hpp"
#include "face.hpp"
#include "visualizer.hpp"

#include <ie_iextension.h>
#ifdef WITH_EXTENSIONS
#include <ext_list.hpp>
#endif

using namespace InferenceEngine;

bool ParseAndCheckCommandLine(int argc, char *argv[])
{
    // ---------------------------Parsing and validating input arguments--------------------------------------
    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    if (FLAGS_h)
    {
        showUsage();
        showAvailableDevices();
        return false;
    }
    slog::info << "Parsing input parameters" << slog::endl;

    if (FLAGS_i.empty())
    {
        throw std::logic_error("Parameter -i is not set");
    }

    if (FLAGS_m.empty())
    {
        throw std::logic_error("Parameter -m is not set");
    }

    if (FLAGS_n_ag < 1)
    {
        throw std::logic_error("Parameter -n_ag cannot be 0");
    }

    if (FLAGS_n_hp < 1)
    {
        throw std::logic_error("Parameter -n_hp cannot be 0");
    }

    // no need to wait for a key press from a user if an output image/video file is not shown.
    FLAGS_no_wait |= FLAGS_no_show;

    return true;
}

bool exists(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

/* returns the path of a file that starts with the number specified in frameIdx, 
5 leading zeros included. 1 -> file starting with 00001, 5000 -> file starting with 05000*/
std::string SearchDirectory(std::string& directory, int frameIdx)
{
    DIR *dirp = opendir(directory.c_str());
    if (!dirp) {
        perror(("opendir " + directory).c_str());
        return std::string("");
    }

    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << frameIdx;
    std::string digits = ss.str();

    struct dirent *dptr;
    while((dptr = readdir(dirp))){
        std::string file_Name = dptr->d_name;
        std::string file_Path = directory + file_Name;

        //check wether file starts with digits
        if(!file_Name.rfind(digits, 0)){
            std::cout << file_Path.c_str() << std::endl;
            return file_Path;
        }
    }

    closedir(dirp);
    return std::string(""); // return empty string when no file is found
}

//checks wether a given paths points to a directory or not
bool isDirectory(const char *path)
{
    struct stat s;
    if (stat(path, &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            std::cout << path << ": isDirectory" << std::endl;
            return true;
        }
        else if (s.st_mode & S_IFREG)
        {
            std::cout << path << ": isFile" << std::endl;
            return false;
        }
        else
        {
            std::cout << path << ": is something else" << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << path << ": ERROR" << std::endl;
        return false;
    }
}
std::string getFileName(std::string path)
{
    std::stringstream ss_currFilename(path);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(ss_currFilename, segment, '/'))
    {
        seglist.push_back(segment);
    }

    return seglist[seglist.size() - 1];
}


int main(int argc, char *argv[])
{
    try
    {
        std::ostringstream out;
        size_t framesCounter = 0;
        bool frameReadStatus;
        bool isLastFrame;
        int delay = 1;
        double msrate = -1;
        cv::Mat prev_frame, next_frame;
        std::list<Face::Ptr> faces;
        size_t id = 0;

        std::string results_path = "../results/filesFaceDetection/";

        std::ofstream result_file; // create csv output file that contains all fetchResults
        result_file.open("../results/face_detection_result.csv");
        result_file << "frameIdx or filename, face_detected(bool), BB_upperLeftCorner.x, BB_upperLeftCorner.y, BB_height, BB_width, isFemale, age; \n";

        std::cout << "InferenceEngine: " << GetInferenceEngineVersion() << std::endl;

        // ------------------------------ Parsing and validating of input arguments --------------------------
        if (!ParseAndCheckCommandLine(argc, argv))
        {
            return 0;
        }

        slog::info << "Reading input" << slog::endl;
        cv::VideoCapture cap;

        const char *path = FLAGS_i.c_str();
        std::string path_str = path;
        bool processFolder = isDirectory(path);
        bool processVideo = false;

        if(!processFolder)
        {
            std::string file_extension = path_str.substr(path_str.find_last_of(".") + 1);
            if( file_extension == "mp4" || file_extension == "avi") {
                processVideo = true;
            } 
            else {
                //process single image
            }
        }


        std::vector<cv::String> fn;
        cv::String cv_path(path_str + "*.jpg");

        if(processFolder)
        {
            cv::glob(cv_path,fn,true); // recurse
        }

        if (!processFolder && !(FLAGS_i == "cam" ? cap.open(0) : cap.open(FLAGS_i)))
        {
            throw std::logic_error("Cannot open input file or camera: " + FLAGS_i);
        }

        Timer timer;
        // read input (video) frame
        cv::Mat frame;
        if(processFolder)
        {
            if(framesCounter < fn.size())
            {
                frame = cv::imread(fn[framesCounter]);
                slog::info << "Init First File" << slog::endl;

                if(!frame.data)                              // Check for invalid input
                {
                    slog::info <<  "Could not open: " << fn[framesCounter] << slog::endl;
                }
            }
            else
            {
                throw std::logic_error("[ERROR] No files to process in the given Folder!");
            }
        }
        else if (!cap.read(frame))
        {
            throw std::logic_error("Failed to get frame from cv::VideoCapture");
        }

        size_t width = static_cast<size_t>(frame.cols);
        size_t height = static_cast<size_t>(frame.rows);

        cv::VideoWriter videoWriter;
        if (processVideo)
        {
            videoWriter.open(results_path + "output.avi", cv::VideoWriter::fourcc('I', 'Y', 'U', 'V'), 25, cv::Size(width, height));
        }
        // ---------------------------------------------------------------------------------------------------
        // --------------------------- 1. Loading Inference Engine -----------------------------

        Core ie;

        std::set<std::string> loadedDevices;
        std::vector<std::pair<std::string, std::string>> cmdOptions = {
            {FLAGS_d, FLAGS_m},
            {FLAGS_d_ag, FLAGS_m_ag},
            {FLAGS_d_hp, FLAGS_m_hp},
            {FLAGS_d_em, FLAGS_m_em},
            {FLAGS_d_lm, FLAGS_m_lm}};
        FaceDetection faceDetector(FLAGS_m, FLAGS_d, 1, false, FLAGS_async, FLAGS_t, FLAGS_r,
                                   static_cast<float>(FLAGS_bb_enlarge_coef), static_cast<float>(FLAGS_dx_coef), static_cast<float>(FLAGS_dy_coef));
        AgeGenderDetection ageGenderDetector(FLAGS_m_ag, FLAGS_d_ag, FLAGS_n_ag, FLAGS_dyn_ag, FLAGS_async, FLAGS_r);
        HeadPoseDetection headPoseDetector(FLAGS_m_hp, FLAGS_d_hp, FLAGS_n_hp, FLAGS_dyn_hp, FLAGS_async, FLAGS_r);
        EmotionsDetection emotionsDetector(FLAGS_m_em, FLAGS_d_em, FLAGS_n_em, FLAGS_dyn_em, FLAGS_async, FLAGS_r);
        FacialLandmarksDetection facialLandmarksDetector(FLAGS_m_lm, FLAGS_d_lm, FLAGS_n_lm, FLAGS_dyn_lm, FLAGS_async, FLAGS_r);

        for (auto &&option : cmdOptions)
        {
            auto deviceName = option.first;
            auto networkName = option.second;

            if (deviceName.empty() || networkName.empty())
            {
                continue;
            }

            if (loadedDevices.find(deviceName) != loadedDevices.end())
            {
                continue;
            }
            slog::info << "Loading device " << deviceName << slog::endl;
            std::cout << ie.GetVersions(deviceName) << std::endl;

            /** Loading extensions for the CPU device **/
            if ((deviceName.find("CPU") != std::string::npos))
            {
#ifdef WITH_EXTENSIONS
                ie.AddExtension(std::make_shared<Extensions::Cpu::CpuExtensions>(), "CPU");
#endif

                if (!FLAGS_l.empty())
                {
                    // CPU(MKLDNN) extensions are loaded as a shared library and passed as a pointer to base extension
                    auto extension_ptr = make_so_pointer<IExtension>(FLAGS_l);
                    ie.AddExtension(extension_ptr, "CPU");
                    slog::info << "CPU Extension loaded: " << FLAGS_l << slog::endl;
                }
            }
            else if (!FLAGS_c.empty())
            {
                // Loading extensions for GPU
                ie.SetConfig({{PluginConfigParams::KEY_CONFIG_FILE, FLAGS_c}}, "GPU");
            }

            loadedDevices.insert(deviceName);
        }

        /** Per-layer metrics **/
        if (FLAGS_pc)
        {
            ie.SetConfig({{PluginConfigParams::KEY_PERF_COUNT, PluginConfigParams::YES}});
        }
        // ---------------------------------------------------------------------------------------------------

        // --------------------------- 2. Reading IR models and loading them to plugins ----------------------
        // Disable dynamic batching for face detector as it processes one image at a time
        Load(faceDetector).into(ie, FLAGS_d, false);
        Load(ageGenderDetector).into(ie, FLAGS_d_ag, FLAGS_dyn_ag);
        Load(headPoseDetector).into(ie, FLAGS_d_hp, FLAGS_dyn_hp);
        Load(emotionsDetector).into(ie, FLAGS_d_em, FLAGS_dyn_em);
        Load(facialLandmarksDetector).into(ie, FLAGS_d_lm, FLAGS_dyn_lm);
        // ----------------------------------------------------------------------------------------------------

        // --------------------------- 3. Doing inference -----------------------------------------------------
        // Starting inference & calculating performance
        slog::info << "Start inference " << slog::endl;

        bool isFaceAnalyticsEnabled = ageGenderDetector.enabled() || headPoseDetector.enabled() ||
                                      emotionsDetector.enabled() || facialLandmarksDetector.enabled();


        if (FLAGS_fps > 0)
        {
            msrate = 1000.f / FLAGS_fps;
        }

        Visualizer::Ptr visualizer;
        if (!FLAGS_no_show || !FLAGS_o.empty())
        {
            visualizer = std::make_shared<Visualizer>(cv::Size(width, height));
            if (!FLAGS_no_show_emotion_bar && emotionsDetector.enabled())
            {
                visualizer->enableEmotionBar(emotionsDetector.emotionsVec);
            }
        }

        // Detecting all faces on the first frame and reading the next one
        faceDetector.enqueue(frame);
        faceDetector.submitRequest();


        prev_frame = cv::Mat();
        prev_frame = frame.clone();

        // Reading the next frame
        if(processFolder)
        {
            cv::Mat newFrame = cv::imread(fn[framesCounter]);
            newFrame.copyTo(frame);

            if(!frame.data)                              // Check for invalid input
            {
                slog::info <<  "Could not open: " << fn[framesCounter] << slog::endl;
            }
            frameReadStatus = !frame.empty();
        }
        else
        {
            frameReadStatus = cap.read(frame);
        }

        //frameReadStatus = !frame.empty();
        

        std::cout << "To close the application, press 'CTRL+C' here";
        if (!FLAGS_no_show)
        {
            std::cout << " or switch to the output window and press any key";
        }
        std::string currFilename("");
        while (true)
        {
            timer.start("total");
            framesCounter++;
            isLastFrame = !frameReadStatus;

            // Retrieving face detection results for the previous frame
            faceDetector.wait();
            faceDetector.fetchResults();
            auto prev_detection_results = faceDetector.results;

            // No valid frame to infer if previous frame is the last
            if (!isLastFrame)
            {
                faceDetector.enqueue(frame);
                faceDetector.submitRequest();
            }
            // Filling inputs of face analytics networks
            for (auto &&face : prev_detection_results)
            {
                if (isFaceAnalyticsEnabled)
                {
                    auto clippedRect = face.location & cv::Rect(0, 0, width, height);
                    cv::Mat face = prev_frame(clippedRect);
                    ageGenderDetector.enqueue(face);
                    headPoseDetector.enqueue(face);
                    emotionsDetector.enqueue(face);
                    facialLandmarksDetector.enqueue(face);
                }
            }

            // Running Age/Gender Recognition, Head Pose Estimation, Emotions Recognition, and Facial Landmarks Estimation networks simultaneously
            if (isFaceAnalyticsEnabled)
            {
                ageGenderDetector.submitRequest();
                headPoseDetector.submitRequest();
                emotionsDetector.submitRequest();
                facialLandmarksDetector.submitRequest();
            }

            // Reading the next frame if the current one is not the last
            if(processFolder && !isLastFrame && framesCounter < fn.size())
            {
                cv::Mat newFrame = cv::imread(fn[framesCounter]);
                newFrame.copyTo(next_frame);
                if(next_frame.empty())
                {
                    slog::info <<  "Could not open: " << fn[framesCounter] << slog::endl;
                }
                frameReadStatus = !next_frame.empty();
            }
            
            else if (!processFolder && !isLastFrame)
            {
                frameReadStatus = cap.read(next_frame);
                if (FLAGS_loop_video && !frameReadStatus)
                {
                    if (!(FLAGS_i == "cam" ? cap.open(0) : cap.open(FLAGS_i)))
                    {
                        throw std::logic_error("Cannot open input file or camera: " + FLAGS_i);
                    }
                    frameReadStatus = cap.read(next_frame);
                }
            }

            if (isFaceAnalyticsEnabled)
            {
                ageGenderDetector.wait();
                headPoseDetector.wait();
                emotionsDetector.wait();
                facialLandmarksDetector.wait();
            }

            //  Postprocessing
            std::list<Face::Ptr> prev_faces;

            if (!FLAGS_no_smooth)
            {
                prev_faces.insert(prev_faces.begin(), faces.begin(), faces.end());
            }

            faces.clear();

            if(processFolder)
            {
                if(framesCounter < 2)
                {
                    currFilename = getFileName(fn[framesCounter - 1]);
                }
                else
                {
                    currFilename = getFileName(fn[framesCounter - 2]);
                }
            }
            else if(!processVideo) // process single file
            {
                currFilename = "output.jpg";
            }
            
            if(prev_detection_results.size() == 0)
            {
                if(processFolder)
                {
                    result_file << currFilename << ", 0, -, -, -, -, -, -; \n";
                }
            }
            // For every detected face
            for (size_t i = 0; i < prev_detection_results.size(); i++)
            {
                auto &result = prev_detection_results[i];
                cv::Rect rect = result.location & cv::Rect(0, 0, width, height);

                Face::Ptr face;
                if (!processFolder && !FLAGS_no_smooth)
                {
                    face = matchFace(rect, prev_faces);
                    float intensity_mean = calcMean(prev_frame(rect));

                    if ((face == nullptr) ||
                        ((face != nullptr) && ((std::abs(intensity_mean - face->_intensity_mean) / face->_intensity_mean) > 0.07f)))
                    {
                        face = std::make_shared<Face>(id++, rect);
                    }
                    else
                    {
                        prev_faces.remove(face);
                    }

                    face->_intensity_mean = intensity_mean;
                    face->_location = rect;
                }
                else
                {
                    face = std::make_shared<Face>(id++, rect);
                }

                // a line in the output file consists of the following elements (BB = Bound box of the detected face)
                // frameIdx, BB_upperLeftCorner.x, BB_upperLeftCorner.y, BB_height, BB_width, isFemale, age;
                if(processFolder){
                    result_file << currFilename << ",";
                }
                else
                {
                    result_file << framesCounter << ", ";   
                }
                
                
                result_file << "1, " << rect.tl().x << ", " << rect.tl().y << ", "; // face_detected (true), get the top left corner
                result_file << rect.height << ", " << rect.width << ", ";  // write height and width from bounding box to file

                face->ageGenderEnable((ageGenderDetector.enabled() &&
                                       i < ageGenderDetector.maxBatch));
                if (face->isAgeGenderEnabled())
                {
                    AgeGenderDetection::Result ageGenderResult = ageGenderDetector[i];
                    face->updateGender(ageGenderResult.maleProb);
                    face->updateAge(ageGenderResult.age);
                    result_file << !face->isMale() << ", " << ageGenderResult.age << ";\n";
                }
                else
                {
                    result_file << "-, -\n";
                }

                face->emotionsEnable((emotionsDetector.enabled() &&
                                      i < emotionsDetector.maxBatch));
                if (face->isEmotionsEnabled())
                {
                    face->updateEmotions(emotionsDetector[i]);
                }

                face->headPoseEnable((headPoseDetector.enabled() &&
                                      i < headPoseDetector.maxBatch));
                if (face->isHeadPoseEnabled())
                {
                    face->updateHeadPose(headPoseDetector[i]);
                }

                face->landmarksEnable((facialLandmarksDetector.enabled() &&
                                       i < facialLandmarksDetector.maxBatch));
                if (face->isLandmarksEnabled())
                {
                    face->updateLandmarks(facialLandmarksDetector[i]);
                }

                faces.push_back(face);
            }

            //  Visualizing results
            if (!FLAGS_no_show || !FLAGS_o.empty())
            {
                out.str("");
                //out << "Total image throughput: " << std::fixed << std::setprecision(2) << 1000.f / (timer["total"].getSmoothedDuration()) << " fps";
                cv::putText(prev_frame, out.str(), cv::Point2f(10, 45), cv::FONT_HERSHEY_TRIPLEX, 1.2,
                            cv::Scalar(255, 0, 0), 2);


                visualizer = std::make_shared<Visualizer>(cv::Size(width, height));
        
                // drawing faces
                visualizer->draw(prev_frame, faces);

                if (!FLAGS_no_show && !processVideo)
                {
                    cv::imshow("Detection results", prev_frame);
                    std::stringstream filename_ss;
                    filename_ss << results_path << currFilename;
                    cv::imwrite(filename_ss.str(), prev_frame);
                }

                // update global width and height values 
                // used for classification and visualization
                width = static_cast<size_t>(frame.cols);
                height = static_cast<size_t>(frame.rows);
            }

            if (processVideo)
            {
                videoWriter.write(prev_frame);
            }
            frameReadStatus = !next_frame.empty();
            frame.copyTo(prev_frame);
            next_frame.copyTo(frame);
            next_frame = cv::Mat();

            timer.finish("total");

            if (FLAGS_fps > 0)
            {
                delay = std::max(1, static_cast<int>(msrate - timer["total"].getLastCallDuration()));
            }

            // End of file (or a single frame file like an image). The last frame is displayed to let you check what is shown
            if (isLastFrame)
            {
                if (!FLAGS_no_wait)
                {
                    std::cout << "No more frames to process!" << std::endl;
                    //cv::waitKey(0);
                }
                break;
            }
            else if (!FLAGS_no_show && -1 != cv::waitKey(delay))
            {
                break;
            }
        }

        slog::info << "Number of processed frames: " << framesCounter << slog::endl;
        slog::info << "Total image throughput: " << framesCounter * (1000.f / timer["total"].getTotalDuration()) << " fps" << slog::endl;

        // Showing performance results
        if (FLAGS_pc)
        {
            faceDetector.printPerformanceCounts(getFullDeviceName(ie, FLAGS_d));
            ageGenderDetector.printPerformanceCounts(getFullDeviceName(ie, FLAGS_d_ag));
            headPoseDetector.printPerformanceCounts(getFullDeviceName(ie, FLAGS_d_hp));
            emotionsDetector.printPerformanceCounts(getFullDeviceName(ie, FLAGS_d_em));
            facialLandmarksDetector.printPerformanceCounts(getFullDeviceName(ie, FLAGS_d_lm));
        }
        // ---------------------------------------------------------------------------------------------------
        result_file.close();
        if (processVideo)
        {
            videoWriter.release();
        }

        // release input video stream
        cap.release();

        // close windows
        cv::destroyAllWindows();
    }
    catch (const std::exception &error)
    {
        slog::err << error.what() << slog::endl;
        return 1;
    }
    catch (...)
    {
        slog::err << "Unknown/internal exception happened." << slog::endl;
        return 1;
    }

    slog::info << "Execution successful" << slog::endl;
    return 0;
}
