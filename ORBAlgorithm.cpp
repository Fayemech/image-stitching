#include"ORBAlgorithm.h"

using namespace std;
using namespace cv;


void ORBAlgorithm::detectPoints(const Mat &img1, const Mat& mask1, const Mat &img2, const Mat& mask2,
	vector<KeyPoint>& kp1, vector<KeyPoint>& kp2, Mat& des1, Mat& des2)
{
	//detect 4000 keypoints, the first image scale is 1.2 and detect 2 scale images
	Ptr<ORB> orb = ORB::create();
	//ORB orb(4000, 1.2);
	//SIFT sift;
	int64 time1 = getTickCount(), time2 = 0, time3 = 0, time4 = 0;
	orb->detectAndCompute(img1, mask1, kp1, des1);
	//sift(img1, Mat(), kp1, des1);
	time2 = getTickCount();
	double cost1 = 1000.0 * (time2 - time1) / getTickFrequency();

	time3 = getTickCount();
	orb->detectAndCompute(img2, mask2, kp2, des2);
	//sift(img2, Mat(), kp2, des2);
	time4 = getTickCount();
	double cost2 = 1000.0*(time4 - time3) / getTickFrequency();
}

//match feature points using descriptors and return the match point pairs
void ORBAlgorithm::matchPoints(const vector<KeyPoint>& kp1, const vector<KeyPoint>& kp2,
	const Mat& des1, const Mat& des2, vector<DMatch>& goodMatches)
{
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	//Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	vector<DMatch> matches1to2;
	vector<DMatch> matches2to1;
	vector<DMatch> twoDirectionMatch;
	//match
	matcher->match(des1, des2, matches1to2);
	matcher->match(des2, des1, matches2to1);
	//get the intersection of match image1 to image2 and match image2 to image1
	int *flag = new int[des2.rows];
	memset(flag, -1, sizeof(int)*des2.rows);
	for (size_t i = 0; i < des2.rows; i++)
	{
		flag[matches2to1[i].queryIdx] = matches2to1[i].trainIdx;
	}
	for (size_t i = 0; i < matches1to2.size(); i++)
	{
		if (flag[matches1to2[i].trainIdx] == matches1to2[i].queryIdx)
			twoDirectionMatch.push_back(matches1to2[i]);
	}
	//get the best 100 matches
	sort(twoDirectionMatch.begin(), twoDirectionMatch.end(), sortByDistance);
	size_t loop_time = twoDirectionMatch.size();
	if (loop_time >= 500)
		loop_time = 500;
	for (size_t i = 0; i < loop_time; i++)
	{
		goodMatches.push_back(twoDirectionMatch[i]);
	}
}


//get homography mat using RANSAC
void ORBAlgorithm::getHomographyMat(const vector<KeyPoint>& kp1, const vector<KeyPoint>& kp2,
	vector<DMatch>& goodMatches, Mat& homography)
{
	vector<Point2f> image1Points;
	vector<Point2f> image2Points;
	for (size_t i = 0; i < goodMatches.size(); i++)
	{
		image1Points.push_back(kp1[goodMatches[i].queryIdx].pt);
		image2Points.push_back(kp2[goodMatches[i].trainIdx].pt);
	}
	Mat inlier_mask;
	homography = findHomography(image2Points, image1Points, CV_RANSAC, 2.5f, inlier_mask);
	std::cout << homography << std::endl;

	int i = 0;
	for (vector<DMatch>::iterator iter = goodMatches.begin(); iter != goodMatches.end();)
	{
		if (!inlier_mask.at<uchar>(i))
		{
			iter = goodMatches.erase(iter);
		}
		else
			iter++;
		i++;
	}
}

void ORBAlgorithm::getHomographyMat(const Mat& img1, const Mat& img2, const vector<KeyPoint>& kp1,
	const vector<KeyPoint>& kp2, vector<DMatch>& goodMatches, Mat& homography)
{
	vector<Point2f> image1Points;
	vector<Point2f> image2Points;
	for (size_t i = 0; i < goodMatches.size(); i++)
	{
		image1Points.push_back(kp1[goodMatches[i].queryIdx].pt);
		image2Points.push_back(kp2[goodMatches[i].trainIdx].pt);
	}
	Mat inlier_mask;
	homography = findHomography(image2Points, image1Points, CV_RANSAC, 2.5f, inlier_mask);
	std::cout << homography << std::endl;

	int i = 0;
	for (vector<DMatch>::iterator iter = goodMatches.begin(); iter != goodMatches.end();)
	{
		if (!inlier_mask.at<uchar>(i))
		{
			iter = goodMatches.erase(iter);
		}
		else
			iter++;
		i++;
	}
	std::cout << "goodMatche num: " << goodMatches.size() << std::endl;
	Mat matchImage;
	drawMatches(img1, kp1, img2, kp2, goodMatches, matchImage, Scalar(0, 255, 0), Scalar(0, 0, 255));
	imwrite("picture\\matchImage.jpg", matchImage);
}
