import cv2
import numpy as np

# Загрузка изображения
img = cv2.imread("ref/termo_ref.png")
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# Применение фильтра Собеля
sobel_x = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
sobel_y = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
edges = np.sqrt(sobel_x**2 + sobel_y**2)
edges = np.uint8(255 * edges / np.max(edges))  # Нормализация

# Оставить только самые яркие границы
_, edges_thresh = cv2.threshold(edges, 150, 255, cv2.THRESH_BINARY)

# Отображение результата
cv2.imshow("Edges", edges_thresh)
cv2.waitKey(0)
cv2.destroyAllWindows()
