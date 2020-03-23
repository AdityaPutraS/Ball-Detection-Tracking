#include <iostream>
#include <opencv/cv.hpp>
#include "BallDetection.hpp"

using namespace std;
using namespace cv;

int main()
{
    VideoCapture cap("dataset/video1.mp4");
    bool ret = true;
    int waitMillis = 25;
    bool pause = false;
    BallDetection ballDet;
    Mat frame;
    while (true)
    {
        if(!pause)
        {
            cap >> frame;
        }
        if (frame.empty())
        {
            cap.release();
            cap = VideoCapture("dataset/video1.mp4");
            continue;
        }
        // imshow("Frame", frame);
        ballDet.Process(frame);
        char c = (char)waitKey(waitMillis);
        if (c == 'q')
        {
            break;
        }
        else if (c == 'e')
        {
            pause = true;
        }
        else if (c == 'r')
        {
            pause = false;
        }
    }
    cap.release();
    return 0;
}