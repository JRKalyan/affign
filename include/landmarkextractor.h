#pragma once

#include "dataextractor.h"

class LandmarkExtractor : public DataExtractor {
  void Extract(const cv::Mat& mat, ImageData& data) override;
};