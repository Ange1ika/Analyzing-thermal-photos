#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>  // Not experimental


using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void processImage(const string& inputPath, const string& outputPath) {
    Mat img = imread(inputPath, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "Could not read the image: " << inputPath << endl;
        return;
    }

    Mat binary_img;
    adaptiveThreshold(img, binary_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);

    // Mat blurred_img, image_back;
    // threshold(img, image_back, 10, 50, THRESH_BINARY);
    // GaussianBlur(image_back, blurred_img, Size(31, 31), 10);  // Размытие фона

    Mat dilated_edges;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(binary_img, dilated_edges, kernel, Point(-1, -1), 1);

    Mat inverted_img;
    bitwise_not(dilated_edges, inverted_img);

    Mat heatmap;
    GaussianBlur(inverted_img, heatmap, Size(11, 11), 4);
    

    Mat heatmap_air;
    GaussianBlur(inverted_img, heatmap_air, Size(51, 51), 70);
    

    Mat combined_heatmap = heatmap + 0.3 * heatmap_air;
    
    // Нормализация heatmap для визуализации
    Mat normalized_heatmap;
    normalize(combined_heatmap, normalized_heatmap, 0, 255, NORM_MINMAX);
    cvtColor(normalized_heatmap, normalized_heatmap, COLOR_GRAY2BGR);

    //imshow("normalized_heatmap", normalized_heatmap);
    
    imwrite(outputPath, normalized_heatmap);
    cout << "Processed and saved: " << outputPath << endl;
}

int main(int argc, char** argv) {
    // if (argc < 3) {
    //     cout << "Usage: " << argv[0] << " <input_folder> <output_folder>" << endl;
    //     return 1;
    // }

    //string inputFolder = argv[1];
    //string outputFolder = argv[2];
    string inputFolder = "/home/angelika/Desktop/8_term/R_D/C++/test_only";
    string outputFolder = "/home/angelika/Desktop/8_term/R_D/C++/thermal_outputs";
    if (!fs::exists(outputFolder)) {
        fs::create_directories(outputFolder);
    }

    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (fs::is_regular_file(entry) && (entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg" || entry.path().extension() == ".png")) {
            string inputPath = entry.path().string();
            string outputPath = outputFolder + "/" + entry.path().filename().string();
            processImage(inputPath, outputPath);
        }
    }

    return 0;
}