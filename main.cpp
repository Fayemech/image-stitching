#include <iostream>
#include <vector>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QApplication>
#include <QSurfaceFormat>
#include "laumemoryobject.h"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

typedef struct {
    Point2f left_top;
    Point2f left_bottom;
    Point2f right_top;
    Point2f right_bottom;
} four_corners_t;

four_corners_t corners;

void CalcCorners(const Mat &H, const Mat &src)
{
    double v2[] = { 0, 0, 1 };
    double v1[3];
    Mat V2 = Mat(3, 1, CV_64FC1, v2);
    Mat V1 = Mat(3, 1, CV_64FC1, v1);

    V1 = H * V2;
    //left top
    cout << "V2: " << V2 << endl;
    cout << "V1: " << V1 << endl;
    corners.left_top.x = v1[0] / v1[2];
    corners.left_top.y = v1[1] / v1[2];

    //left bot
    v2[0] = 0;
    v2[1] = src.rows;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);
    V1 = Mat(3, 1, CV_64FC1, v1);
    V1 = H * V2;
    corners.left_bottom.x = v1[0] / v1[2];
    corners.left_bottom.y = v1[1] / v1[2];

    //right top
    v2[0] = src.cols;
    v2[1] = 0;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);
    V1 = Mat(3, 1, CV_64FC1, v1);
    V1 = H * V2;
    corners.right_top.x = v1[0] / v1[2];
    corners.right_top.y = v1[1] / v1[2];

    //right bot
    v2[0] = src.cols;
    v2[1] = src.rows;
    v2[2] = 1;
    V2 = Mat(3, 1, CV_64FC1, v2);
    V1 = Mat(3, 1, CV_64FC1, v1);
    V1 = H * V2;
    corners.right_bottom.x = v1[0] / v1[2];
    corners.right_bottom.y = v1[1] / v1[2];
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QString("Lau Consulting Inc"));
    a.setOrganizationDomain(QString("drhalftone.com"));
    a.setApplicationName(QString("LAURealSense"));

    QSurfaceFormat format;
    format.setDepthBufferSize(10);
    format.setMajorVersion(4);
    format.setMinorVersion(1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);

    LAUMemoryObject objectA((QString()));
    LAUMemoryObject objectB((QString()));

    //read image first;
    Mat image01 = imread("frame001.tif");
    Mat image02 = imread("frame002.tif");

    //test, read success or not;
    //namedWindow("p1");
    //namedWindow("p2");

    //get gray image for next step
    Mat image1, image2;
    cvtColor(image01, image1, cv::COLOR_RGB2GRAY);
    cvtColor(image02, image2, cv::COLOR_RGB2GRAY);

    //FeatureDetector dete;
    //FastFeatureDetector;
    //surf && sift miss, I got surf,sift back, but still face some problem.
    int minHessian = 40;
    vector<KeyPoint> keypoint1, keypoint2;
    Ptr<SurfFeatureDetector> dete = SurfFeatureDetector::create(minHessian);
    dete->detect(image1, keypoint1);
    dete->detect(image2, keypoint2);

    Ptr<SurfDescriptorExtractor>extractor = SurfDescriptorExtractor::create();
    Mat imagedes1, imagedes2;
    extractor->compute(image1, keypoint1, imagedes1);
    extractor->compute(image2, keypoint2, imagedes2);

    //BFMatcher matcher(NORM_L1);

    Ptr<FlannBasedMatcher> matcher = FlannBasedMatcher::create();
    matcher->BRUTEFORCE;
    //Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create();
    vector<vector<DMatch> > matchePoints;
    vector< DMatch > goodmatches;
    vector<Mat> train_desc(1, imagedes1);
    matcher->add(train_desc);
    matcher->train();

    matcher->knnMatch(imagedes1, matchePoints, 2);

    for (int i = 0; i < matchePoints.size(); i++) {
        if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance) {
            goodmatches.push_back(matchePoints[i][0]);
        }
    }

    Mat img_match;
    drawMatches(image01, keypoint1, image02, keypoint2, goodmatches, img_match);

    namedWindow("match");
    imshow("match", img_match);

    vector<Point2f> imagePoints1, imagePoints2;

    for (int i = 0; i < goodmatches.size(); i++) {
        imagePoints2.push_back(keypoint1[goodmatches[i].queryIdx].pt);
        imagePoints1.push_back(keypoint2[goodmatches[i].trainIdx].pt);
    }

    Mat homo = findHomography(imagePoints1, imagePoints2, cv::RANSAC);

    cout << "homo was£∫\n" << homo << endl << endl;

    CalcCorners(homo, image01);
    cout << "left_top:" << corners.left_top << endl;
    cout << "left_bottom:" << corners.left_bottom << endl;
    cout << "right_top:" << corners.right_top << endl;
    cout << "right_bottom:" << corners.right_bottom << endl;

    Mat imageTransform1, imageTransform2;
    warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));

    imshow("4", imageTransform1);
    //imwrite("trans1.jpg", imageTransform1);

    int dst_width = imageTransform1.cols;  //»°◊Ó”“µ„µƒ≥§∂»Œ™∆¥Ω”Õºµƒ≥§∂»
    int dst_height = image02.rows;

    Mat dst(dst_height, dst_width, CV_8UC3);
    dst.setTo(0);

    imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));
    image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));

    imshow("b_dst", dst);
    imshow("dst", dst);
    //imwrite("dst.jpg", dst);

    waitKey();

    return 0;
}
