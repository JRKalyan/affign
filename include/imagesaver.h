#pragma once

#include "mediasaver.h"

class ImageSaver : public MediaSaver {
public:
  // should probably be constructed with a directory and then pass in a filename
  bool Save(const cv::Mat& mat) override;
};
