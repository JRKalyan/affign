#pragma once

#include "imagedata.h"
#include "opencv2/core/mat.hpp"

class DataExtractor {
public:
  /// attempts to extract relevant image data, returns success
  virtual void Extract(const cv::Mat& mat, ImageData& data) = 0;
};