#pragma once
#include<iostream>
#include <opencv2/opencv.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class ORBAlgorithm
{
public:
	ORBAlgorithm();
	~ORBAlgorithm();
	
	void detectPoints(const cv::Mat &img1, const cv::Mat& mask1, const cv::Mat &img2, const cv::Mat& mask2,
		std::vector<cv::KeyPoint>& kp1, std::vector<cv::KeyPoint>& kp2, cv::Mat& des1, cv::Mat& des2);

	//match feature points and return the match points pairs
	void matchPoints(const std::vector<cv::KeyPoint>& kp1, const std::vector<cv::KeyPoint>& kp2,
		const cv::Mat& des1, const cv::Mat& des2, std::vector<cv::DMatch>& matchPairs);


	//use RANSAC to get the homography mat
	void getHomographyMat(const std::vector<cv::KeyPoint>& kp1, const std::vector<cv::KeyPoint>& kp2,
		std::vector<cv::DMatch>& good_matches, cv::Mat& homography);

	void getHomographyMat(const cv::Mat& img1, const cv::Mat& img2, const std::vector<cv::KeyPoint>& kp1,
		const std::vector<cv::KeyPoint>& kp2, std::vector<cv::DMatch>& good_matches, cv::Mat& homography);

	static bool sortByDistance(const cv::DMatch& match1, const cv::DMatch& match2)
	{
		return match1.distance < match2.distance;
	}
	

ORBAlgorithm::ORBAlgorithm()
{
}

ORBAlgorithm::~ORBAlgorithm()
{
}