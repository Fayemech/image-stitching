#include<iostream>
#include<vector>
#include<opencv2/core/core.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/imgproc.hpp>


using namespace std;
using namespace cv;

/*
Mat mystitched(Mat rgbd1, Mat rgbd2) {

}
*/


int main()
{
	//读取图片
	Mat rgbd1 = imread("frame001.tif");
	Mat rgbd2 = imread("frame002.tif");
	//imshow("rgbd1", depth2);
	//waitKey(0);
	Ptr<ORB> orb = ORB::create();
	vector<KeyPoint> Keypoints1, Keypoints2;
	Mat descriptors1, descriptors2;
	orb->detectAndCompute(rgbd1, Mat(), Keypoints1, descriptors1);
	orb->detectAndCompute(rgbd2, Mat(), Keypoints2, descriptors2);

	//cout << "Key points of image" << Keypoints.size() << endl;

	//可视化，显示关键点
	Mat ShowKeypoints1, ShowKeypoints2;
	drawKeypoints(rgbd1, Keypoints1, ShowKeypoints1);
	drawKeypoints(rgbd2, Keypoints2, ShowKeypoints2);
	imshow("Keypoints1", ShowKeypoints1);
	imshow("Keypoints2", ShowKeypoints2);
	waitKey(0);

	//Matching
	vector<DMatch> matches1to2;
	vector<DMatch> matches2to1;
	vector<DMatch> goodmatches;
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	matcher->match(descriptors1, descriptors2, matches1to2);
	matcher->match(descriptors2, descriptors1, matches2to1);
	int *flag = new int[descriptors2.rows];
	memset(flag, -1, sizeof(int)*descriptors2.rows);
	for (size_t i = 0; i < descriptors2.rows; i++) {
		flag[matches2to1[i].queryIdx] = matches2to1[i].trainIdx;
	}
	for (size_t i = 0; i < matches1to2.size(); i++) {
		if (flag[matches1to2[i].trainIdx] == matches1to2[i].queryIdx) {
			goodmatches.push_back(matches1to2[i]);
		}
	}
	cout << "find out total " << goodmatches.size() << " matches" << endl;
	sort(goodmatches.begin(), goodmatches.end());



	size_t loop = goodmatches.size() * 0.15 > 50 ? 50 : goodmatches.size() * 0.15;
	vector<DMatch> res;
	for (int i = 0; i < loop; i++) {
		res.push_back(goodmatches[i]);
	}
	//可视化
	Mat ShowMatches;
	drawMatches(rgbd1, Keypoints1, rgbd2, Keypoints2, res, ShowMatches);
	imshow("matches", ShowMatches);
	

	vector<Point2f> imgpoints1, imgpoints2;
	for (int i = 0; i < res.size(); i++) {
		imgpoints1.push_back(Keypoints1[res[i].queryIdx].pt);
		imgpoints2.push_back(Keypoints2[res[i].trainIdx].pt);
	}
	Mat homo = findHomography(imgpoints1, imgpoints2, CV_RANSAC);
	cout << homo << endl << endl;

	Mat stitchedimg;
	int mrows = rgbd1.rows > rgbd2.rows ? rgbd1.rows : rgbd2.rows;

	
	int propimg1 = 0, propimg2 = 0;
	for (int i = 0; i < res.size(); i++) {
		if (Keypoints1[res[i].queryIdx].pt.x > rgbd1.cols / 2) {
			propimg1++;
		}
		if (Keypoints2[res[i].trainIdx].pt.x > rgbd2.cols / 2) {
			propimg2++;
		}
	}

	bool fla = false;

	Mat imgright, imgleft;
	if ((propimg1 / (res.size() + 0.0)) > (propimg2 / (res.size() + 0.0))) {
		imgleft = rgbd1.clone();
		fla = true;
	}
	else {
		imgleft = rgbd2.clone();
		fla = false;
	}
	if (fla) {
		imgright = rgbd2.clone();
		fla = false;
	}
	else {
		imgright = rgbd1.clone();
	}

	
	warpPerspective(imgright, stitchedimg, homo, Size(rgbd2.cols + rgbd1.cols, mrows));

	
	Mat half(stitchedimg, Rect(0, 0, imgleft.cols, imgleft.rows));
	imgleft.copyTo(half);
	imshow("test", imgright);
	
	











	waitKey(0);

	

	return 0;
}
