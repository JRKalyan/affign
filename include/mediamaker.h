#pragma once

#include "alignerconfiguration.h"
#include "logger.h"
#include "imagedata.h"
#include "mediasaver.h"
#include "imagesaver.h"
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
  std::unique_ptr<MediaSaver> saver;
  ImageData data;

  std::list<fs::path> files;
  std::list<std::unique_ptr<DataExtractor>> extractors;
  std::list<std::unique_ptr<Processor>> processors;

  template <typename DirectoryIterator>
  static void CopyFiles(std::list<fs::path>& files, DirectoryIterator it);
};

template <typename DirectoryIterator>
void MediaMaker::CopyFiles(std::list<fs::path>& files, DirectoryIterator it) {
  auto last = std::experimental::filesystem::end(it);
  while (it != last) {
    if (it->status().type() == std::experimental::filesystem::file_type::regular) {
      files.emplace_back(it->path());
    }
    it++;
  }
}