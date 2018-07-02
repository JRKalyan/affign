#include "imagesaver.h"
#include <opencv2/imgcodecs.hpp>
#include <experimental/filesystem>

ImageSaver::ImageSaver(const std::string& outputdirectory,
  const std::string& extension) 
  : extension(extension), MediaSaver(outputdirectory) {
}

void ImageSaver::Save(const cv::Mat& mat, const std::string& name) {
  std::string fullname = outputdirectory + "\\" + name + "_aligned." + extension;
  if (!cv::imwrite(fullname, mat))
  {
    throw std::exception("Could not write image");
  }
}