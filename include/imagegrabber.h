#pragma once

#include <vector>
#include <string>

class ImageGrabber {
public:
  ImageGrabber(std::string directory);
  // implement the range based for loop requirement
  // either directly as part of this class, so then I can
  // just say foreach image in imagegrabber (maybe rename)
  // or as a member of this class
};