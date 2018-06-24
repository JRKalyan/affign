#pragma once

#include <vector>
#include "opencv2/core/types.hpp"

/// Holds data that processors may require to process an iamge
struct ImageData {
  static const int LANDMARK_COUNT = 68;
  static const int LEFT_EYE_INDEX = 39;
  static const int RIGHT_EYE_INDEX = 42;
  cv::Point landmarks[LANDMARK_COUNT];
};