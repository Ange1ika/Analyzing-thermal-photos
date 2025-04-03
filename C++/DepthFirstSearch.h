#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <stack>

using namespace std;
using namespace cv;

void DepthFirstSearch(Mat& binary_img, Mat& labels, int x, int y, int current_label)
{
    int rows = binary_img.rows;
    int cols = binary_img.cols;
    stack<pair<int, int>> pixel_stack;
    pixel_stack.push({x, y});
    labels.at<int>(y, x) = current_label;

    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, 
    {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    while (!pixel_stack.empty()) {
        pair<int, int> top_pixel = pixel_stack.top();
        int cx = top_pixel.first;
        int cy = top_pixel.second;
        pixel_stack.pop();

        for (auto [dx, dy] : directions) {
            int nx = cx + dx;
            int ny = cy + dy;

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows 
                && binary_img.at<uchar>(ny, nx) == 255 && labels.at<int>(ny, nx) == 0) {
                labels.at<int>(ny, nx) = current_label;
                pixel_stack.push({nx, ny});
            }
        }
    }
}