#ifndef BALL_DETECTION
#define BALL_DETECTION

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define max_val 255
#define max_lowThreshold 500

using namespace cv;

class BallDetection
{
private:
    int min_H, max_H, min_S, max_S, min_V, max_V;

    int countErode;

    int lowThreshold;
    int ratio;
    int kernel_size;

    int min_dist;
    int upper_threshold;
    int center_threshold;
    int min_r;
    int max_r;

    int epsBoundRC, epsBoundEC;
    int minRContour, maxRContour;

    static void on_trackbar(int value, void *userdata);
    void CreateTrackbar();
    void CreateTrackbarField();
    Mat morphOps(Mat thresh);
    

public:
    int ballRadius;
    Point2d ballPos;

    BallDetection();
    static bool isDetectBall(Point2d center);
    bool Process(Mat image);
    Mat preProcessing(Mat frame);
};

#endif