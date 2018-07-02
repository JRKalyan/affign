#pragma once

#include "mediasaver.h"

class ImageSaver : public MediaSaver {
public:
  ImageSaver(const std::string& outputdirectory, const std::string& extension);
  void Save(const cv::Mat& mat, const std::string& name) override;
private:
  std::string extension;
};
