#include "BallDetection.hpp"
#include <iostream>

using namespace std;

BallDetection::BallDetection() : ballRadius(0), ballPos(-1, -1)
{
    namedWindow("Contour", WINDOW_NORMAL);
    namedWindow("Parameter");

    // Default value
    min_H = 0;
    max_H = 28;
    min_S = 189;
    max_S = 255;
    min_V = 146;
    max_V = 255;
    countErode = 1;
    epsBoundRC = 70;
    epsBoundEC = 45;
    minRContour = 12;//25;
    maxRContour = 200;
    min_dist = 4;
    upper_threshold = 61;
    center_threshold = 45;
    min_r = 90;
    max_r = 209;
    lowThreshold = 150;
    ratio = 1;
    kernel_size = 3;
    createTrackbar("Min H", "Parameter", &min_H, 255, on_trackbar);
    createTrackbar("Max H", "Parameter", &max_H, 255, on_trackbar);
    createTrackbar("Min S", "Parameter", &min_S, 255, on_trackbar);
    createTrackbar("Max S", "Parameter", &max_S, 255, on_trackbar);
    createTrackbar("Min V", "Parameter", &min_V, 255, on_trackbar);
    createTrackbar("Max V", "Parameter", &max_V, 255, on_trackbar);
    createTrackbar("Count Erosi", "Parameter", &countErode, 10, on_trackbar);
    createTrackbar("epsBoundRC", "Parameter", &epsBoundRC, 100, on_trackbar);
    createTrackbar("epsBoundEC", "Parameter", &epsBoundEC, 100, on_trackbar);
    createTrackbar("minRContour", "Parameter", &minRContour, 200, on_trackbar);
    createTrackbar("maxRContour", "Parameter", &maxRContour, 200, on_trackbar);
    createTrackbar("lowThreshold Canny", "Parameter", &lowThreshold, 500, on_trackbar);
    createTrackbar("lowThreshold*ratio Canny", "Parameter", &ratio, 50, on_trackbar);
    createTrackbar("kernel_size", "Parameter", &kernel_size, 50, on_trackbar);
    createTrackbar("min_dist", "Parameter", &min_dist, 20, on_trackbar);
    createTrackbar("upTh", "Parameter", &upper_threshold, 1000, on_trackbar);
    createTrackbar("c_th", "Parameter", &center_threshold, 1000, on_trackbar);
    createTrackbar("min_r", "Parameter", &min_r, 400, on_trackbar);
    createTrackbar("max_r", "Parameter", &max_r, 400, on_trackbar);
}

void BallDetection::on_trackbar(int value, void *userdata) {}

Mat BallDetection::morphOps(Mat thresh)
{
    Mat hasilMorph;
    Mat element = getStructuringElement(0, Size(2 * countErode + 1, 2 * countErode + 1), Point(0, 0));

    morphologyEx(thresh, hasilMorph, MORPH_CLOSE, element);
    return hasilMorph;
}

Mat BallDetection::preProcessing(Mat frame)
{
    Mat hasilBlur, hasilThresholding, hasilMorph;
    GaussianBlur(frame, hasilBlur, Size(3, 3), 2, 2);
    GaussianBlur(hasilBlur, hasilBlur, Size(9, 9), 2, 2, 4);
    cvtColor(hasilBlur, hasilThresholding, COLOR_BGR2HSV);
    inRange(hasilThresholding, Scalar(min_H, min_S, min_V), Scalar(max_H, max_S, max_V), hasilThresholding);
    hasilMorph = morphOps(hasilThresholding);
    GaussianBlur(hasilMorph, hasilMorph, Size(9, 9), 2, 2, 4);
    return hasilMorph;
}

bool BallDetection::isDetectBall(Point2d center)
{
    if (center.x > 0)
    {
        cout << "Bola di : ( " << center.x << " , " << center.y << " )" << endl;
        return true;
    }
    else
    {
        cout << "Bola tidak ada" << endl;
        return false;
    }
}

