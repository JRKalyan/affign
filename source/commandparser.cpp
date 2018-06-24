#include "commandparser.h"
#include <exception>
#include <sstream>
#include <algorithm>

CommandParser::CommandParser(int argc, char** argv)
  : argc(argc), argv(argv) {
  FillCommands();
}

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
  for (auto& command : commands) {
    if (command.isrequired && !command.parsed) {
      std::string msg = "Missing required argument: " + command.name;
      throw std::exception(msg.c_str());
    }
  }
}

void CommandParser::FillCommands() {
  commands = std::list<Command>{
    Command("--inputdirectory", "--i", true, SetInputDirectory),
    Command("--outputdirectory", "--o", true, SetOutputDirectory),
    Command("--modelpath", "--m", false, SetModelPath),
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