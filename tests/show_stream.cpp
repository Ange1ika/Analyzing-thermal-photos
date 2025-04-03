#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Открываем видеопоток с камеры
    cv::VideoCapture cap(0, cv::CAP_V4L2); // Используем V4L2 для Linux

    // Проверяем, открылся ли видеопоток
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть камеру" << std::endl;
        return -1;
    }

    // Устанавливаем параметры камеры
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 256);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 192);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V')); // Указываем YUYV
    cap.set(cv::CAP_PROP_FPS, 25);
    cap.set(cv::CAP_PROP_CONVERT_RGB, 0);

    cv::Mat frame;
    while (true) {
        // Читаем кадр
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Ошибка: Не удалось получить кадр" << std::endl;
            break;
        }

        // Проверяем размеры данных
        std::cout << "Frame size: " << frame.rows << "x" << frame.cols << " Channels: " << frame.channels() << std::endl;

        // Преобразуем YUYV в RGB
        cv::Mat frame_rgb;
        cv::cvtColor(frame, frame_rgb, cv::COLOR_YUV2RGB_YUY2);

        // Отображаем изображение в RGB
        cv::imshow("RGB Video", frame_rgb);

        // Выход по нажатию 'q'
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Освобождаем ресурсы
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
