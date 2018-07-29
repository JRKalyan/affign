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
  const int extractionwidth = 200;
  std::vector<cv::Rect> faces;
  cv::Mat grayscale;
  double scale = (double)extractionwidth / mat.cols;
  cv::resize(mat, grayscale, cv::Size(), scale, scale, cv::INTER_LINEAR_EXACT);
  cv::cvtColor(grayscale, grayscale, cv::COLOR_BGR2GRAY);
  facedetector.detectMultiScale(grayscale, faces);
  // TODO - allow option to bring up selected dialog
  if (faces.size() < 1) {
    throw std::exception("No faces detected");
  }
  if (faces.size() > 1) {
    throw std::exception("More than one face detected");
  }
  // Resizing the rectangle after detecting the face:
  //  I resize after because the bulk of the time is face detection, not landmarks
  //  Also, I tried scaling back the points after landmark detection, and the 
  //  precision loss was more visible, but landmark detection does not care so much
  //  about a facial bound that is slightly off.
  auto& face = faces[0];
  scale = 1 / scale;
  face.x *= scale;
  face.y *= scale;
  face.width *= scale;
  face.height *= scale;
  if (!landmarkdetector->fit(mat, faces, data.landmarks))
  {
    throw std::exception("Could not find landmarks on face");
  }
}
