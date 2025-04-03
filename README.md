# Analyzing Thermal Photos

This project is a computer vision research paper on **"Algorithm for quality control of electric film heaters for spacecraft using thermal imaging and camera data."**

The main folder **“C++”** contains scripts with basic logic of image processing. **Python_utils** includes the research part and various tests. **Multiphysics** contains a model of the heater to obtain synthetic data on which the algorithm is further tested.

---

### Initial tuning of the algorithm was performed based on this image:

<p align="center">
  <img src="./python_utils/multipysics_crop.jpg" alt="picture loading error">
</p>

---

### Several important stages are highlighted:
1. **Processing of a 16-bit dual-channel image** from a thermal imaging photo.
2. **Preprocessing and binarization** by local boundaries obtained from the gradient map.
3. **DFS for mask labeling and resistive layer gap verification**.
4. **Distance map calculation** (including testing the Zhang-Suen thinning algorithm).
5. **Local maxima extraction** — final skeletonization.
6. **Obtaining conclusions** based on the skeleton dispersion with respect to the heater mask.

---

<p align="center">
  <img src="./C++/output_images/Image_with_Squares.jpg" alt="picture loading error" width="30%" />
  <img src="./C++/output_images/Final_skeleton.jpg" alt="picture loading error" width="30%" />
  <img src="./C++/output_images/Distance_Transform_Image_custom.jpg" alt="picture loading error" width="30%" />
</p>

---

### Different directions for calculating the gradient with the Sobel operator:

<p align="center">
  <img src="./python_utils/RESULTS/own/Sobel_comparison.jpg" alt="picture loading error" width="60%" />
</p>

---

### **Performance Results of Algorithms:**

The following are the results of different algorithms used in the project:

| **Algorithm**                           | **Execution Time**    |
|-----------------------------------------|-----------------------|
| Distance calculation (by OpenCV)        | 0.00219966 seconds    |
| Custom distance calculation algorithm   | 0.0096455 seconds     |
| Zhang-Suen thinning algorithm           | 0.101661 seconds      |

It's a straightforward algorithm, which is also realized to obtain thermal photos based on digital photos.
