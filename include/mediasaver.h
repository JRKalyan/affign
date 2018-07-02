#pragma once

#include <opencv2/core/mat.hpp>

class MediaSaver {
public:
  virtual void Save(const cv::Mat& mat, const std::string& name) = 0;
  MediaSaver(const std::string& outputdirectory);
protected:
  std::string outputdirectory;
};