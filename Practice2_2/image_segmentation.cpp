#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

cv::Mat colorFrame(cv::Mat red, cv::Mat green, cv::Mat blue)
{
    cv::Mat res;
    std::vector<cv::Mat> channels;

    channels.push_back(blue);
    channels.push_back(green);
    channels.push_back(red);

    cv::merge(channels, res);
    return res;
}

cv::Mat redFilter(cv::Mat frame)
{
    cv::Mat res;
    cv::inRange(frame, cv::Scalar(0, 0, 100), cv::Scalar(120, 120, 255), res);
    
    return res;
}

cv::Mat greenFilter(cv::Mat frame)
{
    cv::Mat res;
    cv::inRange(frame, cv::Scalar(0, 100, 0), cv::Scalar(100, 255, 150), res);

    return res;
}

cv::Mat blueFilter(cv::Mat frame)
{
    cv::Mat res;
    cv::inRange(frame, cv::Scalar(100, 0, 0), cv::Scalar(255, 150, 100), res);

    return res;
}

cv::Mat dilate(cv::Mat frame, int dilation_size)
{
    cv::Mat res;
    int dilation_type = cv::MORPH_ELLIPSE;
    cv::Mat element = cv::getStructuringElement( dilation_type, cv::Size(2*dilation_size + 1, 2*dilation_size+1), cv::Point(dilation_size, dilation_size));
    cv::dilate(frame, res, element);
    return res;
}

int main(int argc, const char ** argv)
{
    bool run = true;
    cv::VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    bool shouldDilate = true;
    int dillationSize = 1;

    cap.open(deviceID, apiID);

    cv::namedWindow("Original",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Segmented",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Overlay",cv::WINDOW_AUTOSIZE);

    while(run)
    {
        cv::Mat frame, finalFrame;
        if (!cap.read(frame)) // if not success, break loop
        {
            std::cout<<"\n Cannot read the video file. \n";
            break;
        }

        cv::flip(frame, frame, 1);

        cv::Mat red = dilate(redFilter(frame), dillationSize);
        cv::Mat green = dilate(greenFilter(frame), dillationSize);
        cv::Mat blue = dilate(blueFilter(frame), dillationSize);

        cv::Mat coloredFrame = colorFrame(red, green, blue);

        imshow("Original", frame);
        imshow("Segmented", coloredFrame);

        cv::addWeighted(frame, 0.5, coloredFrame, 1.0, 0.0, finalFrame);
        imshow("Overlay", finalFrame);
        char k = cv::waitKey(30);
        switch (k)
        {
            case 27:
                run = false;
                break;
            default:
        imshow("Original", frame);
        imshow("Segmented", coloredFrame);
                break;
        }
    }
    return 0;
}