/// Name: affignaligner.cpp
/// Purpose: Aligner class for image alignment
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

#include "affignaligner.h"

// Constructor ----------------------------------------------------------------

/// Initializes the face detector and landmark predictor
AffignAligner::AffignAligner()
{
	faceDetector = dlib::get_frontal_face_detector();
	dlib::deserialize(predictorPath) >> shapePredictor;
}

// Public Methods -------------------------------------------------------------

/// Attempts to set the reference image, exception if not possible
void AffignAligner::SetReference(const char* imagePath)
{
	cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
	if (image.empty())
	{
		throw AffignException("image could not be read");
	}

	referencePoints = Landmarks(image);
	referenceSize = image.size();
}

/// attempts to align the input image and saves to output path (or vwriter)
void AffignAligner::Align(const char* inputImagePath, const char* outputImagePath)
{
	cv::Mat image = imread(inputImagePath, cv::IMREAD_COLOR);
	if (image.empty())
	{
		throw AffignException("image could not be read");
	}


	std::vector<cv::Point> landmarks = Landmarks(image);

	cv::Point& leftEyeImg = landmarks[leftEyeIndex];
	cv::Point& rightEyeImg = landmarks[rightEyeIndex];

	cv::Point& leftEyeRef = referencePoints[leftEyeIndex];
	cv::Point& rightEyeRef = referencePoints[rightEyeIndex];

	// calculating scale factor
	double refDistance = cv::norm(leftEyeRef - rightEyeRef);
	double imDistance = cv::norm(leftEyeImg - rightEyeImg);

	if (imDistance == (double)0)
	{
		throw AffignException("invalid image landmarks");
	}

	if (refDistance == (double)0)
	{
		throw AffignException("invalid reference landmarks");
	}

	double scale = refDistance / imDistance;

	// calculating degree rotation
	double refAngle = cv::fastAtan2((rightEyeRef.y - leftEyeRef.y),
		(rightEyeRef.x - leftEyeRef.x));

	double imAngle = cv::fastAtan2((rightEyeImg.y - leftEyeImg.y),
		(rightEyeImg.x - leftEyeImg.x));

	// To compensate for top-left origin, use negative angle
	double rotationAngle = -(refAngle - imAngle);


	// calculating new point position for one eye
	cv::Mat rotationMatrix = cv::getRotationMatrix2D(cv::Point2f(0, 0),
		rotationAngle, scale);

	cv::Mat oldPointMat = cv::Mat(3, 1, CV_64FC1);
	oldPointMat.at<double>(0, 0) = leftEyeImg.x;
	oldPointMat.at<double>(1, 0) = leftEyeImg.y;
	oldPointMat.at<double>(2, 0) = 1.0;

	cv::Mat newPointMat = rotationMatrix * oldPointMat;
	double newPointX = newPointMat.at<double>(0, 0);
	double newPointY = newPointMat.at<double>(1, 0);

	double xTranslation = leftEyeRef.x - newPointX;
	double yTranslation = leftEyeRef.y - newPointY;

	// now I need to update final rotation matrix
	rotationMatrix.at<double>(0, 2) += xTranslation;
	rotationMatrix.at<double>(1, 2) += yTranslation;

	// apply transformations
	cv::Mat warpedImage;
	cv::warpAffine(image, warpedImage, rotationMatrix, referenceSize);
	if (outputImagePath == NULL)
	{
		vwriter.write(warpedImage);
	}
	else
	{
		cv::imwrite(cv::String(outputImagePath), warpedImage);
	}
	return;
}

/// initializes a VideoWriter with specified fps
void AffignAligner::AddVideo(const char* videoPath, int fps)
{
	std::string strVideoPath = videoPath;
	vwriter = cv::VideoWriter(
		strVideoPath,
		-1,
		fps,
		referenceSize);
}

/// releases video writer object
void AffignAligner::RemoveVideo()
{
	vwriter.release();
}

// Private Methods ------------------------------------------------------------

/// attempts to get the landmarks of a face at faceIndex
std::vector<cv::Point> AffignAligner::Landmarks(cv::Mat& matImg, int faceIndex)
{	
	std::vector<cv::Point> result;

	dlib::cv_image<dlib::bgr_pixel> dlibImg(matImg);
	std::vector<dlib::rectangle> detections = faceDetector(dlibImg);

	if (detections.size() < 1) {
		throw AffignException("no faces detected");
	}

	if (detections.size() > 1) {
		throw AffignException("multiple faces detected");
	}

	dlib::full_object_detection face = shapePredictor(dlibImg, 
		detections[faceIndex]);

	int numParts = face.num_parts();

	for (int i = 0; i < numParts; ++i)
	{
		int x = face.part(i).x();
		int y = face.part(i).y();
		result.push_back(cv::Point(x, y));
	}

	return result;


}
