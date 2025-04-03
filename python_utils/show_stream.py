import cv2
import numpy as np

# Открываем видеопоток с камеры
cap = cv2.VideoCapture(0, cv2.CAP_V4L2)  # Используем V4L2 для Linux

# Проверяем, открылся ли видеопоток
if not cap.isOpened():
    print("Ошибка: Не удалось открыть камеру")
    exit()

# Устанавливаем параметры камеры
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 256)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 192)
cap.set(cv2.CAP_PROP_MODE, cv2.CAP_MODE_YUYV)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter.fourcc('Y', 'U', 'Y', 'V'))  # Указываем YUYV
cap.set(cv2.CAP_PROP_FPS, 25)
cap.set(cv2.CAP_PROP_CONVERT_RGB, 0)

while True:
    # Читаем кадр
    ret, frame = cap.read()
    if not ret:
        print("Ошибка: Не удалось получить кадр")
        break

    # Проверяем форму данных
    print(frame.shape)  # Должно быть (1, height * width * 2) для YUYV

    # Преобразуем сырые данные YUYV в изображение
    height = 192
    width = 256
    frame_yuyv = frame.reshape((height, width * 2))  # Преобразуем в 2D-массив

    # Преобразуем YUYV в RGB
    frame_rgb = cv2.cvtColor(frame_yuyv, cv2.COLOR_YUV2RGB_YUY2)

    # Отображаем изображение в RGB
    cv2.imshow("RGB Video", frame_rgb)

    # Выход по нажатию 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Освобождаем ресурсы
cap.release()
cv2.destroyAllWindows()