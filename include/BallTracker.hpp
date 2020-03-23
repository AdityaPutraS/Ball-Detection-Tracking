#ifndef BALL_TRACKER
#define BALL_TRACKER

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#define max_val 255
#define max_lowThreshold 500

using namespace cv;

class BallTracker
{
private:
    Ptr<Tracker> tracker;   //Smart pointer dari tracker
public:
    Rect2d box;
    BallTracker();
    void init(Mat frame, Point2d center, int radius);
    bool update(Mat frame);
};

#endif