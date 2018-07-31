#pragma once

#include "mediasaver.h"
#include <opencv2/videoio.hpp>

class VideoSaver : public MediaSaver {
public:
  VideoSaver(const std::string& outputdirectory, 
    const std::string& videostem,
    double framerate,
    cv::Size size);
  void Save(const cv::Mat& mat, const std::string& name) override;

private:
  // Writer will be destructed when 
  cv::VideoWriter writer;
};