#pragma once

#include "imagedata.h"

/// Contains Meta Data about the process along with pure image data
class ProcessData {
public:
  int imagesprocessed = 0;
  ImageData imagedata;
};
