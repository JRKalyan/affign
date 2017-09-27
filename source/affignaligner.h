/// Name: affignaligner.h
/// Purpose: Aligner class for image alignment
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

#pragma once

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>

#include "affignexcept.h"

class AffignAligner
{
public:
	
	// constructor
	AffignAligner();

	// public methods
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
	const std::string predictorPath = "../predictor.dat";
	const int leftEyeIndex = 39;
	const int rightEyeIndex = 42;
};