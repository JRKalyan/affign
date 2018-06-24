#pragma once

#include "configuration.h"
#include "command.h"
#include <exception>
#include <list>
#include <memory>

class Command;

class CommandParser {
public:
  CommandParser(int argc, char** argv);
  void ParseArguments();
  Configuration config;

private:

  char** argv;
  int argc;
  int i = 1;
  std::list<Command> commands;
  void FillCommands();

};

