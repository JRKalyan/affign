#pragma once

#include "logger.h"

class ConsoleLogger : public Logger {
public:
  void Log(std::string msg, MessageType type = MessageType::standard) override;
};
