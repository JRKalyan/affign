#pragma once

#include "processor.h"

class TransformProcessor : public Processor {
public:
  void Process(cv::Mat& mat, ImageData data) override;
};