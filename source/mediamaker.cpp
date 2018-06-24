#include "mediamaker.h"
#include "opencv2/opencv.hpp"

#include "transformprocessor.h"
#include "landmarkextractor.h"

MediaMaker::MediaMaker(const AlignerConfiguration& config, 
  std::shared_ptr<Logger> logger)
  : logger(logger),
    saver(new ImageSaver()) {
  logger->Log("Initializing Media Maker");
  // populate files
  if (config.recursive) {
    CopyFiles<fs::recursive_directory_iterator>(files, 
      fs::recursive_directory_iterator(config.inputdirectory));
  }
  else {
    CopyFiles<fs::directory_iterator>(files,
      fs::directory_iterator(config.inputdirectory));
  }
  // create data extractors
  if (config.enabletransform) {
    // only add in data extractors if one or more requiring processors require it
    extractors.emplace_back(new LandmarkExtractor());
  }

  // create processors
  if (config.enabletransform) {
    processors.emplace_back(new TransformProcessor());
  }

  // create image saver
  if (config.makevideo) {
    // TODO
  }
  else {
    // TODO
  }
}

void MediaMaker::Make() {
  for (auto& file : files) {
    std::string filename = file.filename().string();
    cv::String cvstringpath(file.string());
    cv::Mat mat = cv::imread(cvstringpath, cv::IMREAD_COLOR);
    if (mat.empty()) {
      logger->Log("Failed to read file: " + filename, MessageType::warning);
      continue;
    }
    // TODO - determine best way to log (all in one line? keep similarly)
    try {
      for (auto& getter : extractors) {
        getter->Extract(mat, data);
        logger->Log("Successfully Extracted Data From: " + filename, MessageType::success);
      }
      for (auto& processor : processors) {
        processor->Process(mat, data);
        logger->Log("Successfully Processed: " + filename, MessageType::success);
      }
      saver->Save(mat);
    }
    catch (std::exception& e) {
      logger->Log("Failed to process " + filename + ": " + e.what(), 
        MessageType::error);
    }
  }
  logger->Log("Media Processing Complete", MessageType::standard);
}

// TODO - somehow figure out if direcotry iterator fails and report that
// because right now there is no way, I could do directory exists, but then
// maybe the constructor for iterator fails and we just do a foreach loop
// where hte begin is always equal to the end....
// so I should figure out how to detect that and report it
// looks like you can just poll the directory iterator using exists: exists(it->status())
// supposedly more efficient: http://en.cppreference.com/w/cpp/filesystem/exists

// TODO 2 - find out why the program took so long to finish...