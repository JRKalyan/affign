#pragma once

#include "imagedata.h"
#include "opencv2/core/mat.hpp"

class Processor {
public:
  virtual void Process(cv::Mat& mat, ImageData data) = 0;
};

