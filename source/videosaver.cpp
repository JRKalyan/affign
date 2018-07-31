#include "videosaver.h"

VideoSaver::VideoSaver(const std::string& outputdirectory, 
  const std::string& videostem,
  double framerate,
  cv::Size size)
  : MediaSaver(outputdirectory) {
  // TODO - use filesystem combine if available
  std::string fullpath = outputdirectory + "//" + videostem + ".avi";
  writer = cv::VideoWriter(fullpath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), framerate, size);
}

void VideoSaver::Save(const cv::Mat& mat, const std::string& name) {
  writer.write(mat);
}