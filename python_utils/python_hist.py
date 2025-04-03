import matplotlib.pyplot as plt
import cv2
import numpy as np

img = cv2.imread("/home/angelika/Desktop/8_term/R_D/ref/termo_ref.png")


b, g, r = cv2.split(img)


b_hist = cv2.calcHist([b], [0], None, [256], [0, 256])
g_hist = cv2.calcHist([g], [0], None, [256], [0, 256])
r_hist = cv2.calcHist([r], [0], None, [256], [0, 256])


plt.figure(figsize=(12, 8)) 

plt.plot(b_hist, color='blue', label='Blue Channel', linewidth=2)
plt.plot(g_hist, color='green', label='Green Channel', linewidth=2)
plt.plot(r_hist, color='red', label='Red Channel', linewidth=2)


plt.xlabel('Pixel Intensity', fontsize=14, fontweight='bold')
plt.ylabel('Frequency', fontsize=14, fontweight='bold')
plt.title('Histogram for RGB Channels', fontsize=16, fontweight='bold')
plt.legend(fontsize=12)
plt.grid(True, which='both', axis='both', linestyle='--', linewidth=0.7)


plt.savefig("python_hist.png", dpi=300, bbox_inches='tight')

plt.show()
