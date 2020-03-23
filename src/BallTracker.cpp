#include "BallTracker.hpp"

BallTracker::BallTracker()
{
    tracker = TrackerCSRT::create();
}

void BallTracker::init(Mat frame, Point2d center, int radius)
{
    box = Rect2d(center, Size(radius, radius));
    tracker.release();
    tracker = TrackerCSRT::create();
    tracker->init(frame, box);
}
bool BallTracker::update(Mat frame)
{
    return tracker->update(frame, box);
}