bool compareBola(Point3f p1, Point3f p2)
{
    return p1.z < p2.z;
}

bool BallDetection::Process(Mat image)
{
    //--------------------------------Ball Detection--------------------------------
    vector<Point3f> kandidatBola; //Vector titik 3 dimensi (x, y, r), r = radius

    //Menggunakkan Contour Detection & cek bentuk untuk mencari bola
    //--------------------------------Contour Finding--------------------------------
    vector<vector<Point>> contours;
    findContours(image, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    Mat contoursImg;
    image.copyTo(contoursImg);
    for (size_t i = 0; i < contours.size(); i++)
    {
        Point2f center;
        float radius;
        minEnclosingCircle(contours[i], center, radius);
        RotatedRect minRect = minAreaRect(contours[i]);
        RotatedRect minEllips = RotatedRect(minRect);
        //--------------------------------Cek apakah rasio rect/circle > epsBoundRC--------------------------------
        //Jika iya maka bola
        float luasCirlce = radius * radius * CV_PI;
        float luasRect = minRect.size.area();
        bool passed = false;
        if (100 * luasRect / luasCirlce > epsBoundRC)
        {
            if (contours[i].size() >= 5)
            {
                minEllips = fitEllipse(contours[i]);
#ifdef DEBUGBALL
                ellipse(contoursImg, minEllips, Scalar(0, 255, 0), 3);
#endif
            }
            //--------------------------------Cek apakah rasio ellipse/circle > epsBoundEC--------------------------------
            Point2f elips_points[4];
            minEllips.points(elips_points);
            float minor = norm(elips_points[0] - elips_points[1]) / 2;
            float major = norm(elips_points[1] - elips_points[2]) / 2;
            float luasEllips = minor * major * CV_PI;
            if (100 * luasEllips / luasCirlce > epsBoundEC)
            {
                // ROS_ERROR_STREAM("Con " << i << "Pass EC" << endl);
                passed = true;
            }
#ifdef DEBUGBALL
            // Debugging : gambar minAreaRect
            Point2f rect_points[4];
            minRect.points(rect_points);
            for (int j = 0; j < 4; j++)
            {
                line(contoursImg, rect_points[j], rect_points[(j + 1) % 4], Scalar(100, 0, 255), 2);
            }
#endif
        }
        //--------------------------------Cek apakah area masuk dalam batas--------------------------------
        passed = passed && radius >= minRContour && radius <= maxRContour;
        //--------------------------------Gambal bola ke image--------------------------------
        if (passed)
        {
            circle(contoursImg, center, radius, Scalar(0, 0, 255), 5);
            //Tambahkan ke kandidat
            kandidatBola.push_back(Point3f(center.x, center.y, radius));
        }
    }
    imshow("Contour", contoursImg);
    //--------------------------------Hough Circle--------------------------------
    //PreProcessing lagi
    Mat preProcHough;
    Canny(image, preProcHough, lowThreshold, lowThreshold * ratio, 7);
    GaussianBlur(preProcHough, preProcHough, Size(9, 9), 2, 2, 4);
    vector<Vec3f> circles;
    HoughCircles(preProcHough, circles, HOUGH_GRADIENT, 1, preProcHough.rows / min_dist,
                    upper_threshold, center_threshold, min_r, max_r);
    for (Vec3f c : circles)
    {
#ifdef DEBUGBALL
        circle(contoursImg, Point(c[0], c[1]), c[2] / 2, Scalar(255, 0, 0), 8);
#endif
        kandidatBola.push_back(Point3f(c[0], c[1], c[2]));
    }
    //--------------------------------Sort bola berdasar radius--------------------------------
    sort(kandidatBola.begin(), kandidatBola.end(), compareBola);
    if (kandidatBola.size() >= 1)
    {
        ballRadius = kandidatBola[0].z;
        ballPos = Point2d(kandidatBola[0].x, kandidatBola[0].y);
        return true;
    }
    else
    {
        ballRadius = 0;
        ballPos = Point2d(-1, -1);
        return false;
    }
}
