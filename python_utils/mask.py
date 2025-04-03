import cv2
import numpy as np

# Загружаем тепловизионное изображение
image = cv2.imread('/home/angelika/Desktop/8_term/R_D/ref/termo_ref.png')
# Преобразование в цветовое пространство HSV (оттенок, насыщенность, значение)
hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

# Извлекаем канал насыщенности (S)
saturation = hsv[:, :, 1]

# Нормализуем насыщенность (от 0 до 255)
saturation = cv2.normalize(saturation, None, 0, 255, cv2.NORM_MINMAX)

# Порог для создания маски (подбери вручную, начнем с 150)
_, mask = cv2.threshold(saturation, 150, 255, cv2.THRESH_BINARY)

# Морфологическая обработка для удаления шумов
kernel = np.ones((3, 3), np.uint8)
mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
mask = cv2.dilate(mask, kernel, iterations=1)

# Наложим маску на оригинальное изображение
masked_image = image.copy()
masked_image[mask == 255] = [0, 0, 255]  # Красным отмечаем зоны с максимальной насыщенностью

# Сохраняем результаты
cv2.imwrite('resistive_layer_mask.jpg', mask)
cv2.imwrite('highlighted_resistive_layer.jpg', masked_image)

# Отображаем результаты
cv2.imshow('Original Image', image)
cv2.imshow('Saturation Mask', mask)
cv2.imshow('Highlighted Resistive Layer', masked_image)
cv2.waitKey(0)
cv2.destroyAllWindows()
