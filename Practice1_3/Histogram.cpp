#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

cv::Mat src, graySrc;

cv::Mat BRGHist(cv::Mat src)
{
    std::vector<cv::Mat> bgr_planes;
    split( src, bgr_planes );
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange = { range };
    bool uniform = true, accumulate = false;
    cv::Mat b_hist, g_hist, r_hist;
    calcHist( &bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
    int hist_w = src.size().width, hist_h = src.size().height;
    int bin_w = cvRound( (double) hist_w/histSize );
    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );
    normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ),
              cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
              cv::Scalar( 255, 0, 0), 2, 8, 0  );
        line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ),
              cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
              cv::Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ),
              cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
              cv::Scalar( 0, 0, 255), 2, 8, 0  );
    }
    return histImage;
}

cv::Mat GrayHist(cv::Mat gray)
{
    std::vector<cv::Mat> gray_plane;
    split( gray, gray_plane );
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange = { range };
    bool uniform = true, accumulate = false;
    cv::Mat gray_hist;
    calcHist( &gray_plane[0], 1, 0, cv::Mat(), gray_hist, 1, &histSize, &histRange, uniform, accumulate );
    int hist_w = gray.size().width, hist_h = gray.size().height;
    int bin_w = cvRound( (double) hist_w/histSize );
    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );
    normalize(gray_hist, gray_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(gray_hist.at<float>(i-1)) ),
              cv::Point( bin_w*(i), hist_h - cvRound(gray_hist.at<float>(i)) ),
              cv::Scalar( 128, 128, 128), 2, 8, 0  );
    }
    
    return histImage;
}

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

int main(int argc, char** argv)
{
    bool binarize = false;
    std::cout << "Enter 0 for Histogram, 1 for Binarization" << std::endl;
    std::cin >> binarize;

    int input = -1;
    std::cout << "Enter 0 for Image, 1 for Video, 2 for Webcam" << std::endl;
    std::cin >> input;

    bool RGB = true;
    if(!binarize)
    {
        std::cout << "Enter 0 for Grayscale, 1 for RGB" << std::endl;
        std::cin >> RGB;
    }

    if(input == 0)
    {
        src = cv::imread("shrek.jpeg", cv::IMREAD_COLOR);
        graySrc = cv::imread("shrek.jpeg", cv::IMREAD_GRAYSCALE);
        if( src.empty() )
        {
            std::cout << "Error reading file" << std::endl;
            return -1;
        }

        if(binarize)
        {
            Binarize(graySrc, 110);
            cv::imshow("Binarized Image", graySrc);
            cv::waitKey();
        }
        else
        {
            if(RGB)
            {
                imshow("Shrek", src);
                imshow("Histogram", BRGHist(src));
            }
            else
            {
                imshow("Shrek", graySrc);
                imshow("Histogram", GrayHist(graySrc));
            }
            cv::waitKey();
        }
    }
    else
    {
        cv::VideoCapture cap;
        
        if(input == 1)
        {
            cap = cv::VideoCapture("shrek.mp4");
        }
        else
        {
            int deviceID = 0;             // 0 = open default camera
            int apiID = cv::CAP_ANY;      // 0 = autodetect default API
            cap.open(deviceID, apiID);
        }

        while(1)
        {
            if (!cap.read(src))
            {
                break;
            }

            if(binarize)
            {
                cv::cvtColor(src, graySrc, cv::COLOR_BGR2GRAY);
                Binarize(graySrc, 110);
                cv::imshow("Binarized Image", graySrc);
            }
            else
            {
                if(RGB)
                {
                    imshow("Shork", src);
                    imshow("Histogram", BRGHist(src));
                }
                else
                {
                    cv::cvtColor(src, graySrc, cv::COLOR_BGR2GRAY);
                    imshow("Shork", graySrc);
                    imshow("Histogram", GrayHist(graySrc));
                }
            }

            if(cv::waitKey(20) == 27) // Wait for esc
            { 
                break; 
            }
        }
    }
    
    return 0;
}