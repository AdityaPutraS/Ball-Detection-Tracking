#include <iostream>
#include <opencv/cv.hpp>
#include "BallDetection.hpp"
#include "BallTracker.hpp"
#include <chrono>

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

    int counterObjectDetection = 0;
    int counterObjectTracking = 0;
    int counterFrame = 0;
    long int totalTimeDetect = 0, totalTimeTrack = 0;
    int diff = 0;
    chrono::steady_clock::time_point begin, end;
    while (true)
    {
        counterFrame += 1;
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
            counterObjectDetection += 1;
            cout << "Detecting ball" << endl;
            begin = chrono::steady_clock::now();
            bool adaBola = ballDet.Process(ballMask);
            end = chrono::steady_clock::now();
            diff = chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            totalTimeDetect += diff;
            circle(frame, ballDet.ballPos, ballDet.ballRadius, Scalar(0, 0, 255), 8);
            if (adaBola)
            {
                ballTrack.init(ballMask, ballDet.ballPos, ballDet.ballRadius);
                cariBola = false;
                continue;
            }
        }
        if (!cariBola)
        {
            counterObjectTracking += 1;
            begin = chrono::steady_clock::now();
            bool succ = ballTrack.update(ballMask);
            end = chrono::steady_clock::now();
            diff = chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            totalTimeTrack += diff;
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
    cout << totalTimeDetect << " " << totalTimeTrack << endl;
    cout << "Average time detection : " << (float) totalTimeDetect / counterObjectDetection << " microsecond " << endl;
    cout << "Average time tracking : " << (float) totalTimeTrack / counterObjectTracking << " microsecond " << endl;
    cout << "Num of detection : " << counterObjectDetection << endl;
    cout << "Num of tracking : " << counterObjectTracking << endl;
    cout << "Num of frame : " << counterFrame << endl;
    return 0;
}