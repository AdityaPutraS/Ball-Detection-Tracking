#include <iostream>
#include <opencv/cv.hpp>
#include "BallDetection.hpp"
#include "BallTracker.hpp"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << argv[1];
        return -1;
    }
    string videoName = string(argv[1]);
    VideoCapture cap(videoName);
    bool ret = true;
    int waitMillis = 1;
    bool pause = false;
    BallDetection ballDet;

    bool cariBola = false;
    int counterTracking = 0;
    BallTracker ballTrack;

    Mat frame, ballMask;
    int errorCounter = 0;
    while (true)
    {
        cap >> frame;
        if (pause)
        {
            waitMillis = 0;
        }
        else
        {
            waitMillis = 1;
        }
        if (frame.empty())
        {
            cap.release();
            cap = VideoCapture(videoName);
            continue;
        }
        resize(frame, frame, Size(640, 480));
        ballMask = ballDet.preProcessing(frame);
        imshow("Ball Mask", ballMask);
        if (cariBola)
        {
            cout << "Detecting ball" << endl;
            bool adaBola = ballDet.Process(ballMask);
            circle(frame, ballDet.ballPos, ballDet.ballRadius, Scalar(0, 0, 255), 8);
            if (adaBola)
            {
                ballTrack.init(ballMask, ballDet.ballPos, ballDet.ballRadius);
                cariBola = false;
            }
        }
        if (!cariBola)
        {
            bool succ = ballTrack.update(ballMask);
            if (!succ)
            {
                cout << "Gagal update tracker" << endl;
                errorCounter += 1;
            }
            else
            {
                cout << "Update tracker" << endl;
                counterTracking += 1;
                rectangle(frame, ballTrack.box, Scalar(255, 0, 0), 8);
            }
            if (errorCounter > 5)
            {
                cariBola = true;
                errorCounter = 0;
            }
            if (counterTracking > 20)
            {
                cariBola = true;
                counterTracking = 0;
            }
        }
        imshow("Frame", frame);
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