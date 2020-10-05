#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

void Binarize(cv::Mat src, int threshold)
{
    for(int i = 0; i < src.size().height; i++)
    {
        for(int j = 0; j < src.size().width; j++)
        {
            if(src.at<uchar>(i,j) > threshold)
            {
                src.at<uchar>(i,j) = 255;
            }
            else
            {
                src.at<uchar>(i,j) = 0;
            }
        }
    }
}

int main(int argc, const char ** argv)
{
    bool run = true;
    cv::VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    cap.open(deviceID, apiID);

    cv::namedWindow("Original",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Brightness",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Canny",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Sobel",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Prewitt",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Binary",cv::WINDOW_AUTOSIZE);

    int brightness = 3;
    int lowThreshold = 30;
    const int max_lowThreshold = 100;
    const int ratio = 3;
    const int kernel_size = 3;
    int ddepth = CV_16S;
    int scale = 1;
    int ksize = -1;
    int delta = 0;
    float xPrewitt[] = {1, 1, 1, 0, 0, 0, -1, -1, -1};
    float yPrewitt[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

    cv::Mat xKernelPrewitt(3, 3, CV_32F, xPrewitt);
    cv::Mat yKernelPrewitt(3, 3, CV_32F, yPrewitt);


    while(run)
    {
        cv::Mat frame, grayFrame, blur, sobelX, sobelY, sobel, canny, prewitt, prewittX, prewittY;
        if (!cap.read(frame)) // if not success, break loop
        {
            std::cout<<"\n Cannot read the video file. \n";
            break;
        }
        imshow("Original", frame);
        cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

        //Brightness
        cv::Mat new_frame = cv::Mat::zeros(frame.size(), frame.type());
        for( int y = 0; y < frame.rows; y++ ) {
            for( int x = 0; x < frame.cols; x++ ) {
                for( int c = 0; c < frame.channels(); c++ ) {
                    new_frame.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>(brightness*frame.at<cv::Vec3b>(y,x)[c]);
                }
            }
        }
        imshow("Brightness", new_frame);

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::blur(grayFrame, blur, cv::Size(3,3));

        // Canny
        cv::Canny(blur, canny, lowThreshold, lowThreshold*ratio, kernel_size );
        imshow("Canny", canny);

        // Sobel
        cv::Sobel(blur, sobelX, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
        cv::Sobel(blur, sobelY, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(sobelX, sobelX);
        cv::convertScaleAbs(sobelY, sobelY);
        cv::addWeighted(sobelX, 0.5, sobelY, 0.5, 0, sobel);
        imshow("Sobel", sobel);

        // Prewitt
        cv::filter2D(blur, prewittX, ddepth, xKernelPrewitt);
        cv::filter2D(blur, prewittY, ddepth, yKernelPrewitt);
        cv::convertScaleAbs(prewittX, prewittX);
        cv::convertScaleAbs(prewittY, prewittY);
        cv::addWeighted(prewittX, 0.5, prewittY, 0.5, 0, prewitt);
        imshow("Prewitt", prewitt);

        // Ejes binarizados
        Binarize(grayFrame, 127);
        cv::Sobel(grayFrame, sobelX, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
        cv::Sobel(grayFrame, sobelY, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(sobelX, sobelX);
        cv::convertScaleAbs(sobelY, sobelY);
        cv::addWeighted(sobelX, 0.5, sobelY, 0.5, 0, sobel);        
        imshow("Binary", sobel);        

        char k = cv::waitKey(30);
        switch (k)
        {
            case 27:
                run = false;
                break;
            case 'u':
                brightness++;
                break;
            case 'd':
                brightness--;
                break;
            case 'b':
                lowThreshold-=5;
                scale++;
                delta++;
                break;
            case 'c':
                lowThreshold+=5;
                scale--;
                delta--;
                break;
            default:
                break;
        }
    }
    return 0;
}
