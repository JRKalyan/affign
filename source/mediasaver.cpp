#include "mediasaver.h"
#include <experimental\filesystem>

MediaSaver::MediaSaver(const std::string& outputdirectory)
  : outputdirectory(outputdirectory) {
  if (!std::experimental::filesystem::exists(outputdirectory)) {
    throw std::exception("Directory does not exist.");
  }
}