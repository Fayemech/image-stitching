#include<iostream>
#include<vector>

#include<opencv2\core\core.hpp>
#include<opencv2\features2d\features2d.hpp>
#include<opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;


int main()
{
	//��ȡͼƬ
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

	//���ӻ�����ʾ�ؼ���
	Mat ShowKeypoints1, ShowKeypoints2;
	drawKeypoints(rgbd1, Keypoints1, ShowKeypoints1);
	drawKeypoints(rgbd2, Keypoints2, ShowKeypoints2);
	imshow("Keypoints1", ShowKeypoints1);
	imshow("Keypoints2", ShowKeypoints2);
	waitKey(0);

	//Matching
	vector<DMatch> matches;
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	matcher->match(descriptors1, descriptors2, matches);
	cout << "find out total " << matches.size() << " matches" << endl;

	//���ӻ�
	Mat ShowMatches;
	drawMatches(rgbd1, Keypoints1, rgbd2, Keypoints2, matches, ShowMatches);
	imshow("matches", ShowMatches);
	waitKey(0);



	return 0;
}
