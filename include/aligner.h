/// Name: affignaligner.h
/// Purpose: Aligner class for image alignment
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

/*

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>

#include "aligner.h"
#include "alignerconfiguration.h"

/*
organize(const ALignerConfig& config) { // constructs executor and throws invalid config
executor ex; // executors contain list of processors
// also a list of processors with no detection
// also have detector (or they are a non-detecting exeutor)
// also have a saver (eg VideoSaver ImageSaver) which implement say .Save(mat)
// processors implement: Mat Process(Mat input) which should be by reference, which opencv is?
// then executors decide their workflow with .execute()
// executor without detection iterates over it
if aligner.video: ex.Add(Saver())
else if 

extensibility:
adding in a new effect is adding in a processor
processors may take special data and require a datagetter
datagetters are what the detector is
maybe we want lighting data for effects to use

datagetters have special structure in our for loop template
we get data, and have special reference to that data baked into our executor
- ditch having different executor classes, all classes have all data getters
- they just might not have them all actually initialized
- then in each execute, we do foreach datagetter (could be empty) populate our hard baked data fields
- then foreach processor, process referencing our hard baked data fields
- trust that when organizing the executor we added the correct data getters or threw if they
  told us we didnt need certain data getters when we do (don't perform this check multiple times)
- don't even need to throw an exception because I'm the one guaranteeing that the executor was
  well formed
- so we don't need executor.execute, we can just have our executor be the file grabber
  (which can be a separated class) -> foreach do the steps mentioned based on the configurer's populating me

so some class (MediaMaker) has a Make(config) method, inside this method we 
 1) organize and build our executor - according to above
 2) start the executor - according to directly above
the organization doesn't have to be split into a different method but perhaps it should

Make(config) {
Executor executor = BuildExecutor(config);
executor.Start();
// I mean fuck executor class, just make it a method
// execute(GetDataGetters(config), GetProcessors(config))
// or construct those lists as variables if the validation logic needs
}

Executor { // new design is no executor class, but media maker class itself has these fields?
// maybe just for separation it is nice to keep these fields in a different class
datafields:
data field 1
data field 2
data field ... (hardcode as many as I want)
list data getters // populate data fields
list processors // where processors are split into categories according to data field
file grabber
image reader
image saver

Start() {
files = filegrabber.Go();
foreach file in files:
mat = imagereader.read(file);
foreach data getter {
 getter.populate(mat, datafields) // populates the data fields of this obejct
}
foreach processor {
processor.process(mat, datafields) // use data fields to alter mat (or mat=process(mat, fields))
}
imagesaver.Save(mat);
}

// meta: data getters should be a folder
// processors should be a folder
DataGetter {
void Populate(Mat mat, Executor executor) // OR executors have a datafields struct
// and this populate deals with data fields instead, decreading dependency on executor
// both depend on datafields struct
}
// data getters are not even noticable by user, the don't disable them or not
// we determine if they need to be added or not
// --disablealignment true is equivalent to disabling the rotator processor, not detector


struct ImageDataFields {
  vector<point> landmarks;
  vector<LightDatum> lightdata;
  etc.. (extensible, currently I just have one)
}

the workflow is not all in one place, but relatively simple
new feature -> add in arguments to configure feature
 relies on new data? add in data getter and when feature is enabled, ensure to add it
 and add in data field to imagedatafields
 relies on oldata? just make sure that when overriding process, we use the already there field
   in datafields parameter
 



class Aligner
{
public:
  void Start(AlignerConfiguration& config); // configures 

private:
  // idea is there is a single responsibility
  // but in order to achieve this, I need a set of parts that are well defined, necessary
  // so I have a bunch of members that all implement interfaces for required behaviour,
  // but they can implement them in whichever way I want.
  // outputter - an output class that saves an image (can be a video or image)
  // imagecreator - responsible for performing


public:
	
	// constructor
	Aligner();

	// public methods
  // these should not be public
	void Align(const char* inputImagePath, const char* outputImagePath);
	void SetReference(const char* imagePath);
	void AddVideo(const char* videoPath, int fps);
	void RemoveVideo();

private:

	std::vector<cv::Point> referencePoints;
	cv::Size referenceSize;

	dlib::shape_predictor shapePredictor;
	dlib::frontal_face_detector faceDetector;
	cv::VideoWriter vwriter;

	// private methods
	std::vector<cv::Point> Landmarks(cv::Mat& matImage, int faceIndex = 0);

	// constants
	const std::string predictorPath = "predictor.dat";
	const int leftEyeIndex = 39;
	const int rightEyeIndex = 42;
};
*/
