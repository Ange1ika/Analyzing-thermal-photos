#include <opencv2/opencv.hpp>
#include <iostream>
#include <numeric>

void thinning_iteration(cv::Mat &img, int iter) {
    cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

    for (int i = 1; i < img.rows - 1; i++) {
        for (int j = 1; j < img.cols - 1; j++) {
            if (img.at<uchar>(i, j) == 0) continue;

            int neighbors[8] = {
                img.at<uchar>(i-1, j), img.at<uchar>(i-1, j+1),
                img.at<uchar>(i, j+1), img.at<uchar>(i+1, j+1),
                img.at<uchar>(i+1, j), img.at<uchar>(i+1, j-1),
                img.at<uchar>(i, j-1), img.at<uchar>(i-1, j-1)
            };

            int transitions = 0;
            for (int k = 0; k < 8; k++) {
                if (neighbors[k] == 0 && neighbors[(k+1) % 8] == 255)
                    transitions++;
            }

            int num_white = std::accumulate(std::begin(neighbors), std::end(neighbors), 0) / 255;

            if (num_white >= 2 && num_white <= 6 &&
                transitions == 1 &&
                ((iter == 0 && (neighbors[0] * neighbors[2] * neighbors[4] == 0) &&
                              (neighbors[2] * neighbors[4] * neighbors[6] == 0)) ||
                 (iter == 1 && (neighbors[0] * neighbors[2] * neighbors[6] == 0) &&
                              (neighbors[0] * neighbors[4] * neighbors[6] == 0)))) 
            {
                marker.at<uchar>(i, j) = 255;
            }
        }
    }

    img &= ~marker;
}

void zhang_suen_thinning(const cv::Mat &src, cv::Mat &dst) {
    CV_Assert(src.type() == CV_8UC1);
    
    cv::threshold(src, dst, 127, 255, cv::THRESH_BINARY); // Приводим к бинарному виду

    cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    cv::Mat diff;
    int threshold = 1;  // Минимальное число изменений, при которых алгоритм продолжает работу

    do {
        dst.copyTo(prev);
        thinning_iteration(dst, 0);
        thinning_iteration(dst, 1);
        cv::absdiff(dst, prev, diff);
    } while (cv::countNonZero(diff) > threshold); // Останавливаемся, если изменений слишком мало
}

