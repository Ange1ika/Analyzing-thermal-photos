#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

// Сохраняем RGB кадр как .jpg
#include <sys/stat.h>
#include <sys/types.h>

void create_directory_if_not_exists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        mkdir(path.c_str(), 0777);
    }
}


void save_rgb_jpg(const cv::Mat& rgb_frame, const std::string& path, int index) {
    std::string filename = path + "/frame_rgb_" + std::to_string(index) + ".jpg";

    //std::string filename = "frame_rgb_" + std::to_string(index) + ".jpg";
    if (!cv::imwrite(filename, rgb_frame)) {
        std::cerr << "Не удалось сохранить " << filename << std::endl;
        return;
    }
    std::cout << "Сохранён RGB кадр: " << filename << std::endl;
}

int main() {
    cv::VideoCapture cap(2, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть камеру" << std::endl;
        return -1;
    }

    int width = 256;
    int height = 192;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
    cap.set(cv::CAP_PROP_FPS, 25);
    cap.set(cv::CAP_PROP_CONVERT_RGB, 0); // захватываем в YUYV

    cv::Mat frame, rgb_frame;
    int saved_index = 0;
    std::string save_path = "/home/angelika/Desktop/8_term/R_D/General_C/DATA/08_04";
    create_directory_if_not_exists(save_path);



    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Ошибка: Не удалось получить кадр" << std::endl;
            break;
        }

        std::cout << "Shape: " << frame.rows << "x" << frame.cols
                  << " | channels: " << frame.channels()
                  << " | type: " << frame.type() << std::endl;

        // Преобразуем YUYV -> RGB (BGR в OpenCV)
        if (frame.channels() == 2) {
            cv::cvtColor(frame, rgb_frame, cv::COLOR_YUV2BGR_YUY2);
            cv::imshow("Converted RGB Video", rgb_frame);
        }

        char key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == 's') {
            save_rgb_jpg(rgb_frame, save_path, saved_index++);
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
