#include "consolelogger.h"
#include <iostream>

void ConsoleLogger::Log(std::string msg, MessageType type) {
  std::string prefix;
  switch (type)
  {
  case MessageType::error:
    prefix = "[ERROR]";
    break;
  case MessageType::standard:
    prefix = "[STATUS]";
    break;
  case MessageType::success:
    prefix = "[SUCCESS]";
    break;
  case MessageType::warning:
    prefix = "[WARNING]";
    break;
  }
  std::cout << prefix << " " << msg << std::endl;
}

