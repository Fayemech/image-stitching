
#include<iostream>
#include<vector>
#include<opencv2/core/core.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/imgproc.hpp>


using namespace std;
using namespace cv;

typedef struct
{
	Point2f left_top;
	Point2f left_bottom;
	Point2f right_top;
	Point2f right_bottom;
}four_corners_t;




void CalcCorners(const Mat& H, const Mat& src, four_corners_t& corners)
{
	double v2[] = { 0, 0, 1 };//左上角
	double v1[3];//变换后的坐标值
	Mat V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	Mat V1 = Mat(3, 1, CV_64FC1, v1);  //列向量

	V1 = H * V2;
	//左上角(0,0,1)
	cout << "V2: " << V2 << endl;
	cout << "V1: " << V1 << endl;
	corners.left_top.x = v1[0] / v1[2];
	corners.left_top.y = v1[1] / v1[2];

	//左下角(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.left_bottom.x = v1[0] / v1[2];
	corners.left_bottom.y = v1[1] / v1[2];

	//右上角(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.right_top.x = v1[0] / v1[2];
	corners.right_top.y = v1[1] / v1[2];

	//右下角(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.right_bottom.x = v1[0] / v1[2];
	corners.right_bottom.y = v1[1] / v1[2];

}
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
	Mat homo = findHomography(imgpoints2, imgpoints1, CV_RANSAC);
	cout << homo << endl << endl;

	
	int mrows = rgbd1.rows > rgbd2.rows ? rgbd1.rows : rgbd2.rows;
	/*
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
	*/
	Mat stitchedimg;
	Mat color1 = rgbd1(Rect(100, 0, 5, rgbd1.rows));
	Mat color2 = rgbd2(Rect(100, 0, 5, rgbd2.rows));
	Mat mid;
	four_corners_t cor, cor1;
	CalcCorners(homo, color2, cor1);
	CalcCorners(homo, rgbd2, cor);
	warpPerspective(color2, mid, homo, Size(MAX(cor.right_top.x, cor.right_bottom.x), mrows));
	warpPerspective(rgbd2, stitchedimg, homo, Size(MAX(cor.right_top.x, cor.right_bottom.x), mrows));
	Mat lin(mid.rows, mid.cols, CV_8UC3);
	lin.setTo(0);
	mid.copyTo(lin(Rect(0, 0, mid.cols, mid.rows)));
	color1.copyTo(lin(Rect(0, 0, color1.cols, color1.rows)));

	int dst_width = stitchedimg.cols;
	int dst_height = stitchedimg.rows;
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);
	//Mat tmp = rgbd2(Rect(100, 0, 5, stitchedimg.rows));
	stitchedimg.copyTo(dst(Rect(0, 0, stitchedimg.cols, stitchedimg.rows)));
	rgbd1.copyTo(dst(Rect(0, 0, rgbd1.cols, rgbd1.rows)));
	//Mat half(stitchedimg, Rect(0, 0, rgbd1.cols, rgbd1.rows));
	//100-105
	//rgbd1.copyTo(stitchedimg(Rect(0, 0, rgbd1.cols, rgbd1.rows)));
	imshow("mid", lin);
	imshow("test", dst);
	//imwrite("res.tif", dst);
	
	











	waitKey(0);

	

	return 0;
}
