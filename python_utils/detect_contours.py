import cv2
import numpy as np
import matplotlib.pyplot as plt

# Загрузка изображения
img = cv2.imread('/content/drive/MyDrive/Coco_grain/coco_train/images/EDME-w1_s14060.0_x1.6_e1717018285.png')
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# Пороговая сегментация
_, thresh = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)

# Сегментация с использованием метода Canny
edges = cv2.Canny(gray, 100, 200)

# Сегментация с использованием k-means
Z = img.reshape((-1, 3))
Z = np.float32(Z)
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
K = 2
_, labels, centers = cv2.kmeans(Z, K, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
centers = np.uint8(centers)
segmented_image = centers[labels.flatten()].reshape(img.shape)

# Сегментация с помощью оператора Собеля
sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)  # производная по X
sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)  # производная по Y
sobel_combined = cv2.magnitude(sobelx, sobely)
sobel_combined = np.uint8(np.absolute(sobel_combined))

# Визуализация
plt.figure(figsize=(15, 10))
plt.subplot(2, 3, 1), plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB)), plt.title('Оригинал')
plt.subplot(2, 3, 2), plt.imshow(thresh, cmap='gray'), plt.title('Пороговая сегментация')
plt.subplot(2, 3, 3), plt.imshow(edges, cmap='gray'), plt.title('Контуры (Canny)')
plt.subplot(2, 3, 4), plt.imshow(cv2.cvtColor(segmented_image, cv2.COLOR_BGR2RGB)), plt.title('K-means сегментация')
plt.subplot(2, 3, 5), plt.imshow(sobel_combined, cmap='gray'), plt.title('Сегментация (Собель)')
plt.tight_layout()
plt.show()
