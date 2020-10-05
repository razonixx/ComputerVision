#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

void fourier(cv::Mat image, bool shift)
{
    // 2 matrices to hold the Fourier transform value, one matrix for the real component and one matrix for the imaginary component
    cv::Mat planes[] = {cv::Mat_<float>(image), cv::Mat::zeros(image.size(), CV_32F)};
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);

    cv::dft(complexImg, complexImg);

    // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))
    // Split the DFT into 2 planes, one real and one imaginary
    cv::split(complexImg, planes);

    cv::Mat magnitudeMatrix; // Output for magnitude operation
    cv::magnitude(planes[0], planes[1], magnitudeMatrix);
    magnitudeMatrix += cv::Scalar::all(1);
    cv::log(magnitudeMatrix, magnitudeMatrix);

    if(shift)
    {
        int cx = magnitudeMatrix.cols/2;
        int cy = magnitudeMatrix.rows/2;

        // rearrange the quadrants of Fourier image
        // so that the origin is at the image center
        cv::Mat tmp;
        cv::Mat q0(magnitudeMatrix, cv::Rect(0, 0, cx, cy));
        cv::Mat q1(magnitudeMatrix, cv::Rect(cx, 0, cx, cy));
        cv::Mat q2(magnitudeMatrix, cv::Rect(0, cy, cx, cy));
        cv::Mat q3(magnitudeMatrix, cv::Rect(cx, cy, cx, cy));

        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);

        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);
    }

    cv::normalize(magnitudeMatrix, magnitudeMatrix, 0, 1, cv::NORM_MINMAX);

    imshow("spectrum magnitude", magnitudeMatrix);
    cv::waitKey();
}

int main(int argc, const char** argv) {
    cv::Mat src = cv::imread("shrek.jpeg", cv::IMREAD_COLOR);
    cv::Mat graySrc = cv::imread("shrek.jpeg", cv::IMREAD_GRAYSCALE);

    cv::GaussianBlur(src, src, cv::Size(0, 0), 10);
    cv::imshow("Gauss", src);
    cv::waitKey();

    //cv::GaussianBlur(graySrc, graySrc, cv::Size(0, 0), 10);
    fourier(graySrc, false);

    return 0;
}