#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <iomanip> 
#include <chrono>
#include <fstream>


#include "zhang_suen_thinning.h"
#include "DistanceMap8.h"
#include "DepthFirstSearch.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    
    Mat img = imread("/home/angelika/Desktop/8_term/R_D/python_utils/multipysics_crop.jpg", IMREAD_GRAYSCALE);
    if(img.empty()) {
        cout << "Could not read the image!" << endl;
        return 1;
    }

    // используем clahe (выравнивание гистограммы и увеличение контрастности)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(4);
    cv::Mat dst_clahe;
    clahe->apply(img, dst_clahe);

    // применим морфологию закрытие, убираем контур
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat closed;
    cv::morphologyEx(dst_clahe, closed, cv::MORPH_CLOSE, kernel);

    // Применяем фильтр Собеля по X и Y
    Mat grad_x, grad_y;
    Sobel(closed, grad_x, CV_16S, 1, 0, 3); // Градиент по X
    Sobel(closed, grad_y, CV_16S, 0, 1, 3); // Градиент по Y

    // Вычисляем полную величину градиента sqrt(Gx^2 + Gy^2)
    Mat grad_xy;
    Sobel(closed, grad_xy, CV_16S, 1, 1, 5); // С большим размером ядра работает лучше

    // Преобразуем в 8-битный формат
    Mat abs_grad_x, abs_grad_y, abs_grad_xy;
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    convertScaleAbs(grad_xy, abs_grad_xy);

    // Объединяем градиенты: G = 0.85*|Gx| + 0.5*|Gy| + 0.3*sqrt(Gx² + Gy²) + весовой коэффициент на нужный канал 
    Mat grad, sum_grad_xy;
    addWeighted(abs_grad_x, 0.6, abs_grad_y, 0.1, 0, grad);
    addWeighted(grad, 1, abs_grad_xy, 0.2, 0, sum_grad_xy);

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
    threshold(closed, binary_img, threshold_value, 255, THRESH_BINARY);

    Mat kernel_ellipse = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(binary_img, binary_img, MORPH_OPEN, kernel_ellipse);


    // Perform the distance transform algorithm

    // DFS
    Mat labels = Mat::zeros(binary_img.size(), CV_32S);
    int label = 0;

    for (int y = 0; y < binary_img.rows; y++) {
        for (int x = 0; x < binary_img.cols; x++) {
            if (binary_img.at<uchar>(y, x) == 255 && labels.at<int>(y, x) == 0) {
                label++;
                DepthFirstSearch(binary_img, labels, x, y, label);
            }
        }
    }

    //std::cout << "Количество масок:"  << label << std::endl;
    
    if (label > 1) {
        std::cout << "Разрыв резистивного слоя" << std::endl;
    }

    imshow("binary_11", binary_img);

    // Генерация случайных цветов для каждой метки

    vector<Vec3b> colors(label + 1, Vec3b(0, 0, 0)); // 0-й цвет - черный
    for (int i = 1; i <= label; i++) {
        colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256); // RGB случайные
    }

    // vector<Vec3b> colors = {
    //     {255, 0, 0},   // Красный
    //     {0, 255, 0},   // Зеленый
    //     {0, 0, 255},   // Синий
    //     {255, 255, 0}, // Голубой
    //     {255, 0, 255}, // Пурпурный
    //     {0, 255, 255}, // Желтый
    //     {128, 0, 0},   // Темно-красный
    //     {0, 128, 0},   // Темно-зеленый
    //     {0, 0, 128},   // Темно-синий
    //     {128, 128, 0}  // Темно-желтый
    // };

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
    std::cout << "kolvo masok" << label << std::endl;

    imshow("Color Mask", color_mask);


    auto start = std::chrono::high_resolution_clock::now();

    // Distance Transform (L2-норма, 3x3 маска)
    Mat dist_skel, dist_skel_bin;
    distanceTransform(binary_img, dist_skel, DIST_L2, 3);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Время выполнения алгоритма расчета дистанций: " << duration.count() << " секунд" << std::endl;



    auto start_0 = std::chrono::high_resolution_clock::now();

    cv::Mat dist_skel_custom = DistanceMap8(binary_img);
    auto end_0 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_0 = end_0 - start_0;
    std::cout << "Время выполнения алгоритма расчета дистанций кастомная: " << duration_0.count() << " секунд" << std::endl;


    
    Mat dist_map_norm, dist_skel_custom_bgr;
    normalize(dist_skel_custom, dist_map_norm, 0, 255, NORM_MINMAX, CV_8U);


    std::ofstream file("dist_matrix.txt");
    for (int i = 0; i < dist_skel_custom.rows; i++) {
        for (int j = 0; j < dist_skel_custom.cols; j++) {
            file << dist_skel_custom.at<int>(i, j) << " ";
        }
        file << std::endl;
    }
    file.close();
    std::cout << "Матрица сохранена в dist_matrix.txt\n";

        
    cvtColor(dist_map_norm, dist_skel_custom_bgr, COLOR_GRAY2BGR);
    imshow("dist_skel_custom_NEW", dist_skel_custom_bgr);



    // вычисляем среднее значение только для пикселей, отличных от нуля
    cv::Mat mask = dist_skel > 0;
    double mean_distance = cv::mean(dist_skel, mask)[0];
    std::cout << "Среднее расстояние до границ (без учета нулевых дистанций): " << mean_distance << std::endl;

    threshold(dist_skel, dist_skel_bin, mean_distance+5, 255, THRESH_BINARY);

    cv::Mat binary_img_bgr;
    Mat custom_skel;

    auto start_1 = std::chrono::high_resolution_clock::now();
    zhang_suen_thinning(binary_img, custom_skel);
    auto end_1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_1 = end_1 - start_1;
    std::cout << "Время выполнения алгоритма Zhang_Suen: " << duration_1.count() << " секунд" << std::endl;


    // нужно будет сравнить https://github.com/opencv/opencv/blob/master/modules/imgproc/src/distransform.cpp 

    imwrite("Distance_Transform_Image_custom.jpg", custom_skel);
    imshow("dist_skel_bin", dist_skel_bin);
    // Поиск локальных максимумов скелета
    custom_skel.convertTo(custom_skel, CV_32F);

    Mat skeleton_line = Mat::zeros(dist_skel_custom.size(), CV_8UC1);
    Mat closed_skel = Mat::zeros(dist_skel_custom.size(), CV_8UC1);


    cout << "skeleton_line size: " << skeleton_line.size() << endl;

    auto start_2 = std::chrono::high_resolution_clock::now();

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
    //imshow("dist_skel_custom", skeleton_line);
    auto end_2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_2 = end_2 - start_2;
    std::cout << "Время выполнения алгоритма расчета локальных максимумов: " << duration_2.count() << " секунд" << std::endl;


    Mat dilated_skel;
    cv::Mat kernel_skel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    Mat closed_skel_;
    cv::morphologyEx(skeleton_line, closed_skel_, MORPH_CLOSE, kernel_skel);

    cvtColor(binary_img, binary_img_bgr, cv::COLOR_GRAY2BGR);

    // Удваиваем среднее расстояние для поиска больших значений
    double threshold_dist = 2.2 * mean_distance;

    // Проходим по каждому пикселю и рисуем квадрат, если расстояние больше, чем threshold_dist
    for (int y = 1; y < dist_skel.rows - 1; y++) {
        for (int x = 1; x < dist_skel.cols - 1; x++) {
            if (dist_skel.at<float>(y, x) > threshold_dist) {
                //std::cout << "Найдены области с перегревом" << std::endl;
                int square_side = 10; 
                cv::rectangle(binary_img_bgr, cv::Point(x, y), cv::Point(x + square_side, y + square_side), cv::Scalar(0, 0, 255), -1); // Рисуем красный квадрат
            }
        }
    }

    imwrite("Image_with_Squares.jpg", binary_img_bgr);
    waitKey(0);
    return 0;

    }

