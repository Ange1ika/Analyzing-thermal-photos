#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <iomanip> 
#include <chrono>
#include <fstream>
#include <filesystem>

#include "zhang_suen_thinning.h"
#include "DistanceMap8.h"
#include "DepthFirstSearch.h"

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void processImage(const string& input_path, const string& output_path) {
    Mat img = imread(input_path, IMREAD_GRAYSCALE);
    if(img.empty()) {
        cout << "Could not read the image: " << input_path << endl;
        return;
    }

    // используем clahe (выравнивание гистограммы и увеличение контрастности)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(4);
    cv::Mat dst_clahe, closed;
    clahe->apply(img, dst_clahe);

    /// после CLAHE
    // cv::Mat gaussian_blur;
    // cv::GaussianBlur(dst_clahe, gaussian_blur, cv::Size(3, 3), 0.5);
    // // вместо простого закрытия
    // cv::Mat morph_processed, closed;
    // cv::morphologyEx(gaussian_blur, morph_processed, cv::MORPH_OPEN, 
    //                 cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    cv::morphologyEx(dst_clahe, closed, cv::MORPH_CLOSE, 
                cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    
    cv::Mat bilateral_filtered;
    cv::bilateralFilter(closed, bilateral_filtered, 9, 75, 75);

    // Применяем фильтр Собеля по X и Y
    Mat grad_x, grad_y;
    Sobel(bilateral_filtered, grad_x, CV_16S, 1, 0, 3); // Градиент по X
    Sobel(bilateral_filtered, grad_y, CV_16S, 0, 1, 3); // Градиент по Y

    // Вычисляем полную величину градиента sqrt(Gx^2 + Gy^2)
    Mat grad_xy;
    Sobel(bilateral_filtered, grad_xy, CV_16S, 1, 1, 5); // С большим размером ядра работает лучше

    // Преобразуем в 8-битный формат
    Mat abs_grad_x, abs_grad_y, abs_grad_xy;
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    convertScaleAbs(grad_xy, abs_grad_xy);

    // Объединяем градиенты: G = 0.85*|Gx| + 0.5*|Gy| + 0.3*sqrt(Gx² + Gy²) + весовой коэффициент на нужный канал 
    Mat grad, sum_grad_xy;
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.9, 0, grad);
    addWeighted(grad, 1, abs_grad_xy, 0.2, 0, sum_grad_xy);

    Mat abs_grad_x1, abs_grad_y1, abs_grad_xy1;
    convertScaleAbs(grad_x, abs_grad_x1);
    convertScaleAbs(grad_y, abs_grad_y1);
    convertScaleAbs(grad_xy, abs_grad_xy1);

    // Приводим все к одному размеру, если нужно (на всякий случай)
    resize(abs_grad_y1, abs_grad_y1, abs_grad_x1.size());
    resize(abs_grad_xy1, abs_grad_xy1, abs_grad_x1.size());

    // Объединяем изображения в один горизонтальный стек
    Mat sobel_stack;
    hconcat(std::vector<Mat>{abs_grad_x1, abs_grad_y1, abs_grad_xy1}, sobel_stack);

    cv::Mat kernel_ = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat closed_;
    cv::morphologyEx(sum_grad_xy, closed_, cv::MORPH_CLOSE, kernel_);

    // Увеличиваем границы методом дилатации
    Mat dilated_edges;
    dilate(closed_, dilated_edges, Mat(), Point(-1, -1), 1);

    // Вычисляем порог бинаризации на основе среднего значения по границам
    double sum_intensity = 0;
    int count = 0;
    for (int y = 0; y < dilated_edges.rows; y++) {
        for (int x = 0; x < dilated_edges.cols; x++) {
            if (dilated_edges.at<uchar>(y, x) > 0) {
                sum_intensity += closed.at<uchar>(y, x);
                count++;
            }
        }
    }
    double threshold_value = (count > 0) ? sum_intensity / count : 128;

    // Бинаризация исходного изображения
    Mat binary_img;
    threshold(closed, binary_img, 150, 255, ADAPTIVE_THRESH_MEAN_C);

    Mat kernel_ellipse = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(binary_img, binary_img, MORPH_OPEN, kernel_ellipse);

    Mat labels = Mat::zeros(binary_img.size(), CV_32S);
    int label = 0;
    map<int, int> label_areas;  // для хранения площади каждой маски
    
    for (int y = 0; y < binary_img.rows; y++) {
        for (int x = 0; x < binary_img.cols; x++) {
            if (binary_img.at<uchar>(y, x) == 255 && labels.at<int>(y, x) == 0) {
                label++;
                int area = DepthFirstSearch(binary_img, labels, x, y, label);
                label_areas[label] = area;
                cout << "Label " << label << " area: " << area << endl;
            }
        }
    }

    // Порог для минимальной площади маски
    const int min_area_threshold = 50;
    
    // Подсчитываем количество "существенных" масок
    int valid_labels = 0;
    for (const auto& entry : label_areas) {
        if (entry.second >= min_area_threshold) {
            valid_labels++;
        }
    }
    
    if (valid_labels > 1) {
        cout << "Разрыв резистивного слоя в файле: " << input_path << endl;
    }
    // Генерация случайных цветов для каждой метки
    vector<Vec3b> colors(label + 1, Vec3b(0, 0, 0));
    for (int i = 1; i <= label; i++) {
        colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
    }

    // Создание цветного изображения масок
    Mat color_mask(binary_img.size(), CV_8UC3, Scalar(255, 255, 255));
    for (int y = 0; y < binary_img.rows; y++) {
        for (int x = 0; x < binary_img.cols; x++) {
            int lbl = labels.at<int>(y, x);
            if (lbl > 0) {
                color_mask.at<Vec3b>(y, x) = colors[lbl];
            }
        }
    }

    // Distance Transform (L2-норма, 3x3 маска)
    Mat dist_skel, dist_skel_bin;
    distanceTransform(binary_img, dist_skel, DIST_L2, 3);

    cv::Mat dist_skel_custom = DistanceMap8(binary_img);

    Mat dist_map_norm, dist_skel_custom_bgr;
    normalize(dist_skel_custom, dist_map_norm, 0, 255, NORM_MINMAX, CV_8U);
    cvtColor(dist_map_norm, dist_skel_custom_bgr, COLOR_GRAY2BGR);

    // вычисляем среднее значение только для пикселей, отличных от нуля
    cv::Mat mask = dist_skel > 0;
    double mean_distance = cv::mean(dist_skel, mask)[0];

    threshold(dist_skel, dist_skel_bin, mean_distance+5, 255, THRESH_BINARY);

    // cv::Mat binary_img_bgr;
    // Mat custom_skel;
    // zhang_suen_thinning(binary_img, custom_skel);

    // Поиск локальных максимумов скелета
    //custom_skel.convertTo(custom_skel, CV_32F);
    Mat skeleton_line = Mat::zeros(dist_skel_custom.size(), CV_8UC1);

    for (int y = 1; y < dist_skel_custom.rows - 1; y++) {
        for (int x = 1; x < dist_skel_custom.cols - 1; x++) {
            float center = dist_skel_custom.at<float>(y, x);
            if (center > 0) {
                bool is_local_max = true;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dy == 0 && dx == 0) continue;
                        if (dist_skel_custom.at<float>(y + dy, x + dx) > center) {
                            is_local_max = false;
                            break;
                        }
                    }
                    if (!is_local_max) break;
                }
                if (is_local_max) {
                    skeleton_line.at<uchar>(y, x) = 255;
                }
            }
        }
    }

    Mat dilated_skel;
    cv::Mat kernel_skel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    Mat closed_skel_;
    cv::morphologyEx(skeleton_line, closed_skel_, MORPH_CLOSE, kernel_skel);

    //cvtColor(binary_img, binary_img_bgr, cv::COLOR_GRAY2BGR);

    // Удваиваем среднее расстояние для поиска больших значений
    double threshold_dist = 2.2 * mean_distance;

    // Проходим по каждому пикселю и рисуем квадрат, если расстояние больше, чем threshold_dist
    for (int y = 1; y < dist_skel.rows - 1; y++) {
        for (int x = 1; x < dist_skel.cols - 1; x++) {
            if (dist_skel.at<float>(y, x) > threshold_dist) {
                int square_side = 10; 
                cv::rectangle(closed_skel_, cv::Point(x, y), cv::Point(x + square_side, y + square_side), cv::Scalar(0, 0, 255), -1);
            }
        }
    }

    // Сохранение результатов
    fs::path input_p(input_path);
    string stem = input_p.stem().string();
    
    imwrite(output_path + "/" + stem + "_binary.jpg", binary_img);
    imwrite(output_path + "/" + stem + "_color_mask.jpg", color_mask);
    imwrite(output_path + "/" + stem + "_sobel.jpg", sum_grad_xy);
    imwrite(output_path + "/" + stem + "_bilateral.jpg", bilateral_filtered);
    imwrite(output_path + "/" + stem + "_dist_transform.jpg", dist_skel_custom_bgr);
    imwrite(output_path + "/" + stem + "sobel_stack.jpg", sobel_stack);

}

int main(int argc, char** argv) {
    string input_folder = "/home/angelika/Desktop/8_term/R_D/General_C/DATA/08_04";
    string output_folder = "/home/angelika/Desktop/8_term/R_D/General_C/DATA/final_23_04";
    

    // Создаем выходную папку, если ее нет
    if (!fs::exists(output_folder)) {
        fs::create_directory(output_folder);
    }

    // Обрабатываем все файлы во входной папке
    for (const auto& entry : fs::directory_iterator(input_folder)) {
        if (entry.is_regular_file()) {
            string input_path = entry.path().string();
            cout << "Processing: " << input_path << endl;
            processImage(input_path, output_folder);
        }
    }

    cout << "All images processed. Results saved to: " << output_folder << endl;
    return 0;
}