#include "command.h"
#include <algorithm>

Command::Command(std::string name,
  std::string shortname,
  bool isrequired,
  void(*setter)(Configuration&, const std::string&))
  : name(name),
  isrequired(isrequired),
  shortname(shortname),
  valuesetter(setter),
  takesvalue(true) {
}

Command::Command(std::string name,
  std::string shortname,
  void(*setter)(Configuration&))
  : name(name),
    shortname(shortname),
    singlesetter(setter),
    takesvalue(false),
    isrequired(false) {
}

bool Command::IsMatch(const std::string& arg) {
  return arg == name || arg == shortname;
}

void Command::SetConfig(Configuration& config, const std::string& value) {
  if (parsed) {
    std::string msg = name + " was provided twice.";
    throw std::exception(msg.c_str());
  }
  parsed = true;
  if (takesvalue) {
    valuesetter(config, value);
  }
  else {
    singlesetter(config);
  }
}

// Parse Helper Methods
// ********************
Extension Command::ExtensionParse(const std::string& value) {
  std::string lower(value);
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
  for (int i = 0; i < EXTENSIONCOUNT; i++) {
    if (lower == EXTENSION[i]) return (Extension)i;
  }
  std::string msg = "'" + value + "' is not a recognized image extension.";
  throw std::exception(msg.c_str());
}

bool Command::BoolParse(const std::string& value) {
  std::string lower(value);
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
  if (lower == "true") return true;
  if (lower == "false") return false;
  std::string msg = "'" + value + "' is not true or false.";
  throw std::exception(msg.c_str());
}

double Command::DoubleParse(const std::string& value) {
  double result;
  try {
    result = std::stod(value);
  }
  catch (std::invalid_argument&) {
    std::string msg = "'" + value + "' is not a valid number.";
    throw std::exception(msg.c_str());
  }
  catch (std::out_of_range&) {
    std::string msg = "'" + value + "' is out of possible range for a number.";
    throw std::exception(msg.c_str());
  }
  return result;
}

// Implement Setters
// *****************
void SetInputDirectory(Configuration& config, const std::string& value) {
  config.alignerconfig.inputdirectory = value;
}

void SetOutputDirectory(Configuration& config, const std::string& value) {
  config.alignerconfig.outputdirectory = value;
}

void SetMakeVideo(Configuration& config, const std::string& value) {
  config.alignerconfig.makevideo = Command::BoolParse(value);
}

void SetFrameRate(Configuration& config, const std::string& value) {
  double rate = Command::DoubleParse(value);
  if (rate < 0 || rate > 500) {
    std::string msg = "'" + value + "' is out of possible range for FPS.";
    throw std::exception(msg.c_str());
  }
  config.alignerconfig.framerate = rate;
}

void SetRecursive(Configuration& config, const std::string& value) {
  config.alignerconfig.recursive = Command::BoolParse(value);
}

void SetHelp(Configuration& config) {
  config.help = true;
}

void SetVersion(Configuration& config) {
  config.version = true;
}

void SetSilent(Configuration& config) {
  config.silent = true;
}

void SetModelPath(Configuration& config, const std::string& value) {
  config.alignerconfig.modelpath = value;
}

void SetDetectorPath(Configuration& config, const std::string& value) {
  config.alignerconfig.detectorpath = value;
}

void SetVideoStem(Configuration& config, const std::string& value) {
  config.alignerconfig.videostem = value;
}

void SetImageType(Configuration& config, const std::string& value) {
  config.alignerconfig.imagetype = Command::ExtensionParse(value);
}

void SetEnableTransform(Configuration& config, const std::string& value) {
  config.alignerconfig.enabletransform = Command::BoolParse(value);
}

