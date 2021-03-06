#include "commandparser.h"
#include <exception>
#include <sstream>
#include <algorithm>

CommandParser::CommandParser(int argc, char** argv)
  : argc(argc), argv(argv) {
  FillCommands();
}

// TODO: take a logger and log the
// exception messages or do that outside
// rather than passing it via exceptions.
void CommandParser::ParseArguments() {
  while (i < argc) {
    std::string arg(argv[i]);
    auto it = commands.begin();
    auto end = commands.end();
    while (true) {
      if (it == end) {
        // no commands matched
        std::string msg = "'" + arg + "' is an unrecognized argument";
        throw std::exception(msg.c_str());
      }
      Command& command = *it;
      if (command.IsMatch(arg)) {
        if (i + command.takesvalue >= argc) {
          std::string msg("Missing argument for: " + arg);
          throw std::exception(msg.c_str());
        }
        std::string value;
        if (command.takesvalue) {
          value = argv[++i];
        }
        try {
          command.SetConfig(config, value);
        }
        catch (std::exception& e) {
          std::string msg = "argument to '" + arg + "' failed: " + e.what();
          throw std::exception(msg.c_str());
        }
        break;
      }
      else {
        it++;
      }
    }
    ++i;
  }
  // TODO - this needs to be fixed
  // as in, help messages shouldn't need required args
  for (auto& command : commands) {
    if (command.isrequired && !command.parsed) {
      std::string msg = "Missing required argument: " + command.name;
      throw std::exception(msg.c_str());
    }
  }
}

// TODO - these should be statically defined
// just use an unnamed namespace
// can still have the runtime struct compiled so I can do runtime checks against
// the static data`
void CommandParser::FillCommands() {
  commands = std::vector<Command>{
    Command("--inputdirectory", "--i", true, SetInputDirectory),
    Command("--outputdirectory", "--o", true, SetOutputDirectory),
    Command("--modelpath", "--m", false, SetModelPath),
    Command("--detectorpath", "--d", false, SetDetectorPath),
    Command("--framerate", "--f", false, SetFrameRate),
    Command("--recursive", "--r", false, SetRecursive),
    Command("--videostem", "--s", false, SetVideoStem),
    Command("--imagetype", "--t", false, SetImageType),
    Command("--enabletransform", "--et", false, SetEnableTransform),
    Command("--makevideo", "--v", false, SetMakeVideo), 
    Command("--help", "--h", SetHelp),
    Command("--silent", "--s", SetSilent),
    Command("--version", "--v", SetVersion)
  };
}