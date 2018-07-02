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
  virtual void Log(const std::string& msg, MessageType type = MessageType::standard) = 0;
};