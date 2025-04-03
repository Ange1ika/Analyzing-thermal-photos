#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void getSpectrum(const Mat& img, Mat& amplitude, Mat& phase, Mat& magnitudeWithPhase) 
{
    Mat floatImg;
    img.convertTo(floatImg, CV_32F);

    // Применим БПФ (быстрое преобразование Фурье)
    Mat complexImg;
    Mat planes[2] = {Mat::zeros(img.size(), CV_32F), Mat::zeros(img.size(), CV_32F)};
    split(floatImg, planes);
    merge(planes, 2, complexImg);
    dft(complexImg, complexImg);

    // Получаем амплитуду и фазу
    split(complexImg, planes);
    amplitude = planes[0];  // амплитудная часть
    phase = planes[1];  // фазовая часть

    // Для визуализации амплитуды и фазы
    //cv::Mat amplitude_copy = amplitude.clone();  // Создаем копию матрицы
    //amplitude_copy = log(amplitude_copy + 1);   // Применяем логарифм
    phase = phase / (2 * M_PI);  // нормализация фазы

    // Собираем амплитудно-фазовый спектр
    magnitudeWithPhase = amplitude.clone();
    magnitudeWithPhase += phase * 255;  // комбинируем амплитуду и фазу для визуализации
}

int main() {
    // Загрузим изображение
    Mat img = imread("/home/angelika/Desktop/8_term/R_D/ref/termo_ref.png", IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "Could not read the image!" << endl;
        return -1;
    }

    // Получим спектры
    Mat amplitude, phase, magnitudeWithPhase;
    getSpectrum(img, amplitude, phase, magnitudeWithPhase);

    // Нормализуем и выводим результаты
    //normalize(amplitude, amplitude, 0, 255, NORM_MINMAX);
    //normalize(phase, phase, 0, 255, NORM_MINMAX);
    //normalize(magnitudeWithPhase, magnitudeWithPhase, 0, 255, NORM_MINMAX);

    // Преобразуем в 8-битные изображения для сохранения
    amplitude.convertTo(amplitude, CV_8UC1);
    phase.convertTo(phase, CV_8UC1);
    magnitudeWithPhase.convertTo(magnitudeWithPhase, CV_8UC1);

    // Сохраняем изображения
    imwrite("/home/angelika/Desktop/8_term/R_D/ref/amplitude_spectrum.jpg", amplitude);
    imwrite("/home/angelika/Desktop/8_term/R_D/ref/phase.jpg", phase);
    imwrite("/home/angelika/Desktop/8_term/R_D/ref/amplitude_phase_spectrum.jpg", magnitudeWithPhase);

    // Отображаем результаты
    imshow("Amplitude Spectrum", amplitude);
    imshow("Phase Spectrum", phase);
    imshow("Amplitude-Phase Spectrum", magnitudeWithPhase);

    waitKey(0);
    return 0;
}
