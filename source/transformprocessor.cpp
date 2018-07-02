#include "transformprocessor.h"
#include <opencv2/imgproc.hpp>

 void TransformProcessor::Process(const cv::Mat& in, cv::Mat& out, const ProcessData& data) {
   double imangle;
   double imdistance;
   cv::Point2f imlefteye;
   GetData(imangle, imdistance, imlefteye, data.imagedata);
   // rotate image by:
   // negative because rotation matrix treats positive as counter clockwise,
   // I calculate angles as if we flipped on x (which we did)
   double angle = imangle - refangle; 
   // scale image by:
   double scale = refdistance / imdistance;
   cv::Mat rotationmat = cv::getRotationMatrix2D(cv::Point2f(0, 0), angle, scale);
   // transform a point so we can calculate what to translate image by after rotating
   cv::Mat oldlefteyemat(3, 1, CV_64FC1);
   oldlefteyemat.at<double>(0, 0) = imlefteye.x;
   oldlefteyemat.at<double>(1, 0) = imlefteye.y;
   oldlefteyemat.at<double>(2, 0) = 1.0;
   cv::Mat newlefteyemat = rotationmat * oldlefteyemat;
   // add the translation to the rotation matrix
   rotationmat.at<double>(0, 2) += reflefteye.x - newlefteyemat.at<double>(0, 0);
   rotationmat.at<double>(1, 2) += reflefteye.y - newlefteyemat.at<double>(1, 0);
   // apply the transformation matrix
   cv::warpAffine(in, out, rotationmat, destsize);
}

void TransformProcessor::InitializeFromReference(const ImageData& referencedata) {
  GetData(refangle, refdistance, reflefteye, referencedata);
  destsize = referencedata.size;
}

double TransformProcessor::CalculateDistance(const cv::Point2f& p1, const cv::Point2f& p2) {
  return cv::norm(p1 - p2);
}

double TransformProcessor::CalculateAngle(const cv::Point2f& p1, const cv::Point2f& p2) {
  return cv::fastAtan2(p2.y - p1.y, p2.x - p1.x);
}

void TransformProcessor::GetData(double& angle, 
  double& distance, 
  cv::Point2f& p, 
  const ImageData& data) {
  const cv::Point2f& lefteye = data.landmarks[0][ImageData::LEFT_EYE_INDEX];
  const cv::Point2f& righteye = data.landmarks[0][ImageData::RIGHT_EYE_INDEX];
  distance = CalculateDistance(lefteye, righteye);
  angle = CalculateAngle(lefteye, righteye);
  p = lefteye;
}