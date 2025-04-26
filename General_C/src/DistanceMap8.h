#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;


//Здесь реализован подход Chamfer Distance Transform

Mat DistanceMap8(const Mat& binary_img) {
    int w = binary_img.rows;
    int h = binary_img.cols;

    cv::Mat B = cv::Mat::zeros(binary_img.size(), CV_32SC1);
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            if (binary_img.at<uchar>(x, y) == 0) { 
                B.at<int>(x, y) = 0;
            }
        }
    }

    // Прямой проход (слева направо, сверху вниз)
    for (int x = 1; x < w - 1; x++) {
        for (int y = 1; y < h - 1; y++) {
            if (binary_img.at<uchar>(x, y) != 0) {  
                B.at<int>(x, y) = min({
                    B.at<int>(x - 1, y) + 10,      
                    B.at<int>(x, y - 1) + 10,      
                    B.at<int>(x - 1, y - 1) + 14,   
                    B.at<int>(x - 1, y + 1) + 14   
                });
            }
        }
    }

    // Обратный проход (справа налево, снизу вверх)
    for (int x = w - 2; x >= 1; x--) {
        for (int y = h - 2; y >= 1; y--) {
            if (binary_img.at<uchar>(x, y) != 0) { 
                B.at<int>(x, y) = min({
                    B.at<int>(x + 1, y) + 10,       
                    B.at<int>(x, y + 1) + 10,       
                    B.at<int>(x + 1, y + 1) + 14,   
                    B.at<int>(x + 1, y - 1) + 14,   
                    B.at<int>(x, y)                
                });
            }
        }
    }

    return B;
}


