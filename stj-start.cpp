#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

using namespace cv;
using namespace std;


int main()
{
	//read image first;
	Mat image01 = imread("frame001.tif");
	Mat image02 = imread("frame002.tif");

	//test, read success or not;
	//namedWindow("p1");
	//namedWindow("p2");
   
	//get gray image for next step
	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);

	Ptr<ORB> dete = ORB::create(100);
	//FeatureDetector dete;
	vector<KeyPoint> keypoint1, keypoint2;
	dete->detect(image01, keypoint1);
	dete->detect(image02, keypoint2);

	DescriptorExtractor des;
	Mat imagedes1, imagedes2;
	des.compute(image1, keypoint1, imagedes1);
	des.compute(image2, keypoint2, imagedes2);

	FlannBasedMatcher matcher;
	vector<DMatch> matchpoints;

	matcher.match(imagedes1, imagedes2, matchpoints, Mat());
	cout << "total match points" << matchpoints.size() << endl;

	Mat img_match;
	drawMatches(image01, keypoint1, image02, keypoint2, matchpoints, img_match);

	namedWindow("match");
	imshow("match", img_match);

	waitKey();
	return 0;


}

//looking on the Lowe¡¯s algorithm....