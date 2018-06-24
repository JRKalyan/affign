#pragma once

#include <string>
#include "alignerconfiguration.h"

struct Configuration {
  bool help = false;
  bool version = false;
  bool silent = false;
  // bool silent;
  AlignerConfiguration alignerconfig;
};