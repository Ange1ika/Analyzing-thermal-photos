import cv2
import numpy as np
from sklearn.cluster import KMeans

img = cv2.imread('/home/angelika/Desktop/8_term/R_D/ref/termo_ref.png')
green_channel = img[:, :, 1]  # 1 — индекс зеленого канала

green_norm = cv2.normalize(green_channel, None, 0, 255, cv2.NORM_MINMAX)


green_denoised = cv2.bilateralFilter(green_norm, 9, 75, 75)


_, binary = cv2.threshold(green_denoised, np.mean(green_denoised), 255, cv2.THRESH_BINARY)
contours, _ = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

kernel = np.ones((3, 3), np.uint8)
closed = cv2.morphologyEx(binary, cv2.MORPH_CLOSE, kernel, iterations=2)
if len(contours) > 1:
    print("Обнаружен разрыв дорожки")


# Получим координаты "горячих" пикселей
hot_pixels = np.column_stack(np.where(binary > 0))
kmeans = KMeans(n_clusters=2).fit(hot_pixels)
print("Центры кластеров:", kmeans.cluster_centers_)


for cnt in contours:
    area = cv2.contourArea(cnt)
    if area < 50:  # Эмпирический порог площади "шумовых" артефактов
        print("Обнаружена точечная неисправность")

result = img.copy()
cv2.drawContours(result, contours, -1, (0, 0, 255), 2)
cv2.imshow('Defects', result)
cv2.imwrite('defect_map.png', result)

cv2.waitKey(0)

