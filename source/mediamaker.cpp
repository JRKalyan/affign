#include "mediamaker.h"
#include "opencv2/opencv.hpp"

#include "transformprocessor.h"
#include "landmarkextractor.h"
#include "mediasaver.h"
#include "imagesaver.h"

MediaMaker::MediaMaker(const AlignerConfiguration& config, 
  std::shared_ptr<Logger> logger)
  : logger(logger), config(config) {
  logger->Log("Initializing Media Maker");
  // populate files
  try {
    if (config.recursive) {
      CopyFiles<fs::recursive_directory_iterator>(files,
        fs::recursive_directory_iterator(config.inputdirectory));
    }
    else {
      CopyFiles<fs::directory_iterator>(files,
        fs::directory_iterator(config.inputdirectory));
    }
  }
  catch (std::exception& e)
  {
    std::string msg = "Failed to grab files from " + config.inputdirectory 
      + ": " + e.what();
    throw std::exception(msg.c_str());
  }
  if (files.size() < 1) {
    std::string msg = "There are no files in " + config.inputdirectory;
    throw std::exception(msg.c_str());
  }
  // create data extractors
  if (config.enabletransform) {
    // only add in data extractors if one or more requiring processors require it
    try {
      extractors.emplace_back(new LandmarkExtractor(config.detectorpath, config.modelpath));
    }
    catch (std::exception& e) {
      std::string msg("Could not create landmark extractor: ");
      msg += e.what();
      throw std::exception(msg.c_str());
    }
  }
  // Set the reference data
  try {
    // To add a config option, implement overloads for SetReference 
    SetReference();
  }
  catch (std::exception& e) {
    std::string msg = "Could not set the reference: ";
    msg += e.what();
    throw std::exception(msg.c_str());
  }
  // create processors
  if (config.enabletransform) {
    processors.emplace_back(new TransformProcessor());
  }
  // Initialize processors from reference data
  for (auto& processor : processors) {
    try {
      processor->InitializeFromReference(referencedata);
    }
    catch (std::exception& e) {
      std::string msg = "Could not initialize processors from the reference: ";
      msg += e.what();
      throw std::exception(msg.c_str());
    }
  }
}

void MediaMaker::Make() {
  // determine the correct saver format (should be done at make-time)
  // (so they are destroyed after each Make, and video savers can add
  //  closing logic to destructors)
  std::unique_ptr<MediaSaver> saver;
  try {
    if (config.makevideo) {
      // TODO
    }
    else {
      saver = std::unique_ptr<MediaSaver>(
        new ImageSaver(config.outputdirectory, EXTENSION[config.imagetype]));
    }
  }
  catch (std::exception& e) {
    std::string msg = "Failed to create media saver for " 
      + config.outputdirectory + ": " + e.what();
    logger->Log(msg, MessageType::error);
    return;
  }
  // process files
  for (auto& file : files) {
    std::string filename = file.filename().string();
    cv::String cvstringpath(file.string());
    cv::Mat current = cv::imread(cvstringpath, cv::IMREAD_COLOR);
    data.imagedata.size = current.size();
    cv::Mat previous;
    if (current.empty()) {
      logger->Log("Failed to read file: " + filename, MessageType::warning);
      continue;
    }
    try {
      for (auto& getter : extractors) {
        getter->Extract(current, data.imagedata);
      }
      for (auto& processor : processors) {
        previous = current;
        processor->Process(previous, current, data);
      }
      saver->Save(current, file.stem().string());
      data.imagesprocessed++;
    }
    catch (std::exception& e) {
      logger->Log("Failed to process " + filename + ": " + e.what(), 
        MessageType::warning);
      continue;
    }
    logger->Log("Successfully processed " + filename, MessageType::success);
  }
  logger->Log("Media Processing Complete", MessageType::standard);
  std::stringstream processedstream;
  processedstream << "IMAGES PROCESSED: " << data.imagesprocessed;
  logger->Log(processedstream.str());
}

/// Grabs the image data from first image to be used as the reference image
/// Requires: extractors and files are populated with at least 1 file
void MediaMaker::SetReference() {
  cv::String path = files.begin()->string();
  cv::Mat reference = cv::imread(path, cv::IMREAD_COLOR);
  if (reference.empty()) {
    throw std::exception("Could not load reference");
  }
  for (auto& extractor : extractors) {
    extractor->Extract(reference, referencedata);
  }
  referencedata.size = reference.size();
}