#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

void calculateHist(const Mat& img, Mat& hist, int histSize, const float* histRange) {
    calcHist(&img, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
    normalize(hist, hist, 0, img.rows, NORM_MINMAX, -1, Mat());
}

int main(int argc, char** argv)
{
    Mat img = imread("/home/angelika/Desktop/8_term/R_D/ref/termo_ref.png", IMREAD_COLOR);
    if(img.empty()) {
        cout << "Could not read the image!" << endl;
        return 1;
    }

    vector<Mat> channels(3), clahe_channels(3);
    vector<string> channel_names = {"Blue", "Green", "Red"};
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange[] = { range };

    Ptr<CLAHE> clahe = createCLAHE();
    for (int i = 0; i < 3; ++i) {
        extractChannel(img, channels[i], i);
        clahe->apply(channels[i], clahe_channels[i]);
    }

    
    vector<Mat> images_with_labels(3);
    int padding = 35;
    for (int i = 0; i < 3; ++i) {
        resize(channels[i], channels[i], Size(), 0.20, 0.20);
        Mat image_with_title(channels[i].rows + padding, channels[i].cols, channels[i].type(), Scalar(255, 255, 255));
        channels[i].copyTo(image_with_title(Rect(0, padding, channels[i].cols, channels[i].rows)));
        putText(image_with_title, channel_names[i], Point(10, 25), FONT_ITALIC, 1, Scalar(0, 0, 0), 1);
        images_with_labels[i] = image_with_title.clone();
    }


    Mat combined_image;
    hconcat(images_with_labels[0], images_with_labels[1], combined_image);
    hconcat(combined_image, images_with_labels[2], combined_image);
    imshow("Combined Channels", combined_image);
    imwrite("/home/angelika/Desktop/8_term/R_D/output_combined_channels.png", combined_image);

    Mat b_hist, g_hist, r_hist;
    calculateHist(channels[0], b_hist, histSize, *histRange);
    calculateHist(channels[1], g_hist, histSize, *histRange);
    calculateHist(channels[2], r_hist, histSize, *histRange);

    int hist_w = 512, hist_h = 120;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
    

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
             Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
             Scalar(0, 0, 255), 2, 8, 0);
    }

    // for (int i = 0; i < histSize; i += 50) {
    //     putText(histImage, to_string(i), Point(bin_w * i, hist_h - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    // }


    // imshow("calcHist Demo", histImage);
    // imwrite("/home/angelika/Desktop/8_term/R_D/output_hist_normalize.png", histImage);


    vector<Mat> clahe_images_with_labels(3);
    for (int i = 0; i < 3; ++i) {
        resize(clahe_channels[i], clahe_channels[i], Size(), 0.20, 0.20);
        Mat image_with_title(channels[i].rows + padding, channels[i].cols, channels[i].type(), Scalar(255, 255, 255));
        clahe_channels[i].copyTo(image_with_title(Rect(0, padding, channels[i].cols, channels[i].rows)));
        putText(image_with_title, channel_names[i] + " CLAHE", Point(10, 25), FONT_ITALIC, 1, Scalar(0, 0, 0), 1);
        clahe_images_with_labels[i] = image_with_title.clone();
    }


    Mat combined_clahe_image;
    hconcat(clahe_images_with_labels[0], clahe_images_with_labels[1], combined_clahe_image);
    hconcat(combined_clahe_image, clahe_images_with_labels[2], combined_clahe_image);
    imshow("CLAHE Combined Channels", combined_clahe_image);
    imwrite("/home/angelika/Desktop/8_term/R_D/output_combined_channels_clahe.png", combined_clahe_image);


    Mat merged_clahe;
    vector<Mat> merged_channels = {clahe_channels[0], clahe_channels[1], clahe_channels[2]};
    merge(merged_channels, merged_clahe);
    imshow("CLAHE Merged Image", merged_clahe);
    imwrite("/home/angelika/Desktop/8_term/R_D/output_merged_clahe.png", merged_clahe);

    waitKey();
    return 0;
}
