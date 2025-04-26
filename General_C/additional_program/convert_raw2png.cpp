#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Настройка под твоё разрешение
const int FRAME_WIDTH = 256;
const int FRAME_HEIGHT = 192;

bool convertYUYVtoJPG(const std::string& input_path, const std::string& output_path) {
    // Открываем raw файл
    std::ifstream file(input_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << input_path << std::endl;
        return false;
    }

    // Считываем данные
    std::vector<unsigned char> buffer(FRAME_WIDTH * FRAME_HEIGHT * 2);
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    file.close();

    // Создаём Mat из YUYV байтов
    cv::Mat yuyv(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC2, buffer.data());
    cv::Mat bgr;

    // Преобразуем YUYV → BGR
    cv::cvtColor(yuyv, bgr, cv::COLOR_YUV2BGR_YUY2);

    // Сохраняем как JPG
    if (!cv::imwrite(output_path, bgr)) {
        std::cerr << "Ошибка сохранения JPG: " << output_path << std::endl;
        return false;
    }

    std::cout << "Сохранено: " << output_path << std::endl;
    return true;
}

int main() {
    std::string input_folder = "./";  // или путь к папке с .raw
    std::string output_folder = "./converted_jpgs";
    fs::create_directory(output_folder);

    for (const auto& entry : fs::directory_iterator(input_folder)) {
        if (entry.path().extension() == ".raw") {
            std::string input_file = entry.path().string();
            std::string base_name = entry.path().stem().string();
            std::string output_file = output_folder + "/" + base_name + ".jpg";

            convertYUYVtoJPG(input_file, output_file);
        }
    }

    return 0;
}
