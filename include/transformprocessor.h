#pragma once

#include "processor.h"

class TransformProcessor : public Processor {
public:
  void Process(const cv::Mat& in, cv::Mat& out, const ProcessData& data) override;
  virtual void InitializeFromReference(const ImageData& referencedata) override;
private:
  double refangle;
  double refdistance;
  cv::Point2f reflefteye;
  cv::Size destsize;

  static double CalculateDistance(const cv::Point2f& p1, const cv::Point2f& p2);
  static double CalculateAngle(const cv::Point2f& p1, const cv::Point2f& p2);
  static void GetData(double& angle, double& distance, cv::Point2f& p, const ImageData& data);
};