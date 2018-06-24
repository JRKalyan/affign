#pragma once

#include <string>

enum MessageType {
  standard,
  success,
  warning,
  error
};

class Logger {
public:
  virtual void Log(std::string msg, MessageType type = MessageType::standard) = 0;
};