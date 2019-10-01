#pragma once

#include "alignerconfiguration.h"
#include "logger.h"
#include "imagedata.h"
#include "processdata.h"
#include "dataextractor.h"
#include "processor.h"

#include <list>
#include <exception>
#include <memory>
#include <filesystem>

/// Responsible for producing Affign output
class MediaMaker {
public:
  MediaMaker(const AlignerConfiguration& config, std::shared_ptr<Logger>);
  void Make();
private:
  std::shared_ptr<Logger> m_logger;
  ProcessData m_data;
  ImageData m_referenceData;
  // store reference to the config for make-time configuration
  const AlignerConfiguration& m_config;

  std::vector<std::filesystem::path> m_files;
  std::vector<std::unique_ptr<DataExtractor>> m_extractors;
  std::vector<std::unique_ptr<Processor>> m_processors;

  void SetReference();
};
