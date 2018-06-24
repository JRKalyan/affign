#pragma once

#include <opencv2/core/mat.hpp>

class MediaSaver {
public:
  // Should probably save with a filename
  virtual bool Save(const cv::Mat& mat) = 0;
};