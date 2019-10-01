/// Name: affignmain.cpp
/// Purpose: Affign Entry Point
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

#include "consolelogger.h"
#include "commandparser.h"
#include "mediamaker.h"

#include <iostream>

int main(int argc, char** argv) {
  std::shared_ptr<Logger> logger(new ConsoleLogger());
  CommandParser commandparser(argc, argv);
  try
  {
    commandparser.ParseArguments();
  }
  catch (std::exception& e)
  {
    logger->Log(e.what(), MessageType::error);
    return -1;
  }
  if (commandparser.config.help)
  {
    // TODO - properly create help and version messages
    std::cout << "Affign Help:" << std::endl;
    std::cout << "This is a temporary message, please visit:" << std::endl;
    std::cout << "https://github.com/JRKalyan/affign/wiki/0.2-Usage" << std::endl;
    return 0;
  }
  if (commandparser.config.version)
  {
    // TODO
    std::cout << "Affign Version 0.2.0" << std::endl;
    return 0;
  }
  if (commandparser.config.silent) {
    // TODO - ensure that a silent logger is passed in
  }
  try {
    MediaMaker maker(commandparser.config.alignerconfig, logger);
    maker.Make();
  }
  catch (std::exception& e) {
    logger->Log(e.what(), MessageType::error);
    return -1;
  }
