#include "mediamaker.h"
#include "opencv2/opencv.hpp"

#include "transformprocessor.h"
#include "landmarkextractor.h"
#include "mediasaver.h"
#include "imagesaver.h"
#include "videosaver.h"

MediaMaker::MediaMaker(const AlignerConfiguration& config, 
  std::shared_ptr<Logger> logger)
  : m_logger(logger), m_config(config) {
  logger->Log("Initializing Media Maker");
  // populate files
  try {
    if (config.recursive) {
      auto directoryIterator = std::filesystem::recursive_directory_iterator(config.inputdirectory);
      for (const std::filesystem::directory_entry& entry : directoryIterator) {
        if (entry.exists() && entry.is_regular_file()) {
          m_files.emplace_back(entry.path());
        }
      }
    }
    else {
      auto directoryIterator = std::filesystem::directory_iterator(config.inputdirectory);
      for (const std::filesystem::directory_entry& entry : directoryIterator) {
        if (entry.exists() && entry.is_regular_file()) {
          m_files.emplace_back(entry.path());
        }
      }
    }
  }
  catch (std::exception& e)
  {
    std::string msg = "Failed to grab files from " + config.inputdirectory 
      + ": " + e.what();
    throw std::exception(msg.c_str());
  }
  if (m_files.size() < 1) {
    std::string msg = "There are no files in " + config.inputdirectory;
    throw std::exception(msg.c_str());
  }
  // create data extractors
  if (config.enabletransform) {
    // only add in data extractors if one or more requiring processors require it
    try {
      m_extractors.emplace_back(new LandmarkExtractor(config.detectorpath, config.modelpath));
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
    m_processors.emplace_back(new TransformProcessor());
  }
  // Initialize processors from reference data
  for (auto& processor : m_processors) {
    try {
      processor->InitializeFromReference(m_referenceData);
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
    if (m_config.makevideo) {
      // TODO - this should actually be done after finding the reference
      saver = std::unique_ptr<MediaSaver>(
        new VideoSaver(m_config.outputdirectory,
          m_config.videostem,
          m_config.framerate,
          m_referenceData.size));
    }
    else {
      saver = std::unique_ptr<MediaSaver>(
        new ImageSaver(m_config.outputdirectory, EXTENSION[m_config.imagetype]));
    }
  }
  catch (std::exception& e) {
    std::string msg = "Failed to create media saver for " 
      + m_config.outputdirectory + ": " + e.what();
    m_logger->Log(msg, MessageType::error);
    return;
  }
  // process files
  for (auto& file : m_files) {
    std::string filename = file.filename().string();
    cv::String cvstringpath(file.string());
    cv::Mat current = cv::imread(cvstringpath, cv::IMREAD_COLOR);
    m_data.imagedata.size = current.size();
    cv::Mat previous;
    if (current.empty()) {
      m_logger->Log("Failed to read file: " + filename, MessageType::warning);
      continue;
    }
    try {
      for (auto& getter : m_extractors) {
        getter->Extract(current, m_data.imagedata);
      }
      for (auto& processor : m_processors) {
        previous = current;
        processor->Process(previous, current, m_data);
      }
      saver->Save(current, file.stem().string());
      m_data.imagesprocessed++;
    }
    catch (std::exception& e) {
      m_logger->Log("Failed to process " + filename + ": " + e.what(), 
        MessageType::warning);
      continue;
    }
    m_logger->Log("Successfully processed " + filename, MessageType::success);
  }
  m_logger->Log("Media Processing Complete", MessageType::standard);
  std::stringstream processedstream;
  processedstream << "IMAGES PROCESSED: " << m_data.imagesprocessed;
  m_logger->Log(processedstream.str());
}

/// Grabs the image data from first image to be used as the reference image
/// Requires: extractors and files are populated with at least 1 file
void MediaMaker::SetReference() {
  cv::String path = m_files.begin()->string();
  cv::Mat reference = cv::imread(path, cv::IMREAD_COLOR);
  if (reference.empty()) {
    throw std::exception("Could not load reference");
  }
  for (auto& extractor : m_extractors) {
    extractor->Extract(reference, m_referenceData);
  }
  m_referenceData.size = reference.size();
}

// TODO
// - remove exceptions 
// - I have a lot of useless stuff stored on the mediamaker object. I don't really think
//   MediaMaker could just be function
// - make commands to static data
// - fix help text 
//   1. it should display without requiring valid command line args that make it early out
//   2. it should actually show help and not just link to the usage.