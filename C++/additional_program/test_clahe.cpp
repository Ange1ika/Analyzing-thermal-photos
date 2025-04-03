#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

// Apply CLAHE to an image
Mat applyCLAHE(const Mat& img, double clipLimit = 2.0, Size tileGridSize = Size(8, 8)) {
    Mat lab_img, clahe_img;
    cvtColor(img, lab_img, COLOR_BGR2Lab);

    vector<Mat> lab_planes;
    split(lab_img, lab_planes);

    Ptr<CLAHE> clahe = createCLAHE(clipLimit, tileGridSize);
    clahe->apply(lab_planes[0], lab_planes[0]);

    merge(lab_planes, lab_img);
    cvtColor(lab_img, clahe_img, COLOR_Lab2BGR);

    return clahe_img;
}

// Apply CLAHE to each channel separately
void applyCLAHEtoChannels(const Mat& img, vector<Mat>& clahe_channels) {
    vector<Mat> channels(3);
    split(img, channels);

    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    for (int i = 0; i < 3; ++i) {
        clahe->apply(channels[i], clahe_channels[i]);
    }
}

int main() {
    Mat img = imread("/home/angelika/Desktop/8_term/R_D/ref/termo_ref.png");
    if (img.empty()) {
        cout << "Could not read the image!" << endl;
        return 1;
    }

    // Convert to grayscale and apply CLAHE
    Mat gray_img, clahe_gray_img;
    cvtColor(img, gray_img, COLOR_BGR2GRAY);
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(gray_img, clahe_gray_img);

    // Convert grayscale CLAHE result to BGR for consistent concatenation
    Mat clahe_gray_bgr;
    cvtColor(clahe_gray_img, clahe_gray_bgr, COLOR_GRAY2BGR);

    // Apply CLAHE to original image
    Mat clahe_img = applyCLAHE(img);

    // Apply CLAHE to individual channels
    vector<Mat> clahe_channels(3);
    applyCLAHEtoChannels(img, clahe_channels);

    // Merge CLAHE-adjusted channels into one image
    Mat merged_clahe_img;
    merge(clahe_channels, merged_clahe_img);

    // Resize all images to the same size for visualization
    Size displaySize(400, 400);
    resize(img, img, displaySize);
    resize(clahe_img, clahe_img, displaySize);
    resize(merged_clahe_img, merged_clahe_img, displaySize);
    resize(clahe_channels[0], clahe_channels[0], displaySize);
    resize(clahe_channels[1], clahe_channels[1], displaySize);
    resize(clahe_channels[2], clahe_channels[2], displaySize);
    resize(clahe_gray_bgr, clahe_gray_bgr, displaySize);

    // Create blank images for padding
    Mat blank_image = Mat::zeros(displaySize, CV_8UC3);

    // Add labels to each image
    putText(img, "Original Image", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    putText(clahe_img, "CLAHE on Original", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    putText(merged_clahe_img, "Merged CLAHE Channels", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    putText(clahe_channels[0], "CLAHE on Blue Channel", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    putText(clahe_channels[1], "CLAHE on Green Channel", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    putText(clahe_channels[2], "CLAHE on Red Channel", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    //putText(clahe_gray_bgr, "CLAHE on Grayscale", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

    // Combine all images into one for comparison
    Mat top_row = Mat::ones(displaySize, CV_8UC3) * 255;
    Mat middle_row = Mat::ones(displaySize, CV_8UC3) * 255;
    Mat combined_image = Mat::ones(displaySize, CV_8UC3) * 255;
    hconcat(vector<Mat>{img, clahe_img, merged_clahe_img}, top_row);
    hconcat(vector<Mat>{clahe_channels[0], clahe_channels[1], clahe_channels[2]}, middle_row);
    //hconcat(vector<Mat>{blank_image, clahe_gray_bgr, blank_image}, bottom_row);
    //vconcat(vector<Mat>{top_row, middle_row, bottom_row}, combined_image);

    // Display and save combined result
    //imshow("CLAHE Comparison", combined_image);
    imwrite("/home/angelika/Desktop/8_term/R_D/top.png", top_row);
    imwrite("/home/angelika/Desktop/8_term/R_D/middle.png", middle_row);
    imwrite("/home/angelika/Desktop/8_term/R_D/clahe_gray.png", clahe_gray_img);
    imwrite("/home/angelika/Desktop/8_term/R_D/clahe_gray_1.png", clahe_gray_img);

    waitKey(0);
    return 0;
}
