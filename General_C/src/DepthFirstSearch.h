#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <stack>

using namespace std;
using namespace cv;

int DepthFirstSearch(Mat& img, Mat& labels, int x, int y, int label) {
    stack<Point> s;
    s.push(Point(x, y));
    int area = 0;

    while (!s.empty()) {
        Point p = s.top();
        s.pop();

        if (p.x < 0 || p.y < 0 || p.x >= img.cols || p.y >= img.rows)
            continue;
        if (img.at<uchar>(p.y, p.x) != 255 || labels.at<int>(p.y, p.x) != 0)
            continue;

        labels.at<int>(p.y, p.x) = label;
        area++;

        s.push(Point(p.x + 1, p.y));
        s.push(Point(p.x - 1, p.y));
        s.push(Point(p.x, p.y + 1));
        s.push(Point(p.x, p.y - 1));
    }

    return area;
}
