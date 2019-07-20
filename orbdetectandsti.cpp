#include<iostream>
#include<vector>
#include<opencv2/core/core.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/imgproc.hpp>
#include<time.h>

using namespace std;
using namespace cv;


typedef struct
{
	Point2f left_top;
	Point2f left_bottom;
	Point2f right_top;
	Point2f right_bottom;
}four_corners_t;

//four_corners_t corners;



void CalcCorners(const Mat& H, const Mat& src, four_corners_t& corners)
{
	double v2[] = { 0, 0, 1 };//left top
	double v1[3];//after transform
	Mat V2 = Mat(3, 1, CV_64FC1, v2);  //
	Mat V1 = Mat(3, 1, CV_64FC1, v1);  //

	V1 = H * V2;
	//left top(0,0,1)
	cout << "V2: " << V2 << endl;
	cout << "V1: " << V1 << endl;
	corners.left_top.x = v1[0] / v1[2];
	corners.left_top.y = v1[1] / v1[2];

	//left bot(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //
	V1 = Mat(3, 1, CV_64FC1, v1);  //
	V1 = H * V2;
	corners.left_bottom.x = v1[0] / v1[2];
	corners.left_bottom.y = v1[1] / v1[2];

	//right top(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //
	V1 = Mat(3, 1, CV_64FC1, v1);  //
	V1 = H * V2;
	corners.right_top.x = v1[0] / v1[2];
	corners.right_top.y = v1[1] / v1[2];

	//right bot(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //
	V1 = Mat(3, 1, CV_64FC1, v1);  //
	V1 = H * V2;
	corners.right_bottom.x = v1[0] / v1[2];
	corners.right_bottom.y = v1[1] / v1[2];

}

Mat mystitched(Mat rgb1, Mat rgb2) {
	
	Mat rgbd1(rgb1, Rect(0, 0, rgb1.cols, rgb1.rows));
	Mat rgbd2(rgb2, Rect(0, 0, rgb2.cols, rgb2.rows));
	Ptr<ORB> orb = ORB::create();
	vector<KeyPoint> Keypoints1, Keypoints2;
	Mat descriptors1, descriptors2;
	orb->detectAndCompute(rgbd1, Mat(), Keypoints1, descriptors1);
	orb->detectAndCompute(rgbd2, Mat(), Keypoints2, descriptors2);

	//cout << "Key points of image" << Keypoints.size() << endl;

	//show
	//Mat ShowKeypoints1, ShowKeypoints2;
	//drawKeypoints(rgbd1, Keypoints1, ShowKeypoints1);
	//drawKeypoints(rgbd2, Keypoints2, ShowKeypoints2);
	//imshow("Keypoints1", ShowKeypoints1);
	//imshow("Keypoints2", ShowKeypoints2);
	//waitKey(0);

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
	//show
	//Mat ShowMatches;
	//drawMatches(rgbd1, Keypoints1, rgbd2, Keypoints2, res, ShowMatches);
	//imshow("matches", ShowMatches);


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
	Mat sti;
	four_corners_t cor;
	CalcCorners(homo, rgbd2, cor);

	warpPerspective(rgbd2, sti, homo, Size(MAX(cor.right_top.x, cor.right_bottom.x), mrows));
	int dst_width = sti.cols;
	int dst_height = sti.rows;
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);
	sti.copyTo(dst(Rect(0, 0, sti.cols, sti.rows)));
	rgbd1.copyTo(dst(Rect(0, 0, rgbd1.cols, rgbd1.rows)));
	//Mat half(stitchedimg, Rect(0, 0, rgbd1.cols, rgbd1.rows));

	//rgbd1.copyTo(sti(Rect(0, 0, rgbd1.cols, rgbd1.rows)));
	return dst;
	//imshow("test", stitchedimg);
}



int main()
{
	//read img
	Mat img = imread("frame001.tif");
	//Mat rgbd2 = imread("frame002.tif");
	//Mat stitchedimg;
	int n;
	cout << "how many img you want to stitched" << endl;
	cin >> n;
	cout << "start" << endl;
	clock_t start, finish;
	double totaltime;
	start = clock();
	for (int k = 2; k <= n; k++) {
		stringstream stream;
		string str;
		stream << k;
		stream >> str;
		//string cur = to_string(k);
		string filename = "frame00" + str + ".tif";
		Mat img1 = imread(filename);
		Mat stitchedimg = mystitched(img, img1);
		img = stitchedimg;
	}
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "finish" << endl;
	cout << "total time is" << totaltime << "Sec!" << endl;
	imshow("res", img);
	waitKey(0);
	return 0;













	waitKey(0);



	return 0;
}
