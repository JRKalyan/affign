#pragma once

#include "dataextractor.h"
#include <opencv2/face.hpp>

class LandmarkExtractor : public DataExtractor {
public:
  LandmarkExtractor(std::string detectorpath, std::string modelpath);
  void Extract(const cv::Mat& mat, ImageData& data) override;
private:
  cv::CascadeClassifier facedetector;
  cv::Ptr<cv::face::Facemark> landmarkdetector;
};