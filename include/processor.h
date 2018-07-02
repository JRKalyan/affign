#pragma once

#include "processdata.h"
#include "opencv2/core/mat.hpp"

class Processor {
public:
  virtual void Process(const cv::Mat& in, cv::Mat& out, const ProcessData& data) = 0;
  // override this function if special initialization is needed
  // will be called from MediaMaker after reference is set
  virtual void InitializeFromReference(const ImageData& referencedata) {};
};

