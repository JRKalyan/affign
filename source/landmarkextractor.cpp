#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "landmarkextractor.h"

LandmarkExtractor::LandmarkExtractor(std::string detectorpath, std::string modelpath) {
  // TODO - make model type customizable with another option from config
  try {
    facedetector.load(detectorpath);
    if (facedetector.empty()) {
      throw std::exception("Detector is empty");
    }
  }
  catch (std::exception) {
    std::string msg = "Failed to load face detector from " + detectorpath;
    throw std::exception(msg.c_str());
  }
  landmarkdetector = cv::face::FacemarkLBF::create();
  try {
    landmarkdetector->loadModel(modelpath);
  }
  catch (std::exception& e) {
    std::string msg = "Failed to load predictor model from " + modelpath;
    throw std::exception(msg.c_str());
  }
}

void LandmarkExtractor::Extract(const cv::Mat& mat, ImageData& data) {
  std::vector<cv::Rect> faces;
  cv::Mat grayscale;
  cv::cvtColor(mat, grayscale, cv::COLOR_BGR2GRAY);
  facedetector.detectMultiScale(grayscale, faces);
  // TODO - allow option to bring up selected dialog
  if (faces.size() < 1) {
    throw std::exception("No faces detected");
  }
  if (faces.size() > 1) {
    throw std::exception("More than one face detected");
  }
  if (!landmarkdetector->fit(mat, faces, data.landmarks))
  {
    throw std::exception("Could not find landmarks on face");
  }
}
