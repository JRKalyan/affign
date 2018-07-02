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

namespace fs = std::experimental::filesystem;

/// Responsible for producing Affign output
class MediaMaker {
public:
  MediaMaker(const AlignerConfiguration& config, std::shared_ptr<Logger>);
  void Make();
private:
  std::shared_ptr<Logger> logger;
  ProcessData data;
  ImageData referencedata;
  // store reference to the config for make-time configuration
  const AlignerConfiguration& config;

  std::list<fs::path> files;
  std::list<std::unique_ptr<DataExtractor>> extractors;
  std::list<std::unique_ptr<Processor>> processors;

  void SetReference();
  template <typename DirectoryIterator>
  static void CopyFiles(std::list<fs::path>& files, DirectoryIterator& it);
};

template <typename DirectoryIterator>
void MediaMaker::CopyFiles(std::list<fs::path>& files, DirectoryIterator& it) {
  if (!fs::exists(it->status())) {
    throw std::exception("Directory does not exist");
  }
  auto last = fs::end(it);
  while (it != last) {
    if (it->status().type() == fs::file_type::regular) {
      files.emplace_back(it->path());
    }
    it++;
  }
}