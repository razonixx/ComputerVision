#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    cv::VideoCapture cap;
    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cap.open(deviceID, apiID);
    
    cv::Mat src;

    while(cv::waitKey(30) != 27)
    {
        cap.read(src);
        if(src.empty()){
            std::cout << "Error reading camera" << std::endl;
            return EXIT_FAILURE;
        }
        cv::Mat gray;
        cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        medianBlur(gray, gray, 5);
        std::vector<cv::Vec3f> circles;
        HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, gray.rows/16, 100, 30, 1, 30);
        
        for( size_t i = 0; i < circles.size(); i++ )
        {
            cv::Vec3i c = circles[i];
            cv::Point center = cv::Point(c[0], c[1]);
            circle( src, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
            int radius = c[2];
            circle( src, center, radius, cv::Scalar(0,255,0), 3, cv::LINE_AA);
        }
        imshow("Circles", src);
    }
    return EXIT_SUCCESS;
}