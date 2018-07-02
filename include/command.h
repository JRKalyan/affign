#pragma once

#include <string>
#include "configuration.h"

class Command {
public:
  // Constructs a command that takes a value
  Command(std::string name,
    std::string shortname,
    bool isrequired,
    void(*setter)(Configuration&, const std::string&)
  );
  // Constructs a ommand that takes no value
  Command(std::string name,
    std::string shortname,
    void(*setter)(Configuration&)
  );

  const std::string name;
  const std::string shortname;
  bool parsed = false;
  const bool isrequired;
  const bool takesvalue;

  void SetConfig(Configuration& config, const std::string& value);
  bool IsMatch(const std::string& arg);

  static bool BoolParse(const std::string& value);
  static Extension ExtensionParse(const std::string& value);
  static double DoubleParse(const std::string& value);

private:
  void(*valuesetter)(Configuration&, const std::string&);
  void(*singlesetter)(Configuration&);
};

// Setter Functions for Commands
// *****************************
void SetInputDirectory(Configuration& config, const std::string& value);
void SetOutputDirectory(Configuration& config, const std::string& value);
void SetMakeVideo(Configuration& config, const std::string& value);
void SetFrameRate(Configuration& config, const std::string& value);
void SetRecursive(Configuration& config, const std::string& value);
void SetHelp(Configuration& config);
void SetVersion(Configuration& config);
void SetSilent(Configuration& config);
void SetModelPath(Configuration& config, const std::string& value);
void SetDetectorPath(Configuration& config, const std::string& value);
void SetVideoStem(Configuration& config, const std::string& value);
void SetImageType(Configuration& config, const std::string& value);
void SetEnableTransform(Configuration& config, const std::string& value);