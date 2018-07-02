#pragma once

#include <string>
#include "extension.h"

// eventually for example we add outputnamemode (or something)
// would have enum like append, numberedroot

struct AlignerConfiguration {
  // Processors enabled
  bool enabletransform = true;
  bool makevideo = false;
  bool recursive = false;
  Extension imagetype = bmp;
  double framerate = 30;
  std::string modelpath = "predictor.yaml";
  std::string detectorpath = "detector.xml";
  std::string videostem = "default";
  std::string inputdirectory;
  std::string outputdirectory;
};
